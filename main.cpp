#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <variant>
#include <memory>
using namespace std;

class Obj;
class element;

string readString(string s, int &i) {
    string res;
    if (s[i] != '"') {
        cerr << "Error in readString()" << endl;
        exit(1);
    }
    i++;
    while (s[i] != '"') {
        res += s[i];
        i++;
    }
    i++;
    return res;
}

int readInt(const string &s, int &i) {
    int res = 0;
    while (isdigit(s[i])) {
        res = res * 10 + s[i] - '0';
        i++;
    }
    return res;
}

class element {
public:
    variant<int, string, shared_ptr<Obj>, vector<element> > val;
};

class Obj {
public:
    map<string, vector<element> > data;
};

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
                    el.val = readInt(s, i);
                    v.push_back(el);
                } else if (s[i] == '"') {
                    el.val = readString(s, i);
                    v.push_back(el);
                } else if (s[i] == '{') {
                    el.val = make_shared<Obj>(parse(s, i));
                    v.push_back(el);
                } else if (s[i] == '[') {
                    i++;
                    vector<element> vec;
                    while (s[i] != ']') {
                        element inner_el;
                        if (isdigit(s[i])) {
                            inner_el.val = readInt(s, i);
                        } else if (s[i] == '"') {
                            inner_el.val = readString(s, i);
                        } else if (s[i] == '{') {
                            inner_el.val = make_shared<Obj>(parse(s, i));
                        } else {
                            i++;
                            continue;
                        }
                        vec.push_back(inner_el);
                    }
                    el.val = vec;
                    v.push_back(el);
                } else {
                    i++;
                }
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

string to_string(Obj);

string to_string(vector<element> vec) {
    string res = "";
    res += "[";
    for (const auto &el: vec) {
        if (holds_alternative<int>(el.val)) {
            res += to_string(get<int>(el.val)) + ",";
        } else if (holds_alternative<string>(el.val)) {
            res += "\"" + get<string>(el.val) + "\",";
        } else if (holds_alternative<shared_ptr<Obj> >(el.val)) {
            res += to_string(*get<shared_ptr<Obj> >(el.val)) + ",";
        } else if (holds_alternative<vector<element> >(el.val)) {
            res += to_string(get<vector<element> >(el.val)) + ",";
        }
    }
    if (!vec.empty()) {
        res.pop_back();
    }
    res += "],";
    return res;
}

string to_string(Obj obj) {
    string res = "";
    res += "{";
    for (auto i: obj.data) {
        res += "\"" + i.first + "\":[";
        for (int j = 0; j < i.second.size(); j++) {
            if (holds_alternative<int>(i.second[j].val)) {
                res += to_string(get<int>(i.second[j].val)) + ",";
            } else if (holds_alternative<string>(i.second[j].val)) {
                res += "\"" + get<string>(i.second[j].val) + "\",";
            } else if (holds_alternative<shared_ptr<Obj> >(i.second[j].val)) {
                res += to_string(*get<shared_ptr<Obj> >(i.second[j].val)) + ",";
            } else if (holds_alternative<vector<element> >(i.second[j].val)) {
                const auto &vec = get<vector<element> >(i.second[j].val);
                res += to_string(vec);
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

