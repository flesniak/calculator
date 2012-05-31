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
    if( parse->parse(expression) == parser::complete )
       cout << expression << " = " << parse->result() << endl;
    else
      cout << parse->getError() << endl;
  }

  cout << "Goodbye" << endl;

  //It's done
  return 0;
}
