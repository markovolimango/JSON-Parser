#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <variant>
using namespace std;

class Obj;
class element;

Obj parse(string s, int &i);

class element {
public:
    variant<int, double, string, shared_ptr<Obj>, vector<element> > val;
};

class Obj {
public:
    map<string, vector<element> > data;
};

string readString(const string &s, int &i) {
    string res;
    i++;
    while (s[i] != '"') {
        res += s[i];
        i++;
    }
    i++;
    return res;
}

variant<int, double> readNum(const string &s, int &i) {
    double res = 0;
    while (isdigit(s[i])) {
        res = res * 10 + static_cast<double>(s[i]) - '0';
        i++;
    }
    if (s[i] == '.') {
        i++;
        double p = 10;
        while (isdigit(s[i])) {
            res += static_cast<double>(s[i] - '0') / p;
            p *= 10;
            i++;
        }
        return res;
    }
    return static_cast<int>(res);
}

vector<element> readVector(const string &s, int &i) {
    i++;
    vector<element> res;
    while (s[i] != ']') {
        element el;
        if (isdigit(s[i])) {
            if (variant<int, double> num = readNum(s, i); holds_alternative<double>(num)) {
                el.val = get<double>(num);
            } else {
                el.val = get<int>(num);
            }
        } else if (s[i] == '"') {
            el.val = readString(s, i);
        } else if (s[i] == '{') {
            el.val = make_shared<Obj>(parse(s, i));
        } else if (s[i] == '[') {
            el.val = readVector(s, i);
        } else {
            i++;
            continue;
        }
        res.push_back(el);
    }
    i++;
    return res;
}

Obj parse(string s, int &i) {
    Obj res;
    while (s[i] != '}' && i < s.length()) {
        if (s[i] == '"') {
            string name = readString(s, i);
            i++;
            vector<element> v = readVector(s, i);
            res.data[name] = v;
            v.clear();
        } else {
            i++;
        }
    }
    i++;
    return res;
}

string to_string(const Obj &);

string to_string(const vector<element> &v) {
    string res;
    res += "[";
    for (const auto &[val]: v) {
        if (holds_alternative<int>(val)) {
            res += to_string(get<int>(val)) + ",";
        } else if (holds_alternative<double>(val)) {
            res += to_string(get<double>(val)) + ",";
        } else if (holds_alternative<string>(val)) {
            res += "\"" + get<string>(val) + "\",";
        } else if (holds_alternative<shared_ptr<Obj> >(val)) {
            res += to_string(*get<shared_ptr<Obj> >(val)) + ",";
        } else if (holds_alternative<vector<element> >(val)) {
            res += to_string(get<vector<element> >(val)) + ",";
        }
    }
    if (!v.empty()) {
        res.pop_back();
    }
    res += "]";
    return res;
}

string to_string(const Obj &obj) {
    string res;
    res += "{";
    for (auto i: obj.data) {
        res += "\"" + i.first + "\":";
        res += to_string(i.second);
    }
    if (!obj.data.empty()) {
        res.pop_back();
    }
    res += "}";
    return res;
}

string format(string s) {
    string res = "";
    return res;
}

int main() {
    ifstream json("text.json");
    string s, out;
    getline(json, s);
    Obj obj;
    int i = 0;

    obj = parse(s, i);

    out = to_string(obj);
    cout << out << endl;
}
