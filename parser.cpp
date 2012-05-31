#include "parser.h"

#include <sstream>
#include <iostream>
#include <cmath>

parser::parser() {
  clear();
  p_debug = false;

  //Initialize operator map
  p_opmap["+"] = operators::plus;
  p_opmap["-"] = operators::minus;
  p_opmap["*"] = operators::times;
  p_opmap["/"] = operators::divide;
  p_opmap["sin"] = operators::sin;
  p_opmap["cos"] = operators::cos;
  p_opmap["tan"] = operators::tan;
  p_opmap["arcsin"] = operators::arcsin;
  p_opmap["arccos"] = operators::arccos;
  p_opmap["arctan"] = operators::arctan;
  p_opmap["SIN"] = operators::sin;
  p_opmap["COS"] = operators::cos;
  p_opmap["TAN"] = operators::tan;
  p_opmap["ARCSIN"] = operators::arcsin;
  p_opmap["ARCCOS"] = operators::arccos;
  p_opmap["ARCTAN"] = operators::arctan;
  p_opmap["("] = operators::lbracket;
  p_opmap[")"] = operators::rbracket;
  p_opmap["pi"] = operators::pi;
  p_opmap["Pi"] = operators::pi;
  p_opmap["PI"] = operators::pi;
}

//parse expression
parser::state parser::parse(const string& expression) {
  debug("parse() initializing to parse "+expression);
  clear(); //make sure no data from previous parsing is left
  p_state = running;
  p_expression = expression; //we won't modify expression
  double temp;
  operators::ops op;
  bool needoperator = false; //the needoperator value helps deciding between +/- signs or operators ( and to process "missing" *'s)

  //Shunting-yard algorithm
  while( p_state == running && !p_expression.empty() ) { //process p_expression until it is empty or we encouter a p_state change
    debug("parse() parsing expression "+p_expression);
    //Process input
    if( !needoperator && extractNumber(temp) ) { //we won't try to read two numbers in a row, if extractNumber fails, try to exractOperator
      if( !p_operators.empty() && p_operators.top() > operators::operatorCount && p_operators.top() != operators::rbracket ) {
        debug("parse() missing operator before %v1",temp);
        p_errorstring = "Operator missing";
        p_state = syntaxerror;
        return p_state;
      }
      p_numbers.push(temp);
      needoperator = true;
      debug("parse() found number %v1",p_numbers.top());
    }
    else {
      if( extractOperator(op) ) {
        while( p_state == running && !p_operators.empty() && op != operators::lbracket && op != operators::rbracket && p_operators.top() >= op ) { //process operators with higher priority, parentheses need special care
          debug("parse() preferring operator %o1 over %o2",p_operators.top(),op);
          processOperator();
        }
        if( p_state != running ) //if processOperator encountered an error, return
          return p_state;
        if( (op > operators::operatorCount || op == operators::lbracket) && p_numbers.size()-1 == p_operators.size() ) { //prepend operators::times to functions and parentheses where it is left out
          debug("parse() function without preceeding operator, inserting operator %o1",operators::times);
          p_operators.push(operators::times);
        }
        p_operators.push(op);
        needoperator = false;
        debug("parse() operator %o1 found",p_operators.top());
        if( op == operators::rbracket ) //Immediately process parentheses
          processOperator();
      }
      else {
        p_state = syntaxerror;
        p_errorstring = "unable to parse " + p_expression;
        return p_state;
      }
    }
  }
  debug("parse() finished, computing remaining operators/numbers");

  //Expression is parsed, we now just have to process all remaining operators
  while( p_state == running && !p_operators.empty() )
    processOperator();

  if( !p_operators.empty() && !p_numbers.empty() && p_numbers.size() > 1 ) {
    p_errorstring = "Too few operators!";
    p_state = syntaxerror;
    return p_state;
  }

  if( p_state == running )
    p_state = complete;
  p_expression = expression; //p_expression got mutilated during execution, save it for our getError() method
  return p_state;
}

