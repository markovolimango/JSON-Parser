#include <fstream>
#include <sstream>
#include <vector>
#include <limits>

#include "json.h"

float getFloat(const json::Element &i) {
    float t;
    if (!std::holds_alternative<float>(i.value)) {
        if (!std::holds_alternative<int>(i.value)) {
            std::cerr << "Error: " << json::to_string(i) << "is not a number." << std::endl;
            exit(1);
        }
        t = static_cast<float>(std::get<int>(i.value));
    } else {
        t = std::get<float>(i.value);
    }
    return t;
}

float max(const std::vector<json::Element> &v) {
    float max = -1 * std::numeric_limits<float>::max();
    for (const auto &i: v) {
        if (const float t = getFloat(i); t > max) {
            max = t;
        }
    }
    return max;
}

float min(const std::vector<json::Element> &v) {
    float min = -1 * std::numeric_limits<float>::max();
    for (const auto &i: v) {
        if (const float t = getFloat(i); t < min) {
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

json::Element readElement(const json::Object &obj, const std::string &s, int &i) {
    json::Element elem;
    elem.value = std::make_shared<json::Object>(obj);
    if (isdigit(s[i]) || s[i] == '-') {
        if (std::variant<int, float> num = readNumber(s, i); holds_alternative<float>(num)) {
            elem.value = std::get<float>(num);
        } else if (holds_alternative<int>(num)) {
            elem.value = std::get<int>(num);
        }
        return elem;
    }
    if (s.substr(i, 4) == "max(") {
        i += 4;
        elem = readElement(obj, s, i);
        if (std::holds_alternative<std::vector<json::Element> >(elem.value)) {
            if (s[i] == ',') {
                std::cerr << "Error: " << json::to_string(elem) << " is not an number." <<
                        std::endl;
                exit(1);
            }
            elem.value = max(std::get<std::vector<json::Element> >(elem.value));
        } else {
            float max, t = getFloat(elem);
            max = t;
            i++;
            while (s[i] != ')' && i < s.size()) {
                if (s[i] == ',') {
                    i++;
                }
                elem = readElement(obj, s, i);
                t = getFloat(elem);
                if (t > max) {
                    max = t;
                }
            }
            elem.value = max;
        }
        if (s[i] == ')') {
            i++;
        }
        return elem;
    }
    if (s.substr(i, 4) == "min(") {
        i += 4;
        elem = readElement(obj, s, i);
        if (std::holds_alternative<std::vector<json::Element> >(elem.value)) {
            if (s[i] == ',') {
                std::cerr << "Error: Argument " << json::to_string(elem) << " of min() is not a number." <<
                        std::endl;
                exit(1);
            }
            elem.value = max(std::get<std::vector<json::Element> >(elem.value));
        } else {
            float min, t = getFloat(elem);
            min = t;
            i++;
            while (s[i] != ')' && i < s.size()) {
                if (s[i] == ',') {
                    i++;
                }
                elem = readElement(obj, s, i);
                t = getFloat(elem);
                if (t < min) {
                    min = t;
                }
            }
            elem.value = min;
        }
        if (s[i] == ')') {
            i++;
        }
        return elem;
    }
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
                if (!std::holds_alternative<int>(e.value)) {
                    if (std::holds_alternative<float>(e.value)) {
                        float t = std::get<float>(e.value);
                        if (static_cast<int>(t) == t) {
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
                    pos = std::get<int>(e.value);
                }
            }
            if (!std::holds_alternative<std::vector<json::Element> >(elem.value)) {
                std::cerr << "Error: " << json::to_string(elem) << " is not an array." << std::endl;
                exit(1);
            }
            if (pos < 0) {
                std::cerr << "Error: Array index " << pos << " is negative." << std::endl;
                exit(1);
            }
            auto elems = std::get<std::vector<json::Element> >(elem.value);
            elem = elems[pos];
        } else {
            if (s[i] == '.') {
                i++;
            }
            std::string name = readString(s, i);
            if (!std::holds_alternative<std::shared_ptr<json::Object> >(elem.value)) {
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

    std::string input;
    std::cin >> input;
    i = 0;
    json::Element elem = readElement(obj, input, i);
    std::cout << json::to_string(elem) << std::endl;
}
