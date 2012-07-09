#include "interface.h"

int interface::talk() {
string str;
while( str != "exit" ) {
  cin >> str;
  parser p;
  p.parse(str);
}