//take operator (or function) from stack, try to parse it and push the result onto p_numbers
void parser::processOperator() {
  if( p_operators.empty() ) {
    debug("processOperator() p_operators empty");
    p_state = internalerror;
    return;
  }
  double temp1, temp2;
  switch( p_operators.top() ) {
    case operators::plus    : if( p_numbers.size() < 2 ) {
                                debug("processOperator() plus: not enough numbers");
                                p_state = syntaxerror;
                                return;
                              }
                              temp1 = p_numbers.top();
                              p_numbers.pop();
                              temp2 = p_numbers.top();
                              p_numbers.pop();
                              p_numbers.push(temp1 + temp2);
                              debug("processOperator() %v1 + %v2 ",temp2,temp1);
                              break;
    case operators::minus   : if( p_numbers.size() < 2 ) {
                                debug("processOperator() minus: not enough numbers");
                                p_state = syntaxerror;
                                return;
                              }
                              temp1 = p_numbers.top();
                              p_numbers.pop();
                              temp2 = p_numbers.top();
                              p_numbers.pop();
                              p_numbers.push(temp2 - temp1); //take care of correct sequence!
                              debug("processOperator() %v1 - %v2 ",temp2,temp1);
                              break;
    case operators::times   : if( p_numbers.size() < 2 ) {
                                debug("processOperator() times: not enough numbers");
                                p_state = syntaxerror;
                                return;
                              }
                              temp1 = p_numbers.top();
                              p_numbers.pop();
                              temp2 = p_numbers.top();
                              p_numbers.pop();
                              p_numbers.push(temp1 * temp2);
                              debug("processOperator() %v1 * %v2 ",temp2,temp1);
                              break;
    case operators::divide  : if( p_numbers.size() < 2 ) {
                                debug("processOperator() divide: not enough numbers");
                                p_state = syntaxerror;
                                return;
                              }
                              temp1 = p_numbers.top();
                              p_numbers.pop();
                              temp2 = p_numbers.top();
                              p_numbers.pop();
                              if( temp1 == 0 ) {
                                p_state = matherror;
                                p_errorstring = "Division by zero";
                                return;
                              }
                              p_numbers.push(temp2 / temp1); //take care of correct sequence!
                              debug("processOperator() %v1 / %v2 ",temp2,temp1);
                              break;
    case operators::sin     : if( p_numbers.size() < 1 ) {
                                debug("processOperator() sin: not enough numbers");
                                p_state = syntaxerror;
                                return;
                              }
                              temp1 = p_numbers.top();
                              p_numbers.pop();
                              p_numbers.push(sin(temp1));
                              debug("processOperator() sin(%v1)",temp1);
                              break;
    case operators::cos     : if( p_numbers.size() < 1 ) {
                                debug("processOperator() cos: not enough numbers");
                                p_state = syntaxerror;
                                return;
                              }
                              temp1 = p_numbers.top();
                              p_numbers.pop();
                              p_numbers.push(cos(temp1));
                              debug("processOperator() cos(%v1)",temp1);
                              break;
    case operators::tan     : if( p_numbers.size() < 1 ) {
                                debug("processOperator() tan: not enough numbers");
                                p_state = syntaxerror;
                                return;
                              }
                              temp1 = p_numbers.top();
                              p_numbers.pop();
                              p_numbers.push(tan(temp1));
                              debug("processOperator() tan(%v1)",temp1);
                              break;
    case operators::pi      : //If the local implementation defines M_PI, use it, but if its missing due to it's not being standard, push our hardcoded pi
                              #ifdef M_PI
                                p_numbers.push(M_PI);
                              #else
                                p_numbers.push(3.14159);
                              #endif
                              debug("processOperator() pi");
                              break;
    case operators::lbracket : debug("processOperator() lbracket"); //nothing to do here, lbracket only gets processed while processing the corresponding rbracket, so its save to be pop'ed
                               break;
    case operators::rbracket : p_operators.pop(); //pop that rbracket
                               while( !p_operators.empty() && p_operators.top() != operators::lbracket ) { //process everything until we reach a lbracket
                                 debug("processOperator() processing rbracket...");
                                 processOperator();
                               }
                               if( p_operators.empty() ) { //no lbracket
                                 p_state = syntaxerror;
                                 p_errorstring = "Missing open parenthese";
                                 return;
                               }
                               break;
    default                  : debug("processOperator() invalid operator (missing implementation)");
                               p_state = internalerror;
                               return;
  }
  if( p_state == running )
    p_operators.pop(); //when processing parentheses, this will pop the lbracket
}

