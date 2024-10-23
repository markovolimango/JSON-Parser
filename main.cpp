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
    variant<int, float, string, shared_ptr<Obj>, vector<element> > val;
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

variant<int, float> readNum(const string &s, int &i) {
    float res = 0;
    while (isdigit(s[i])) {
        res = res * 10 + static_cast<float>(s[i]) - '0';
        i++;
    }
    if (s[i] == '.') {
        i++;
        float p = 10;
        while (isdigit(s[i])) {
            res += static_cast<float>(s[i] - '0') / p;
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
            if (variant<int, float> num = readNum(s, i); holds_alternative<float>(num)) {
                el.val = get<float>(num);
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
    string name;
    vector<element> v;
    while (s[i] != '}' && i < s.length()) {
        if (s[i] == '"') {
            name = readString(s, i);
            i += 2;
            while (s[i] != ']') {
                element el;
                if (isdigit(s[i])) {
                    if (variant<int, float> num = readNum(s, i); holds_alternative<float>(num)) {
                        el.val = get<float>(num);
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
                v.push_back(el);
            }
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
        } else if (holds_alternative<float>(val)) {
            res += to_string(get<float>(val)) + ",";
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
        res += "\"" + i.first + "\":[";
        for (auto &j: i.second) {
            if (holds_alternative<int>(j.val)) {
                res += to_string(get<int>(j.val)) + ",";
            } else if (holds_alternative<float>(j.val)) {
                res += to_string(get<float>(j.val)) + ",";
            } else if (holds_alternative<string>(j.val)) {
                res += "\"" + get<string>(j.val) + "\",";
            } else if (holds_alternative<shared_ptr<Obj> >(j.val)) {
                res += to_string(*get<shared_ptr<Obj> >(j.val)) + ",";
            } else if (holds_alternative<vector<element> >(j.val)) {
                const auto &vec = get<vector<element> >(j.val);
                res += to_string(vec) + ",";
            }
        }
        if (!i.second.empty()) {
            res.pop_back();
        }
        res += "],";
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

