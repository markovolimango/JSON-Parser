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

variant<int, string> read(string s, int &i) {
    if (s[i] == '"') {
        string res = "";
        i++;
        while (s[i] != '"') {
            res += s[i];
            i++;
        }
        i++;
        return res;
    } else if (s[i] >= '0' && s[i] <= '9') {
        int res = 0;
        while (s[i] != ',' && s[i] != '}') {
            res = res * 10 + (s[i] - '0');
            i++;
        }
        return res;
    }
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

    Obj(string name, variant<int, string> el) {
        this->name = name;
        if (holds_alternative<int>(el)) {
            data = {get<int>(el)};
        } else if (holds_alternative<string>(el)) {
            data = {get<string>(el)};
        }
        children = vector<Obj>();
    }
};

void form(string s, int &i, Obj &parent) {
    string name;
    variant<int, string> res;

    while (s[i] != '}' && i < s.size()) {
        if (s[i] == '"') {
            name = readStr(s, i);
        } else if (s[i] == ':') {
            i++;
            if (s[i] == '{') {
                Obj child(name);
                i++;
                form(s, i, child);
                parent.children.push_back(child);
                i++;
            } else {
                res = read(s, i);
                parent.children.push_back(Obj(name, res));
            }
        } else {
            i++;
        }
    }
}

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

int main() {
    ifstream myFile("text.json");
    string s;
    getline(myFile, s);

    Obj root("root");
    int i = 1;
    form(s, i, root);
    cout << to_string(root) << endl;

    myFile.close();
    return 0;
}
