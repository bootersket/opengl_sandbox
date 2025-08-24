#include <iostream>
#include <string>
using namespace std;

int main() {
  string s = "hello string";
  const char* s2 = s.c_str();
  cout << s << endl;
  cout << s2 << endl;

  return 0;
}
