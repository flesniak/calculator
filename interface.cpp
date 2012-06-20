#include <iostream>
#include <cmath>
#include <iomanip>

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
  p_commandMap["help"]  = displayHelp;
  p_commandMap["?"]  = displayHelp;
  p_commandMap["test"]  = runTest;
  p_commandMap["exit"]  = exitProgram;
  p_commandMap["quit"]  = exitProgram;
  p_commandMap["debug"] = toggleDebug;
  p_commandMap[""]      = noCommand;

  p_commandHelpMap[displayHelp] = "Shows this help screen";
  p_commandHelpMap[runTest] = "Runs several calculations to test the parser class";
  p_commandHelpMap[exitProgram] = "Exits the program";
  p_commandHelpMap[toggleDebug] = "Toggles algorithm debugging (you may want to use this!)";

  testExpression te;
  te.expression = "4--3";
  te.result     = 7;
  te.help       = "Negation and subtraction can be distinguished";
  p_testExpressions.push_back(te);
  te.expression = "-4*sin(.5pi)";
  te.result     = -4;
  te.help       = "sin, cos, tan, arcsin, arccos, arctan can be accessed, constants pi and e exist";
  p_testExpressions.push_back(te);
  te.expression = "2e*4E5";
  te.result     = 2174625.463;
  te.help       = "Lowercase e is Euler's number, uppercase E will do *10^X";
  p_testExpressions.push_back(te);
  te.expression = "2pisin(2)";
  te.result     = 5.71328;
  te.help       = "Left-out multiplication signs will be inserted";
  p_testExpressions.push_back(te);
  te.expression = "(4+3)^-.5";
  te.result     = 0.377964;
  te.help       = "Parentheses and power are supported";
  p_testExpressions.push_back(te);
  te.expression = "abs(-sqrt(ans))";
  te.result     = 0.614788;
  te.help       = "Square root, absolute value and last result (ans)";
  p_testExpressions.push_back(te);

  cout.precision(16);
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

  //Welcome user
  cout << "Calculate " << version << endl;
  cout << "Enter expression. You may type \"help\"." << endl << "> ";

  char c[4];
  unsigned char charIndex = 0;
  p_commandHistory.push_back(string()); //prepare an empty prompt
  p_commandHistoryIterator = p_commandHistory.begin();
  p_commandIterator = p_commandHistory.back().begin();

  //MAIN LOOP
  while( p_poll ) {
    c[charIndex] = getch();
    switch( c[charIndex] ) {
      case 10  : putchar(10); //newline
                 processLine();
                 break;
      case 127 : deleteCharacter();
                 break;
      case 27  : if( charIndex == 0 )
                   charIndex++;
                 else
                   charIndex = 0;
                 break;
      case 91  : if( charIndex == 1 )
                   charIndex++;
                 else {
                   insertCharacter(c[charIndex]);
                   charIndex = 0;
                 }
                 break;
      case 79  : if( charIndex == 1 )
                   charIndex++;
                 else {
                   insertCharacter(c[charIndex]);
                   charIndex = 0;
                 }
                 break;
      case 51  : if( charIndex == 2 )
                   charIndex++;
                 else {
                   insertCharacter(c[charIndex]);
                   charIndex = 0;
                 }
                 break;
      case 65  : if( charIndex == 2 )
                   showPreviousExpression();
                 else
                   insertCharacter(c[charIndex]);
                 charIndex = 0;
                 break;
      case 66  : if( charIndex == 2 )
                   showNextExpression();
                 else
                   insertCharacter(c[charIndex]);
                 charIndex = 0;
                 break;
      case 67  : if( charIndex == 2 )
                   moveCursorRight();
                 else
                   insertCharacter(c[charIndex]);
                 charIndex = 0;
                 break;
      case 68  : if( charIndex == 2 )
                   moveCursorLeft();
                 else
                   insertCharacter(c[charIndex]);
                 charIndex = 0;
                 break;
      case 72  : if( charIndex == 2 )
                   moveCursorPos1();
                 else
                   insertCharacter(c[charIndex]);
                 charIndex = 0;
                 break;
      case 70  : if( charIndex == 2 )
                   moveCursorEnd();
                 else
                   insertCharacter(c[charIndex]);
                 charIndex = 0;
                 break;
      case 126  : if( charIndex == 3 )
                   deleteCharacterReverse();
                 else
                   insertCharacter(c[charIndex]);
                 charIndex = 0;
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
  cout << "Built-in commands:" << endl;
  string str;
  for(map<string,command>::iterator it = p_commandMap.begin(); it != p_commandMap.end(); it++)
    if( !it->first.empty() )
      cout << setw(7) << it->first << " - " << p_commandHelpMap[it->second] << endl;
  cout << endl << "Everything else will be parsed as an mathematical expression, based on the following notation:" << endl;
  for(list<testExpression>::iterator it = p_testExpressions.begin(); it != p_testExpressions.end(); it++) {
    cout << setw(15) << (*it).expression << " = " << setw(12) << (*it).result << " | " << (*it).help << endl;
  }
}

void interface::test() {
  for(list<testExpression>::iterator it = p_testExpressions.begin(); it != p_testExpressions.end(); it++) {
    if( p_parse->parse((*it).expression) == parser::complete ) {
      if( fabs( p_parse->result() - (*it).result ) < fabs((*it).result)*0.0001 )
        cout << (*it).expression << " = " << (*it).result << " OK!" << endl;
      else
        cout << (*it).expression << " shall equal " << (*it).result << " but parser returned " << p_parse->result() << endl;
    }
    else
      cout << (*it).expression << " failed: " << p_parse->getError() << endl;
  }
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
    case toggleDebug : p_parse->setDebug(!p_parse->getDebug());
                       cout << "Debugging information " << (p_parse->getDebug() ? "enabled" : "disabled") << endl;
                       break;
    case noCommand   : break;
    default          : parse(*p_commandHistoryIterator);
  }
  if( p_commandHistory.back() != string() )
    p_commandHistory.push_back(string());
  p_commandHistoryIterator = p_commandHistory.end()-1;
  p_commandIterator = (*p_commandHistoryIterator).begin();
  if( p_poll )
    cout << "> ";
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
    cout << "> " << *p_commandHistoryIterator;
  }
}

