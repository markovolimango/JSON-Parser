//
// Created by Marko Popadić on 24.10.24..
//

#ifndef JSON_H
#define JSON_H

#endif //JSON_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <variant>
using namespace std;

namespace json {
    class Obj;

    class element {
    public:
        std::variant<int, float, std::string, std::shared_ptr<Obj>, std::vector<element> > val;
    };

    class Obj {
    public:
        std::map<std::string, std::vector<element> > data;
    };

    Obj parse(const string &s, int &i);


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

    vector<element> readVector(const string &s, int &i);

    element readElement(const string &s, int &i) {
        element res;
        if (isdigit(s[i])) {
            if (variant<int, float> num = readNum(s, i); holds_alternative<float>(num)) {
                res.val = get<float>(num);
            } else {
                res.val = get<int>(num);
            }
        } else if (s[i] == '"') {
            res.val = readString(s, i);
        } else if (s[i] == '{') {
            res.val = make_shared<Obj>(parse(s, i));
        } else if (s[i] == '[') {
            res.val = readVector(s, i);
        } else {
            cerr << "Invalid character in JSON file. " << i << endl;
            exit(1);
        }
        return res;
    }

    vector<element> readVector(const string &s, int &i) {
        vector<element> res;
        if (s[i] != '[') {
            res = {readElement(s, i)};
            return res;
        }
        i++;
        while (s[i] == ' ' || s[i] == '\n') {
            i++;
        }
        while (s[i] != ']') {
            if (s[i] == ',') {
                i++;
                while (s[i] == ' ' || s[i] == '\n') {
                    i++;
                }
            }
            element el = readElement(s, i);
            res.push_back(el);
            while (s[i] == ' ' || s[i] == '\n') {
                i++;
            }
        }
        i++;
        return res;
    }

    Obj parse(const string &s, int &i) {
        Obj res;
        while (s[i] != '}' && i < s.length()) {
            if (s[i] == '"') {
                string name = readString(s, i);
                while (s[i] != ':') {
                    if (s[i] != ' ' || s[i] != '\n') {
                        cerr << "Missing ':' in JSON file." << endl;
                        exit(1);
                    }
                    i++;
                }
                i++;
                while (s[i] == ' ' || s[i] == '\n') {
                    i++;
                }
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

    string to_string(const vector<element> &v);

    string to_string(const element &el) {
        string res;
        if (holds_alternative<int>(el.val)) {
            res += std::to_string(get<int>(el.val));
        } else if (holds_alternative<float>(el.val)) {
            res += std::to_string(get<float>(el.val));
        } else if (holds_alternative<string>(el.val)) {
            res += "\"" + get<string>(el.val) + "\"";
        } else if (holds_alternative<shared_ptr<Obj> >(el.val)) {
            res += to_string(*get<shared_ptr<Obj> >(el.val));
        } else if (holds_alternative<vector<element> >(el.val)) {
            res += to_string(get<vector<element> >(el.val));
        }
        return res;
    }

    string to_string(const vector<element> &v) {
        string res;
        if (v.size() == 1) {
            res += to_string(v[0]);
            return res;
        }
        res += "[";
        for (const auto &el: v) {
            res += to_string(el) + ", ";
        }
        if (!v.empty()) {
            res.pop_back();
            res.pop_back();
        }
        res += "]";
        return res;
    }

    string to_string(const Obj &obj) {
        string res;
        res += "{";
        for (const auto &i: obj.data) {
            res += "\"" + i.first + "\": " + to_string(i.second) + ", ";
        }
        if (!obj.data.empty()) {
            res.pop_back();
            res.pop_back();
        }
        res += "}";
        return res;
    }
}
