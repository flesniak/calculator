#include <iostream>
#include <string>
#include <sstream>

using namespace std;

int main() {
  string str, str2;
  double test;
  cin >> str;
  istringstream conv(str);
  if( conv >> test ) {
  cout << "test " << test << endl;
  conv >> str;
  cout << "str " << str << endl; }
//   if( conv >> test >> str2 )
//     cout << test << " str " << conv.str() << " str2 " << str2 << endl;
}
