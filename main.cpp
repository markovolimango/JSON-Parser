#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

bool isNum(char c) {
    return c >= '0' && c <= '9';
}

string readStr(string s, int &i) {
    string res = "";
    i++;
    while (s[i] != '"') {
        res += s[i];
        i++;
    }
    i++;

    return res;
}

int readInt(string s, int &i) {
    int num = 0;
    while (isNum(s[i])) {
        num = num * 10 + (s[i] - '0');
        i++;
    }
    return num;
}

class Obj {
public:
    string name;
    vector<variant<int, string, Obj> > data;
    vector<Obj> children;

    Obj(string name) {
        this->name = name;
        data = vector<variant<int, string, Obj> >();
        children = vector<Obj>();
    }

    Obj(string name, int num) {
        this->name = name;
        data = {num};
        children = vector<Obj>();
    }

    Obj(string name, string str) {
        this->name = name;
        data = {str};
        children = vector<Obj>();
    }
};

string to_string(Obj obj) {
    string s = "\"" + obj.name + "\":";
    if (!obj.data.empty()) {
        for (int i = 0; i < obj.data.size(); i++) {
            if (holds_alternative<int>(obj.data[i])) {
                s += to_string(get<int>(obj.data[i])) + ",";
            }
            if (holds_alternative<string>(obj.data[i])) {
                s += "\"" + get<string>(obj.data[i]) + "\",";
            }
        }
    }
    if (!obj.children.empty()) {
        s += "{";
        for (int i = 0; i < obj.children.size(); i++) {
            s += to_string(obj.children[i]);
        }
        s += "},";
    }
    return s;
}

void form(string s, int &i, Obj &parent) {
    string name;
    int num;
    string str;

    while (s[i] != '}' && i < s.size()) {
        if (s[i] == '"') {
            name = readStr(s, i);
        } else if (s[i] == ':') {
            i++;
            if (isNum(s[i])) {
                num = readInt(s, i);
                parent.children.push_back(Obj(name, num));
            } else if (s[i] == '"') {
                str = readStr(s, i);
                parent.children.push_back(Obj(name, str));
            } else if (s[i] == '{') {
                Obj child(name);
                i++;
                form(s, i, child);
                parent.children.push_back(child);
                i++;
            }
        } else {
            i++;
        }
    }
}

int main() {
    ifstream myFile("text.json");
    string s;
    getline(myFile, s);

    Obj root("root");
    int i = 1;
    form(s, i, root);
    cout << to_string(root) << endl;
    //komentar

    myFile.close();
    return 0;
}
