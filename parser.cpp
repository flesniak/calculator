#include "parser.h"

#include <sstream>
#include <iostream>

parser::parser() {
  clear();
  p_debug = false;

  //Initialize operator map
  opmap["+"] = operators::plus;
  opmap["-"] = operators::minus;
  opmap["*"] = operators::times;
  opmap["/"] = operators::divide;
  opmap["sin"] = operators::sin;
  opmap["cos"] = operators::cos;
  opmap["tan"] = operators::tan;
  opmap["arcsin"] = operators::arcsin;
  opmap["arccos"] = operators::arccos;
  opmap["arctan"] = operators::arctan;
  opmap["SIN"] = operators::sin;
  opmap["COS"] = operators::cos;
  opmap["TAN"] = operators::tan;
  opmap["ARCSIN"] = operators::arcsin;
  opmap["ARCCOS"] = operators::arccos;
  opmap["ARCTAN"] = operators::arctan;
  opmap["("] = operators::lbracket;
  opmap[")"] = operators::rbracket;
}

parser::state parser::parse(const string& expression) {
  clear();
  p_state = running; //pseudo "running" state
  p_expression = expression; //we won't modify expression
  double temp;
  operators::ops op;
  bool needoperator = false;

  //Shunting-yard algorithm
  while( p_state == running && !p_expression.empty() ) {
    debug("parse loop: expression "+p_expression);
    //Process input
    if( !needoperator && extractNumber(temp) ) {
      p_numbers.push(temp);
      needoperator = true;
      debug("parse loop: number "+d2s(p_numbers.top())+" found");
    }
    else {
      if( extractOperator(op) ) {
        while( p_state == running && !p_operators.empty() && p_operators.top()+p_prioFactor >= op ) { //process operators with higher priority
          debug("parse loop: preferring operator "+d2s((int)p_operators.top())+" over "+d2s((int)op));
          processOperator();
        }
        if( p_state != running )
          return p_state;
        p_operators.push(op);
        needoperator = false;
        debug("parse loop: operator "+d2s((int)p_operators.top())+" found");
      }
      else {
        p_state = error;
        return p_state;
      }
    }
  }

  //Expression is parsed, we now just have to process all remaining operators
  while( p_state == running && !p_operators.empty() )
    processOperator();

  //debug
  /*for(int a = p_numbers.size(); a > 0; a--) {
    debug("p_numbers["+d2s(a)+"] = "+d2s(p_numbers.top()));
    p_numbers.pop();
  }
  for(int a = p_operators.size(); a > 0; a--) {
    debug("p_operators["+d2s(a)+"] = "+d2s((int)p_operators.top()));
    p_operators.pop();
  }*/

  if( p_state == running )
    p_state = complete;
  return p_state;
}

void parser::processOperator() {
  if( p_operators.empty() ) {
    debug("processOperator: p_operators empty!");
    p_state = bug;
    return;
  }
  double temp1, temp2;
  switch( p_operators.top() ) {
    case operators::plus    : if( p_numbers.size() < 2 ) {
                                debug("processOperator: plus: not enough numbers");
                                p_state = bug;
                                return;
                              }
                              temp1 = p_numbers.top();
                              p_numbers.pop();
                              temp2 = p_numbers.top();
                              p_numbers.pop();
                              p_numbers.push(temp1 + temp2);
                              debug("processOperator: "+d2s(temp1)+" + "+d2s(temp2));
                              break;
    case operators::minus   : if( p_numbers.size() < 2 ) {
                                debug("processOperator: minus: not enough numbers");
                                p_state = bug;
                                return;
                              }
                              temp1 = p_numbers.top();
                              p_numbers.pop();
                              temp2 = p_numbers.top();
                              p_numbers.pop();
                              p_numbers.push(temp2 - temp1); //take care of correct sequence!
                              debug("processOperator: "+d2s(temp2)+" - "+d2s(temp1));
                              break;
    case operators::times   : if( p_numbers.size() < 2 ) {
                                debug("processOperator: times: not enough numbers");
                                p_state = bug;
                                return;
                              }
                              temp1 = p_numbers.top();
                              p_numbers.pop();
                              temp2 = p_numbers.top();
                              p_numbers.pop();
                              p_numbers.push(temp1 * temp2);
                              debug("processOperator: "+d2s(temp1)+" * "+d2s(temp2));
                              break;
    case operators::divide  : if( p_numbers.size() < 2 ) {
                                debug("processOperator: divide: not enough numbers");
                                p_state = bug;
                                return;
                              }
                              temp1 = p_numbers.top();
                              p_numbers.pop();
                              temp2 = p_numbers.top();
                              p_numbers.pop();
                              if( temp1 == 0 ) {
                                p_state = error;
                                return;
                              }
                              p_numbers.push(temp2 / temp1); //take care of correct sequence!
                              debug("processOperator: "+d2s(temp2)+" / "+d2s(temp1));
                              break;
    case operators::lbracket : p_prioFactor += operators::operatorCount;
                               debug("processOperator: (");
                               break; //nothing to do with brackets, they just signalize priority which is handled by the algorith directly; we just need to pop the bracket
    case operators::rbracket : if( p_prioFactor < operators::operatorCount )
                                 p_state = error; // ) without (
                               else
                                 p_prioFactor -= operators::operatorCount;
                               debug("processOperator: )");
                               break; //dito
    default                 : p_state = bug;
  }
  if( p_state == running )
    p_operators.pop();
}

void parser::clear() {
  p_expression.clear();
  for(int i=p_numbers.size(); i>0; i--)
    p_numbers.pop();
  for(int i=p_operators.size(); i>0; i--)
    p_operators.pop();
  p_state = complete;
  p_prioFactor = 1;
}

const string& parser::getError() {
  return p_expression;
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

//Returns true if the operator exists and sets op to the corresponding enum, otherwise returns false
bool parser::string2operator(const string &str, operators::ops &op) {
  if( opmap.count(str) ) {
    op = opmap[str];
    return true;
  }
  else
    return false;
}

double parser::result() {
  if( !p_numbers.empty() )
    return p_numbers.top();
  else
    return 0;
}

void parser::debug(const string& message) {
  if( p_debug )
    std::cout << message << endl;
}

string parser::d2s(double v) {
  ostringstream convert;
  convert << v;
  return convert.str(); //do not return a reference to temporary
}

void parser::setDebug(bool active) {
  p_debug = active;
}

bool parser::getDebug() {
  return p_debug;
}
