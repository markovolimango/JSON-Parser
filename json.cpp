#include "json.h"

namespace json {
    std::variant<int, float> json::readNumber(const std::string &text, int &pos) {
        float res = 0;
        while (isdigit(text[pos])) {
            res = res * 10 + static_cast<float>(text[pos]) - '0';
            pos++;
        }
        if (text[pos] == '.') {
            pos++;
            float decimalPower = 10;
            while (isdigit(text[pos])) {
                res += static_cast<float>(text[pos] - '0') / decimalPower;
                decimalPower *= 10;
                pos++;
            }
            return res;
        }
        return static_cast<int>(res);
    }

    std::string json::readString(const std::string &text, int &pos) {
        std::string res;
        pos++;
        while (text[pos] != '"') {
            res += text[pos];
            pos++;
        }
        pos++;
        return res;
    }

    Element readElement(const std::string &text, int &pos) {
        Element res;
        if (isdigit(text[pos])) {
            if (std::variant<int, float> number = readNumber(text, pos); holds_alternative<float>(number)) {
                res.value = get<float>(number);
            } else {
                res.value = get<int>(number);
            }
        } else if (text[pos] == '"') {
            res.value = readString(text, pos);
        } else if (text[pos] == '{') {
            res.value = std::make_shared<Object>(readObj(text, pos));
        } else if (text[pos] == '[') {
            res.value = readVector(text, pos);
        } else {
            std::cerr << "Invalid character in JSON file. " << pos << std::endl;
            exit(1);
        }
        while (text[pos] == ' ' || text[pos] == '\n') {
            pos++;
        }
        return res;
    }

    std::vector<Element> readVector(const std::string &text, int &pos) {
        std::vector<Element> res;
        if (text[pos] != '[') {
            res = {readElement(text, pos)};
            return res;
        }
        pos++;
        while (text[pos] == ' ' || text[pos] == '\n') {
            pos++;
        }
        while (text[pos] != ']') {
            if (text[pos] == ',') {
                pos++;
                while (text[pos] == ' ' || text[pos] == '\n') {
                    pos++;
                }
            }
            Element element = readElement(text, pos);
            res.push_back(element);
        }
        pos++;
        return res;
    }

    Object json::readObj(const std::string &text, int &pos) {
        Object res;
        while (text[pos] != '}' && pos < text.length()) {
            if (text[pos] == '"') {
                std::string name = readString(text, pos);
                while (text[pos] != ':') {
                    if (text[pos] != ' ' || text[pos] != '\n') {
                        std::cerr << "Missing ':' in JSON file." << std::endl;
                        exit(1);
                    }
                    pos++;
                }
                pos++;
                while (text[pos] == ' ' || text[pos] == '\n') {
                    pos++;
                }
                std::vector<Element> v = readVector(text, pos);
                res.data[name] = v;
                v.clear();
            } else {
                pos++;
            }
        }
        pos++;
        return res;
    }

    std::string to_string(const Element &element) {
        std::string res;
        if (holds_alternative<int>(element.value)) {
            res += std::to_string(get<int>(element.value));
        } else if (holds_alternative<float>(element.value)) {
            res += std::to_string(get<float>(element.value));
        } else if (holds_alternative<std::string>(element.value)) {
            res += "\"" + get<std::string>(element.value) + "\"";
        } else if (holds_alternative<std::shared_ptr<Object> >(element.value)) {
            res += to_string(*get<std::shared_ptr<Object> >(element.value));
        } else if (holds_alternative<std::vector<Element> >(element.value)) {
            res += to_string(get<std::vector<Element> >(element.value));
        }
        return res;
    }

    std::string to_string(const std::vector<Element> &vec) {
        std::string res;
        if (vec.size() == 1) {
            res += to_string(vec[0]);
            return res;
        }
        res += "[";
        for (const auto &element: vec) {
            res += to_string(element) + ", ";
        }
        if (!vec.empty()) {
            res.pop_back();
            res.pop_back();
        }
        res += "]";
        return res;
    }

    std::string to_string(const Object &object) {
        std::string res;
        res += "{";
        for (const auto &[name, element]: object.data) {
            res += "\"" + name + "\": " + to_string(element) + ", ";
        }
        if (!object.data.empty()) {
            res.pop_back();
            res.pop_back();
        }
        res += "}";
        return res;
    }
}
