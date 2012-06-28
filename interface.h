/***********************************************************/
/*                   interface class                       */
/* Talks to the user, handles special commands and         */
/* implements shell-like editing (using cursors, backspace */
/* etc.) for unix-like systems. Should also work on win32, */
/* but is currently untested and unsupported               */
/*                                                         */
/* AUTHOR: Fabian Lesniak <fabian.lesniak@student.kit.edu> */
/***********************************************************/

#ifndef INTERFACE_H
#define INTERFACE_H

#include <deque>
#include <string>
#include <map>
#include <list>

using namespace std;

static const char version[] = "0.7b";

class parser;

class interface {
public:
  interface();
  int talk();
private:
  void help();
  void test();
  void parse(string&);
  void processLine();
  void clearLine();
  void showPreviousExpression();
  void showNextExpression();
  void moveCursorRight();
  void moveCursorLeft();
  void moveCursorPos1();
  void moveCursorEnd();
  void insertCharacter(char c);
  void deleteCharacter();
  void deleteCharacterReverse();

  parser *p_parse;
  deque<string> p_commandHistory;
  deque<string>::iterator p_commandHistoryIterator;
  string::iterator p_commandIterator;
  bool p_poll;

  enum command { parseLine, displayHelp, runTest, exitProgram, toggleDebug, noCommand };
  map<string,command> p_commandMap;
  map<command,string> p_commandHelpMap;
  struct testExpression {
    string expression;
    double result;
    string help;
  };
  list<testExpression> p_testExpressions;
};

#endif //INTERFACE_H
