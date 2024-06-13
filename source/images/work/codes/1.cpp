#include <iostream>
#include <string>

using namespace std;

int function(string& ss) {
  int i = 0;
  string temp;
  while(i < ss.size()) {
    if(ss[i] <= '9' && ss[i] >= '0') {
      if(temp.size() < 4) {
        temp.push_back(ss[i]);
        if(stoi(temp) > 255) return 9;
      }
      else return 7;
      i ++;
    }
    else if(ss[i] == '.' && !temp.empty()) {
      temp.clear();
      i ++;
    }
    else return 8;
  }
  return true;
}

int main() {
  string ss;
  cin >> ss;
  cout << function(ss) << endl;
  return 0;
}

