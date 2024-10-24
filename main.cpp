#include <fstream>
#include <sstream>
#include <vector>

#include "json.h"

void readInput(json::Element &elem, const std::string &s, int &i) {
    while (i < s.size()) {
        if (s[i] == '.') {
            i++;
            std::string name;
            while (s[i] != '.' && s[i] != '[' && i < s.size()) {
                name += s[i];
                i++;
            }
            elem = std::get<std::shared_ptr<json::Object> >(elem.value)->data[name];
        } else if (s[i] == '[') {
            i++;
            int pos = 0;
            while (s[i] != ']') {
                pos = pos * 10 + (s[i] - '0');
                i++;
            }
            if (std::holds_alternative<std::vector<json::Element> >(elem.value)) {
                std::vector<json::Element> elems = std::get<std::vector<json::Element> >(elem.value);
                elem = elems[pos];
            } else {
                std::cout << "nema";
            }
            i++;
        } else {
            i++;
        }
    }
}

int main() {
    std::ifstream json("text.json");
    std::stringstream buffer;
    buffer << json.rdbuf();
    std::string text = buffer.str();
    std::string out;
    json::Object obj;
    int i = 0;

    obj = json::readObj(text, i);
    std::cout << json::to_string(obj) << "\n";

    std::string input = ".aca[1].b[3][0]";
    json::Element elem;
    elem.value = std::make_shared<json::Object>(obj);
    i = 0;
    readInput(elem, input, i);
    std::cout << json::to_string(elem);
}