void interface::showNextExpression() {
  if( p_commandHistoryIterator < p_commandHistory.end()-1 ) {
    clearLine();
    ++p_commandHistoryIterator;
    p_commandIterator = (*p_commandHistoryIterator).end();
    cout << "> " << *p_commandHistoryIterator;
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

void interface::moveCursorPos1() {
  while( p_commandIterator > (*p_commandHistoryIterator).begin() ) {
    putchar('\b');
    p_commandIterator--;
  }
}

void interface::moveCursorEnd() {
  while( p_commandIterator < (*p_commandHistoryIterator).end() ) {
    putchar(*p_commandIterator);
    p_commandIterator++;
  }
}

void interface::insertCharacter(char c) {
  if( c < 0 ) //Negative characters (unsigned >127) represent extended characters that spread over multiple bytes and break out terminal
    return;
  p_commandIterator = (*p_commandHistoryIterator).insert(p_commandIterator,c);
  string::iterator it = p_commandIterator;
  for(; it < (*p_commandHistoryIterator).end(); it++)
    cout << *it;
  p_commandIterator++;
  for(; it > p_commandIterator; it--)
    putchar('\b');
}

void interface::deleteCharacter() {
  if( p_commandIterator > (*p_commandHistoryIterator).begin() ) {
    putchar('\b');
    p_commandIterator--;
    p_commandIterator = (*p_commandHistoryIterator).erase(p_commandIterator);
    string::iterator it = p_commandIterator;
    for(; it < (*p_commandHistoryIterator).end(); it++)
      putchar(*it);
    cout << " \b";
    for(; it > p_commandIterator; it--)
      putchar('\b');
  }
}

void interface::deleteCharacterReverse() {
  if( p_commandIterator < (*p_commandHistoryIterator).end() ) {
    p_commandIterator = (*p_commandHistoryIterator).erase(p_commandIterator);
    string::iterator it = p_commandIterator;
    for(; it < (*p_commandHistoryIterator).end(); it++)
      putchar(*it);
    cout << " \b";
    for(; it > p_commandIterator; it--)
      putchar('\b');
  }
}

