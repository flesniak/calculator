/***********************************************************/
/*                    parser class                         */
/* Tries to evaluate string given to parse() member,       */
/* obtain result via result().                             */
/*                                                         */
/* AUTHOR: Fabian Lesniak <fabian.lesniak@student.kit.edu> */
/***********************************************************/

#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <stack>
#include <map>

using namespace std;

namespace operators { //Namespace to avoid conflicts
  //sorted by importance, more important operators will usually be processed preferably (except parentheses)
  enum ops { none, lbracket, rbracket, bracketCount, plus, minus, times, divide, pow, negation, operatorCount, sin, cos, tan, arcsin, arccos, arctan, sqrt, abs, functionCount, pi, e, ans, constantCount };
};

class parser {
public:
  parser();
  enum state { running, complete, syntaxerror, matherror, internalerror };
  state parse(const string& expression);
  void clear();
  string getError();
  double result();
  void setDebug(bool active);
  bool getDebug();

private:
  bool extractNumber(double &value);
  bool extractOperator(operators::ops &op);
  bool string2operator(const string &str, operators::ops &op);
  void processOperator();

  void debug(const string& message, const double v1 = 0, const double v2 = 0, const operators::ops op1 = operators::none, const operators::ops op2 = operators::none);
  void debug(const string& message, const operators::ops op1, const operators::ops op2 = operators::none);
  string d2s(const double v);
  string o2s(const operators::ops op);

  state p_state;
  string p_expression;
  stack<double> p_numbers;
  stack<operators::ops> p_operators;
  map<string,operators::ops> p_opmap;
  double p_ans;
  string p_errorstring;
  bool p_debug;
};

#endif //PARSER_H
