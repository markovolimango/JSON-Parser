#include <fstream>
#include <sstream>
#include <vector>
#include <limits>

#include "json.h"

float makeFloat(const json::Element &i) {
    float t;
    if (!i.isFloat()) {
        if (!i.isInt()) {
            std::cerr << "Error: " << json::to_string(i) << " is not a number." << std::endl;
            exit(1);
        }
        t = static_cast<float>(i.getInt());
    } else {
        t = i.getFloat();
    }
    return t;
}

float max(const std::vector<json::Element> &v) {
    float max = -1 * std::numeric_limits<float>::max();
    for (const auto &i: v) {
        if (const float t = makeFloat(i); t > max) {
            max = t;
        }
    }
    return max;
}

float min(const std::vector<json::Element> &v) {
    float min = std::numeric_limits<float>::max();
    for (const auto &i: v) {
        if (const float t = makeFloat(i); t < min) {
            min = t;
        }
    }
    return min;
}

std::variant<int, float> readNumber(const std::string &s, int &i) {
    return json::parseNumber(s, i);
}

std::string readString(const std::string &s, int &i) {
    std::string res;
    while (s[i] != '.' && s[i] != '[' && s[i] != ']' && s[i] != ')' && i < s.size()) {
        res += s[i];
        i++;
    }
    return res;
}

json::Element readElement(const json::Object &obj, const std::string &s, int &i);

float readMax(const json::Object &obj, const std::string &s, int &i) {
    json::Element elem;
    std::vector<json::Element> elems;
    float res;
    i += 4;
    elem = readElement(obj, s, i);
    if (elem.isVector()) {
        res = max(elem.getVector());
    } else {
        elems.push_back(elem);
        i++;
        while (s[i] != ')' && i < s.size()) {
            if (s[i] == ',') {
                i++;
            }
            elem = readElement(obj, s, i);
            elems.push_back(elem);
        }
        res = max(elems);
    }
    if (s[i] == ')') {
        i++;
    }
    return res;
}

float readMin(const json::Object &obj, const std::string &s, int &i) {
    json::Element elem;
    std::vector<json::Element> elems;
    float res;
    i += 4;
    elem = readElement(obj, s, i);
    if (elem.isVector()) {
        res = min(elem.getVector());
        i++;
    } else {
        elems.push_back(elem);
        i++;
        while (s[i] != ')' && i < s.size()) {
            if (s[i] == ',') {
                i++;
            }
            elem = readElement(obj, s, i);
            elems.push_back(elem);
        }
        res = min(elems);
    }
    if (s[i] == ')') {
        i++;
    }
    return res;
}

json::Element readElement(const json::Object &obj, const std::string &s, int &i) {
    json::Element elem;
    elem.value = std::make_shared<json::Object>(obj);
    if (isdigit(s[i]) || s[i] == '-') {
        if (std::variant<int, float> num = readNumber(s, i); holds_alternative<float>(num)) {
            elem.value = std::get<float>(num);
        } else if (holds_alternative<int>(num)) {
            elem.value = std::get<int>(num);
        }
    } else if (s.substr(i, 4) == "max(") {
        elem.value = readMax(obj, s, i);
    } else if (s.substr(i, 4) == "min(") {
        elem.value = readMin(obj, s, i);
    } else if (s.substr(i, 5) == "size(") {
        i += 5;
        int size;
        elem = readElement(obj, s, i);
        if (elem.isVector()) {
            size = static_cast<int>(elem.getVector().size());
        } else if (elem.isObjectPtr()) {
            size = static_cast<int>(elem.getObjectPtr()->data.size());
        }
        elem.value = size;
        if (s[i] == ')') {
            i++;
        }
    } else {
        while (i < s.size() && s[i] != ']' && s[i] != ',' && s[i] != ')') {
            json::skipEmpty(s, i);
            if (s[i] == '[') {
                i++;
                int pos;
                if (isdigit(s[i]) || s[i] == '-') {
                    std::variant<int, float> tempPos = readNumber(s, i);
                    if (!std::holds_alternative<int>(tempPos)) {
                        std::cerr << "Error: Array index " << std::to_string(std::get<float>(tempPos)) <<
                                " is not an integer." << std::endl;
                        exit(1);
                    }
                    pos = std::get<int>(tempPos);
                    i++;
                } else {
                    json::Element e = readElement(obj, s, i);
                    if (!e.isInt()) {
                        if (e.isFloat()) {
                            float t = e.getFloat();
                            if (floor(t) == t) {
                                pos = static_cast<int>(t);
                            } else {
                                std::cerr << "Error: " << json::to_string(e) << " is not an integer." << std::endl;
                                exit(1);
                            }
                        } else {
                            std::cerr << "Error: " << json::to_string(e) << " is not an integer." << std::endl;
                            exit(1);
                        }
                    } else {
                        pos = e.getInt();
                    }
                }
                if (!elem.isVector()) {
                    std::cerr << "Error: " << json::to_string(elem) << " is not an array." << std::endl;
                    exit(1);
                }
                if (pos < 0) {
                    std::cerr << "Error: Array index " << pos << " is negative." << std::endl;
                    exit(1);
                }
                auto elems = elem.getVector();
                elem = elems[pos];
            } else {
                if (s[i] == '.') {
                    i++;
                }
                std::string name = readString(s, i);
                if (!elem.isObjectPtr()) {
                    std::cerr << "Error: " << json::to_string(elem) << " is not an object." << std::endl;
                    exit(1);
                }
                auto [data] = *std::get<std::shared_ptr<json::Object> >(elem.value);
                if (!data.contains(name)) {
                    std::cerr << "Error: " << json::to_string(elem) << " does not contain object named " << name
                            << "."
                            <<
                            std::endl;
                    exit(1);
                }
                elem = data[name];
            }
        }
    }
    if (s[i] == ']') {
        i++;
    }
    return elem;
}

int main() {
    std::ifstream json("text.json");
    std::stringstream buffer;
    buffer << json.rdbuf();
    std::string text = buffer.str();
    json::Object obj;
    int i = 0, j = 0;
    obj = json::parseObject(text, i);

    std::string s;
    std::cin >> s;
    i = 0;
    json::Element elem;
    float res;
    std::vector<json::Element> elems;

    if (s[i] == ')') {
        i++;
    }
    json::Element e = readElement(obj, s, i);
    std::cout << json::to_string(e) << std::endl;
}



