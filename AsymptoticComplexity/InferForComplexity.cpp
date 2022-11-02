#include <fstream>
#include <iostream>
using namespace std;

string ForInfoTool = "../tool/build/bin/ForInfoTool";

string calculateSingleCost(string init, string cond, string inc) {
  if (isdigit(init[0]) && isdigit(cond[0]) && isdigit(inc[0])) {
    return "1";
  } else if (isdigit(init[0]) && !isdigit(cond[0]) && isdigit(inc[0])) {
    return "n";
  } else
    return "x";
}

string CalculateCumulativeCost(string cumulative) {
  int cN = 0, cConst = 0;
  for (int i = 0; i < cumulative.length(); i++) {
    if (cumulative[i] == 'n')
      cN++;
    else if (cumulative[i] == '1')
      cConst++;
    else if (cumulative[i] == 'x') {
      cN = -1;
      cConst = -1;
      break;
    }
  }
  if (cN == 0) {
    return "O(1)";
  } else if (cN > 0) {
   return  "O(n^" + to_string(cN) + ")";
  } else return "Undefined";

}

int main(int argc, char* argv[]) {
  if (argc != 2)
    return -1;
  string command = ForInfoTool + " " + argv[1] + " > temp.txt";
  system(command.c_str());

  fstream infos;
  infos.open("temp.txt", fstream::in);

  string induc, infosBuffer;
  string init, cond, inc;
  string costBuffer = "";

  int brackets = 0;
  while (infos >> infosBuffer) {
    if (infosBuffer == "}") {
      brackets--;
      if (brackets == 0) {
        cout << CalculateCumulativeCost(costBuffer) << endl;
        costBuffer = "";
      }
    } else if (infosBuffer[0] == '[') {
      while (infosBuffer[infosBuffer.length() - 1] != ']') {
        infos >> infosBuffer;
      }
    } else {
      induc = infosBuffer.substr(0, infosBuffer.length() - 1);

      infos >> infosBuffer;
      init = infosBuffer.substr(1, infosBuffer.length() - 2);

      infos >> infosBuffer;
      cond = infosBuffer.substr(0, infosBuffer.length() - 1);

      infos >> infosBuffer;
      inc = (infosBuffer.substr(0, infosBuffer.length() - 1));

      infos >> infosBuffer;
      brackets++;

      costBuffer += calculateSingleCost(init, cond, inc);
    }
  }
  infos.close();
  return 0;
}