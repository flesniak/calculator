#include <deque>
#include <string>
#include <map>

using namespace std;

static const char version[] = "0.4";

class parser;

class interface {
public:
  interface();
  int talk();
private:
  void help();
  void test();
  void parse(const string&);
  void processLine();
  void clearLine();
  void showPreviousExpression();
  void showNextExpression();
  void moveCursorRight();
  void moveCursorLeft();
  void insertCharacter(char c);
  void deleteCharacter();

  parser *p_parse;
  deque<string> p_commandHistory;
  deque<string>::iterator p_commandHistoryIterator;
  string::iterator p_commandIterator;
  bool p_poll;

  enum command { parseLine, displayHelp, runTest, exitProgram, noCommand };
  map<string,command> p_commandMap;
};
