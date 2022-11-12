#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <string>

using namespace std;

string ForInfoTool = "../tool/build/bin/ForInfoTool";

string calculateSingleCost(string init, string cond, string inc) {
  if (isdigit(init[0]) && isdigit(cond[0]) && isdigit(inc[0]))
    return "1";
  else if (isdigit(init[0]) && !isdigit(cond[0]) && ( isdigit(inc[0]) || isdigit(inc[1])))
    return cond;
  else if(!isdigit(init[0]) && isdigit(cond[0]) && ( isdigit(inc[0]) || isdigit(inc[1])))
   return init;
  else
    return "x";
}

void formatCostString(string &cost) {
  for(auto it = cost.begin(); it != cost.end(); it++) {
    if(*it == '*')
      cost.erase(it);
    if(*it == '(' && *(it+1) == ')') {
      cost.erase(it);
      cost.erase(it);
    }
    if(*it == '+' && *(it+1) == ')') {
      cost.erase(it);
    }
  }
  cost.erase(cost.end()-1);
}

int main(int argc, char* argv[]) {
  if (argc != 2)
    return -1;
  string command = ForInfoTool + " " + argv[1] + " > temp.csv";
  system(command.c_str());

  fstream infos("temp.csv");
  vector<string> row;
  string line, content, temp;

  string costBuffer = "";
  string induct, init, cond, inc;
  int brackets = 0;

  while (!infos.eof()) {
    row.clear();
    getline(infos, line);
    stringstream s(line);

    while(getline(s, content, ',')) {
      row.push_back(content);
    }

    for(auto it = row.begin(); it != row.end(); it++) {
      if(*it == "}") {
        brackets--;
        costBuffer += ")";
        costBuffer += "+";
        if(brackets == 0) {
          formatCostString(costBuffer);
          cout << "O("<<costBuffer << ")\n";
          costBuffer.clear();
        }
      }
      else if(*it == "[") {
        break;
      }
      else if(*it == "{") {
        brackets++;
        costBuffer += "(";
      } else {
      induct = *it;
      init = *(++it);
      cond = *(++it);
      inc = *(++it);
    
      costBuffer.append(calculateSingleCost(init, cond, inc));
      costBuffer += "*";
      }
    }
  }
  return 0;
}