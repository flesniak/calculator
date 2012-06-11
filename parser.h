#include <string>
#include <stack>
#include <map>

using namespace std;

namespace operators { //Namespace to avoid conflicts
  enum ops { none, lbracket, rbracket, bracketCount, plus, minus, times, divide, pow, negation, operatorCount, sin, cos, tan, arcsin, arccos, arctan, functionCount, pi, e, constantCount }; //sorted by importance, rbracket has to be highest
  //            0         1         2     3      4      5       6              7    8    9      10      11      12  13        14          15
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
  string p_errorstring;
  bool p_debug;
};
