#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>

#include "json.h"

std::string readString(const std::string &s, int &i) {
    std::string res;
    while (s[i] != '.' && s[i] != '[' && s[i] != ']' && i < s.size()) {
        res += s[i];
        i++;
    }
    return res;
}

json::Element readElement(const json::Object &obj, const std::string &s, int &i) {
    json::Element elem;
    elem.value = std::make_shared<json::Object>(obj);
    if (s.substr(i, 4) == "max(") {
        i += 4;

        return elem;
    }
    while (i < s.size() && s[i] != ']' && s[i] != ',' && s[i] != ')') {
        json::skipEmpty(s, i);
        if (s[i] == '[') {
            i++;
            int pos;
            if (isdigit(s[i]) || s[i] == '-') {
                pos = std::get<int>(json::parseNumber(s, i));
                //NIJE GOTOV PROVERI DA JE CEO I POZITIVAN
                i++;
            } else {
                json::Element e = readElement(obj, s, i);
                if (!std::holds_alternative<int>(e.value)) {
                    std::cerr << "Error: " << json::to_string(e) << " is not an integer." << std::endl;
                    exit(1);
                }
                pos = std::get<int>(e.value);
            }
            if (!std::holds_alternative<std::vector<json::Element> >(elem.value)) {
                std::cerr << "Error: " << json::to_string(elem) << " is not an array." << std::endl;
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
                std::cerr << "Error: " << json::to_string(elem) << " does not contain object named " << name << "."
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

int max(std::vector<int> v) {
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
