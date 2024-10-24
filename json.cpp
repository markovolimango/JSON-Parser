#include "json.h"

namespace json {
    std::variant<int, float> json::readNum(const std::string &s, int &i) {
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

    std::string json::readString(const std::string &s, int &i) {
        std::string res;
        i++;
        while (s[i] != '"') {
            res += s[i];
            i++;
        }
        i++;
        return res;
    }

    Obj json::parse(const std::string &s, int &i) {
        Obj res;
        while (s[i] != '}' && i < s.length()) {
            if (s[i] == '"') {
                std::string name = readString(s, i);
                while (s[i] != ':') {
                    if (s[i] != ' ' || s[i] != '\n') {
                        std::cerr << "Missing ':' in JSON file." << std::endl;
                        exit(1);
                    }
                    i++;
                }
                i++;
                while (s[i] == ' ' || s[i] == '\n') {
                    i++;
                }
                std::vector<element> v = readVector(s, i);
                res.data[name] = v;
                v.clear();
            } else {
                i++;
            }
        }
        i++;
        return res;
    }

    element readElement(const std::string &s, int &i) {
        element res;
        if (isdigit(s[i])) {
            if (std::variant<int, float> num = readNum(s, i); holds_alternative<float>(num)) {
                res.val = get<float>(num);
            } else {
                res.val = get<int>(num);
            }
        } else if (s[i] == '"') {
            res.val = readString(s, i);
        } else if (s[i] == '{') {
            res.val = std::make_shared<Obj>(parse(s, i));
        } else if (s[i] == '[') {
            res.val = readVector(s, i);
        } else {
            std::cerr << "Invalid character in JSON file. " << i << std::endl;
            exit(1);
        }
        return res;
    }

    std::vector<element> readVector(const std::string &s, int &i) {
        std::vector<element> res;
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

    std::string to_string(const element &el) {
        std::string res;
        if (holds_alternative<int>(el.val)) {
            res += std::to_string(get<int>(el.val));
        } else if (holds_alternative<float>(el.val)) {
            res += std::to_string(get<float>(el.val));
        } else if (holds_alternative<std::string>(el.val)) {
            res += "\"" + get<std::string>(el.val) + "\"";
        } else if (holds_alternative<std::shared_ptr<Obj> >(el.val)) {
            res += to_string(*get<std::shared_ptr<Obj> >(el.val));
        } else if (holds_alternative<std::vector<element> >(el.val)) {
            res += to_string(get<std::vector<element> >(el.val));
        }
        return res;
    }

    std::string to_string(const std::vector<element> &v) {
        std::string res;
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

    std::string to_string(const Obj &obj) {
        std::string res;
        res += "{";
        for (const auto &[fst, snd]: obj.data) {
            res += "\"" + fst + "\": " + to_string(snd) + ", ";
        }
        if (!obj.data.empty()) {
            res.pop_back();
            res.pop_back();
        }
        res += "}";
        return res;
    }
}