//reset internal data structures
void parser::clear() {
  p_expression.clear();
  for(int i=p_numbers.size(); i>0; i--)
    p_numbers.pop();
  for(int i=p_operators.size(); i>0; i--)
    p_operators.pop();
  p_state = complete;
}

//Tries to extract a number off the beginning of p_expression, returns true on success and sets value to the extracted number. Otherwise returns false, value is not proved to remain unchanged in both cases
bool parser::extractNumber(double &value) {
  istringstream convert;
  convert.str(p_expression);
  if( convert >> value ) {
    string temp;
    convert >> temp; //on success, write the remaining unconverted part back to p_expression for further analysis
    p_expression = temp;
    return true;
  }
  else
    return false;
}

//Behaviour similar to extractNumber
bool parser::extractOperator(operators::ops &op) {
  int processed = p_expression.length();
  while( processed >= 0 && !string2operator(p_expression.substr(0,processed),op) ) //terminates when substring is operator or we reached processed = 0, then we have an syntax error
    processed--;
  if( processed >= 0 ) {
    p_expression = p_expression.substr(processed,p_expression.length()-processed); //strip processed bytes
    return true;
  }
  else
    return false;
}

//returns true if the operator exists and sets op to the corresponding enum, otherwise returns false
bool parser::string2operator(const string &str, operators::ops &op) {
  if( p_opmap.count(str) ) {
    op = p_opmap[str];
    return true;
  }
  else
    return false;
}

//result access function
double parser::result() {
  if( !p_numbers.empty() )
    return p_numbers.top();
  else
    return 0;
}

//get a nice error string in case parsing fails
string parser::getError() {
  switch( p_state ) {
    case running : return "parse() has not finished yet!";
    case complete : return "Parsing successful.";
    case syntaxerror : return "Syntax error: "+p_errorstring;
    case matherror : return "Math error: "+p_errorstring;
    case internalerror : return "Internal parser error. This should not happen. Debug info:\nexpression "+p_expression+(p_errorstring.empty() ? string() : "error "+p_errorstring);
  }
}

//only debug() prepares strings to save runtime during normal operation, specific operators in message will be replaced with the other parameters:
// %o1 -> o1  |  %o2 -> o2  |  %v1 -> v1  |  %v2 -> v2
void parser::debug(const string& message, const double v1, const double v2, const operators::ops op1, const operators::ops op2) {
  if( p_debug ) {
    string debugstr = message; //to save memory ressources, we get message as const reference, but in case we output debug, copy it to a modifyable string here
    int find;
    find = debugstr.find("%o1");
    if( find != string::npos )
      debugstr.replace(find,3,d2s((int)op1));
    find = debugstr.find("%o2");
    if( find != string::npos )
      debugstr.replace(find,3,d2s((int)op2));
    find = debugstr.find("%v1");
    if( find != string::npos )
      debugstr.replace(find,3,d2s(v1));
    find = debugstr.find("%v2");
    if( find != string::npos )
      debugstr.replace(find,3,d2s(v2));
    std::cout << debugstr << endl;
  }
}

void parser::debug(const string& message, const operators::ops op1, const operators::ops op2) { //convenance overload
  debug(message,0.0,0.0,op1,op2);
}

//converts double to string for debugging function
string parser::d2s(double v) {
  ostringstream convert;
  convert << v;
  return convert.str();
}

void parser::setDebug(bool active) {
  p_debug = active;
}

bool parser::getDebug() {
  return p_debug;
}
