#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <stack>
#include <map>

using namespace std;

namespace operators { //Namespace to avoid conflicts
  enum ops { none, plus, minus, times, divide, negation, operatorCount, sin, cos, tan, arcsin, arccos, arctan, functionCount, pi, constantCount };
};

class parser {
public:
  parser();
  enum state { running, complete, syntaxerror, matherror, internalerror };
  state parse(const string& expression);
  void clear();

private:
  bool extractNumber(double &value);
  bool extractOperator(operators::ops &op);
  bool string2operator(const string &str, operators::ops &op);
  void processOperator();

  state p_state;
  string p_expression;
  stack<double> p_numbers;
  stack<operators::ops> p_operators;
  map<string,operators::ops> p_opmap;
};

#endif //PARSER_H
