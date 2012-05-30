#include <iostream>
#include <string>

#include "interface.h"
#include "parser.h"

using namespace std;

int interface::talk() {
  //Welcome user
  cout << "Calculate " << version << std::endl;

  //Create parser object
  parser *parse = new parser;
  parse->setDebug(true); //debugging true in development phase
  string expression;

  //Help
  cout << "Enter expression. Type \"exit\" to do so. \"debug\" toggles calculation information." << endl;

  //Main loop
  while( 1 ) {
    std::cout << "> ";
    cin >> expression;
    if( expression == "exit" )
      break;
    if( expression == "debug" ) {
      parse->setDebug(!parse->getDebug()); //toggles debug output
      continue;
    }
    switch( parse->parse(expression) ) {
      case parser::complete : cout << expression << " = " << parse->result() << endl;
                              break;
      case parser::error    : cout << "Syntax error near: " << parse->getError() << endl;
                              break;
      default               : cout << "Internal parser error" << endl;
    }
  }

  cout << "Goodbye" << endl;

  //It's done
  return 0;
}
