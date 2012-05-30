#include <string>
#include <stack>
#include <map>

using namespace std;

namespace operators {
  enum ops { none, plus, minus, times, divide, sin, cos, tan, arcsin, arccos, arctan, rbracket, lbracket, operatorCount }; //sorted by importance, maximum
  //            0      1      2     3      4      5      6      7     8     9      10      11      12         13
};

class parser {
public:
  parser();
  enum state { running, complete, error, bug };
  state parse(const string& expression);
  void clear();
  const string& getError();
  double result();
  void setDebug(bool active);
  bool getDebug();

private:
  bool extractNumber(double &value);
  bool extractOperator(operators::ops &op);
  bool string2operator(const string &str, operators::ops &op);
  void processOperator();
  void debug(const string& message);
  string d2s(double v);

  state p_state;
  string p_expression;
  stack<double> p_numbers;
  stack<operators::ops> p_operators;
  map<string,operators::ops> opmap;
  int p_prioFactor;
  bool p_debug;
};
