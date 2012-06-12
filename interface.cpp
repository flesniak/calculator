#include <iostream>

#include "interface.h"
#include "parser.h"

//Linux specific
#if defined(__GNUC__) || defined(__MINGW32__)
  #include <termios.h>
  #include <unistd.h>
  #include <cstdio>

  char getch() {
    return getchar();
  }
#endif

#ifdef _WIN32
  #include <conio.h>
#endif

interface::interface() : p_poll(true) {
  p_commandMap["help"] = displayHelp;
  p_commandMap["test"] = runTest;
  p_commandMap["exit"] = exitProgram;
  p_commandMap["quit"] = exitProgram;
  p_commandMap[""]     = noCommand;
}

int interface::talk() {
#if defined(__GNUC__) || defined(__MINGW32__)
  //Init shell interface
  static struct termios oldt, newt;
  tcgetattr( STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr( STDIN_FILENO, TCSANOW, &newt);
#endif

  //Create parser object
  p_parse = new parser;
  p_parse->setDebug(true); //debugging true in development phase

  //Welcome user, Help
  cout << "Calculate " << version << endl;
  cout << "Enter expression. You may type \"help\"." << endl << "> ";

  char c[3];
  unsigned char charIndex = 0;
  p_commandHistory.push_back(string()); //prepare an empty prompt
  p_commandHistoryIterator = p_commandHistory.begin();
  p_commandIterator = p_commandHistory.back().begin();

  //MAIN LOOP
  while( p_poll ) {
    c[charIndex] = getch();
    switch( c[charIndex] ) {
      case 10  : putchar(10); //enter
                 processLine();
                 break;
      case 127 : deleteCharacter();
                 break;
      case 27  : if( charIndex == 0 )
                   charIndex++;
                 break;
      case 91  : if( charIndex == 1 )
                   charIndex++;
                 break;
      case 65  : charIndex = 0;
                 showPreviousExpression();
                 break;
      case 66  : charIndex = 0;
                 showNextExpression();
                 break;
      case 67  : charIndex = 0;
                 moveCursorRight();
                 break;
      case 68  : charIndex = 0;
                 moveCursorLeft();
                 break;
      default :  insertCharacter(c[charIndex]);
                 break;
    }
  }

  //It's done
  tcsetattr( STDIN_FILENO, TCSANOW, &oldt);
  cout << "Goodbye" << endl;
  return 0;
}

void interface::parse(const string& str) {
  if( p_parse->parse(str) == parser::complete )
     cout << str << " = " << p_parse->result() << endl;
  else
    cout << p_parse->getError() << endl;
}

void interface::help() {

}

void interface::test() {

}

void interface::processLine() {
  command cmd = parseLine;
  if( p_commandMap.count(*p_commandHistoryIterator) )
    cmd = p_commandMap[*p_commandHistoryIterator];
  switch( cmd ) {
    case displayHelp : help();
                       break;
    case runTest     : test();
                       break;
    case exitProgram : p_poll = false;
                       break;
    case noCommand   : break;
    default          : parse(*p_commandHistoryIterator);
  }
  if( p_commandHistory.back() != string() )
    p_commandHistory.push_back(string());
  p_commandHistoryIterator = p_commandHistory.end()-1;
  p_commandIterator = (*p_commandHistoryIterator).begin();
  if( p_poll )
    std::cout << "> ";
}

void interface::clearLine() {
  putchar('\r');
  for(int n = 0; n < (*p_commandHistoryIterator).length()+2; n++)
    putchar(' ');
  putchar('\r');
}

void interface::showPreviousExpression() {
  if( p_commandHistoryIterator > p_commandHistory.begin() ) {
    clearLine();
    p_commandHistoryIterator--;
    p_commandIterator = (*p_commandHistoryIterator).end();
    std::cout << "> " << *p_commandHistoryIterator;
  }
}

void interface::showNextExpression() {
  if( p_commandHistoryIterator < p_commandHistory.end()-1 ) {
    clearLine();
    ++p_commandHistoryIterator;
    p_commandIterator = (*p_commandHistoryIterator).end();
    std::cout << "> " << *p_commandHistoryIterator;
  }
}

void interface::moveCursorRight() {
  if( p_commandIterator < (*p_commandHistoryIterator).end() ) {
    putchar(*p_commandIterator);
    p_commandIterator++;
  }
}

void interface::moveCursorLeft() {
  if( p_commandIterator > (*p_commandHistoryIterator).begin() ) {
    putchar('\b');
    p_commandIterator--;
  }
}

void interface::insertCharacter(char c) {
  p_commandIterator = (*p_commandHistoryIterator).insert(p_commandIterator,c);
  string::iterator it = p_commandIterator;
  for(; it < (*p_commandHistoryIterator).end(); it++)
    std::cout << *it;
  p_commandIterator++;
  for(; it > p_commandIterator; it--)
    putchar('\b');
}

void interface::deleteCharacter() {
  if( p_commandIterator > p_commandHistory.back().begin() ) {
    std::cout << "\b \b";
    p_commandIterator--;
    p_commandIterator = p_commandHistory.back().erase(p_commandIterator);
  }
}
