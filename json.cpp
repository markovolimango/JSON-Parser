#include "json.h"

namespace json {
    bool Element::isInt() const {
        return std::holds_alternative<int>(value);
    }

    bool Element::isFloat() const {
        return std::holds_alternative<float>(value);
    }

    bool Element::isNumber() const {
        return std::holds_alternative<int>(value) || std::holds_alternative<float>(value);
    }

    bool Element::isString() const {
        return std::holds_alternative<std::string>(value);
    }

    bool Element::isObjectPtr() const {
        return std::holds_alternative<std::shared_ptr<Object> >(value);
    }

    bool Element::isVector() const {
        return std::holds_alternative<std::vector<Element> >(value);
    }

    int Element::getInt() const {
        return std::get<int>(value);
    }

    float Element::getFloat() const {
        return std::get<float>(value);
    }

    std::string Element::getString() const {
        return std::get<std::string>(value);
    }

    std::shared_ptr<Object> Element::getObjectPtr() const {
        return std::get<std::shared_ptr<Object> >(value);
    }

    std::vector<Element> Element::getVector() const {
        return std::get<std::vector<Element> >(value);
    }


    void skipEmpty(const std::string &text, int &pos) {
        while (text[pos] == ' ' || text[pos] == '\n') {
            pos++;
        }
    }

    std::variant<int, float> json::parseNumber(const std::string &text, int &pos) {
        float res = 0, sign = 1;
        if (text[pos] == '-') {
            pos++;
            sign = -1;
        }
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
            return res * sign;
        }
        return static_cast<int>(res * sign);
    }

    std::string json::parseString(const std::string &text, int &pos) {
        std::string res;
        pos++;
        while (text[pos] != '"') {
            res += text[pos];
            pos++;
        }
        pos++;
        return res;
    }

    Element parseElement(const std::string &text, int &pos) {
        Element res;
        if (isdigit(text[pos]) || text[pos] == '-') {
            if (std::variant<int, float> number = parseNumber(text, pos); holds_alternative<float>(number)) {
                res.value = get<float>(number);
            } else {
                res.value = get<int>(number);
            }
        } else if (text[pos] == '"') {
            res.value = parseString(text, pos);
        } else if (text[pos] == '{') {
            res.value = std::make_shared<Object>(parseObject(text, pos));
        } else if (text[pos] == '[') {
            res.value = parseVector(text, pos);
        } else {
            std::cerr << "Invalid character in JSON file. " << pos << std::endl;
            exit(1);
        }
        skipEmpty(text, pos);
        return res;
    }

    std::vector<Element> parseVector(const std::string &text, int &pos) {
        std::vector<Element> res;
        pos++;
        skipEmpty(text, pos);
        while (text[pos] != ']') {
            if (text[pos] == ',') {
                pos++;
                while (text[pos] == ' ' || text[pos] == '\n') {
                    pos++;
                }
            }
            Element element = parseElement(text, pos);
            res.push_back(element);
        }
        pos++;
        return res;
    }

    Object json::parseObject(const std::string &text, int &pos) {
        Object res;
        while (text[pos] != '}' && pos < text.length()) {
            if (text[pos] == '"') {
                std::string name = parseString(text, pos);
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
                Element element = parseElement(text, pos);
                res.data[name] = element;
            } else {
                pos++;
            }
        }
        pos++;
        return res;
    }

    std::string to_string(const Element &element) {
        std::string res;
        if (element.isInt()) {
            res += std::to_string(get<int>(element.value));
        } else if (element.isFloat()) {
            res += std::to_string(get<float>(element.value));
        } else if (element.isString()) {
            res += get<std::string>(element.value);
        } else if (element.isObjectPtr()) {
            res += to_string(*get<std::shared_ptr<Object> >(element.value));
        } else if (element.isVector()) {
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
