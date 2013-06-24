/***********************************************************/
/*              parser class implementation                */
/*                                                         */
/* AUTHOR: Fabian Lesniak <fabian.lesniak@student.kit.edu> */
/***********************************************************/

#include "parser.h"

#include <sstream>
#include <iostream>
#include <cmath>
#include <limits>

parser::parser() {
  clear();

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
  p_opmap["PI"] = operators::pi;
//  p_opmap["negation"] = operators::negation; //map negation, may be used in formula but mainly useful for debugging output (reverse lookup)
}

//parse expression
parser::state parser::parse(const string& expression) {
  clear(); //make sure no data from previous parsing is left
  p_state = running;
  p_expression = expression; //we won't modify expression
  double temp;
  operators::ops op;
  bool needoperator = false; //the needoperator value helps deciding between +/- signs or operators ( and to process "missing" *'s)

  //Shunting-yard algorithm
  while( p_state == running && !p_expression.empty() ) { //process p_expression until it is empty or we encouter a p_state change
    //Process input
    if( !needoperator && extractNumber(temp) ) { //we won't try to read two numbers in a row (!needoperator), if extractNumber fails, try to exractOperator
      if( !p_operators.empty() && p_operators.top() > operators::operatorCount ) {
        p_state = syntaxerror;
        return p_state;
      }
      p_numbers.push(temp);
      needoperator = true;
    }
    else { //BEGIN OPERATOR HANDLING (this will be nasty)
      if( extractOperator(op) ) {
        //Handle negation
        if( !needoperator && op == operators::minus )
          op = operators::negation;

        while( p_state == running && !p_operators.empty() && p_operators.top() >= op )
          processOperator();
        if( p_state != running )
          return p_state;

        p_operators.push(op);

        //Constants are just being replaced, so we still need an operator!
        if( op > operators::functionCount )
          needoperator = true;
        else
          needoperator = false;
      }
      else { //extractOperator was unable to process p_expression
        p_state = syntaxerror;
        return p_state;
      }
    } //END OPERATOR HANDLING
  }

  while( p_state == running && !p_operators.empty() )
    processOperator();

  if( !p_operators.empty() && !p_numbers.empty() && p_numbers.size() > 1 ) {
    p_state = syntaxerror;
    return p_state;
  }

  if( p_state == running )
    p_state = complete;
  p_expression = expression; //p_expression got mutilated during execution, save it for our getError() method
  std::cout << p_expression << " = " << p_numbers.top();
  return p_state;
}

//take operator (or function) from stack, try to parse it and push the result onto p_numbers
void parser::processOperator() {
  if( p_operators.empty() ) {
    p_state = internalerror;
    return;
  }
  double temp1, temp2;
  temp1 = p_numbers.top();
  p_numbers.pop();
  if( p_numbers.size() < ((p_operators.top() < operators::negation) ? 2 : 1) ) {
    p_state = syntaxerror;
    return;
  }
  if( p_operators.top() < operators::negation ) {
    temp2 = p_numbers.top();
    p_numbers.pop();
  }
  switch( p_operators.top() ) {
    case operators::plus    : p_numbers.push(temp1 + temp2);
                              break;
    case operators::minus   : p_numbers.push(temp1 - temp2); //take care of correct sequence!
                              break;
    case operators::times   : p_numbers.push(temp1 * temp2);
                              break;
    case operators::divide  : p_numbers.push(temp1 / temp2); //take care of correct sequence!
                              break;
    case operators::negation : p_numbers.push(-1*temp1);
                              break;
    case operators::sin     : temp2 = sin(temp1);
                              if( fabs(temp2) < numeric_limits<double>::epsilon()*(2*temp1/M_PI) ) //workaround for sin(n*pi) != 0
                                temp2 = 0;
                              p_numbers.push(temp2);
                              break;
    case operators::cos     : temp2 = cos(temp1);
                              if( fabs(temp2) < numeric_limits<double>::epsilon()*(2*temp1/M_PI) )
                                temp2 = 0;
                              p_numbers.push(temp2);
                              break;
    case operators::tan     : temp2 = tan(temp1);
                              if( fabs(temp2) < numeric_limits<double>::epsilon()*(2*temp1/M_PI) )
                                temp2 = 0;
                              if( 1/fabs(temp2) < numeric_limits<double>::epsilon()*(2*temp1/M_PI) )
                                temp2 = numeric_limits<double>::infinity();
                              p_numbers.push(temp2);
                              break;
    case operators::arcsin  : p_numbers.push(asin(temp1));
                              break;
    case operators::arccos  : p_numbers.push(acos(temp1));
                              break;
    case operators::arctan  : p_numbers.push(atan(temp1));
                              break;
    case operators::pi      : p_numbers.push(M_PI);
                              break;
    default                  : p_state = internalerror;
                               return;
  }
  if( p_state == running )
    p_operators.pop(); //when processing parentheses, this will pop the lbracket
}

void parser::clear() {
  p_expression.clear();
  for(int i=p_numbers.size(); i>0; i--)
    p_numbers.pop();
  for(int i=p_operators.size(); i>0; i--)
    p_operators.pop();
  p_state = complete;
}

bool parser::extractNumber(double &value) {
  istringstream convert;
  convert.str(p_expression); //make sure to not scan small 'e', only 'E' will be used to do "*10^"
  if( convert >> value ) {
    p_expression.clear();
    convert >> p_expression; //on success, write the remaining unconverted part back to p_expression for further analysis
    return true;
  }
  else
    return false;
}

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

bool parser::string2operator(const string &str, operators::ops &op) {
  if( p_opmap.count(str) ) {
    op = p_opmap[str];
    return true;
  }
  else
    return false;
}

