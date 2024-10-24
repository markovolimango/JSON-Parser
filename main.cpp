#include <fstream>
#include <sstream>
#include <vector>

#include "json.h"

json::Element readInput(json::Element elem, std::string s, int i) {
    while (i < s.size()) {
        if (s[i] == '.') {
            i++;
            std::string name;
            name = s[i];
            elem = std::get<std::shared_ptr<json::Object> >(elem.value)->data[name];
            i++;
        } else if (s[i] == '[') {
            i++;
            int pos = s[i] - '0';
            if (std::holds_alternative<std::vector<json::Element> >(elem.value)) {
                std::vector<json::Element> elems = std::get<std::vector<json::Element> >(elem.value);
                elem = elems[pos];
            } else {
                std::cout << "nema";
            }
            i += 2;
        }
    }
    return elem;
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

    std::string input = ".a.b[1]";
    json::Element elem;
    elem.value = std::make_shared<json::Object>(obj);
    i = 0;
    elem = readInput(elem, input, i);
    std::cout << json::to_string(elem);
}
