#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <variant>
#include "json.h"
using namespace std;
using namespace json;

int main() {
    ifstream json("text.json");
    stringstream buffer;
    buffer << json.rdbuf();
    string s = buffer.str();
    string out;
    Obj obj;
    int i = 0;

    obj = parse(s, i);

    out = to_string(obj);
    cout << out << endl;

    cout << get<int>((*get<shared_ptr<Obj> >(obj.data["a"][0].val)).data["b"][1].val);
    string input = "a.b[1]";
    i = 0;
}
