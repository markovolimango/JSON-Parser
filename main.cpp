#include <fstream>
#include <sstream>
#include "json.h"

int main() {
    std::ifstream json("text.json");
    std::stringstream buffer;
    buffer << json.rdbuf();
    std::string s = buffer.str();
    std::string out;
    json::Object obj;
    int i = 0;

    obj = json::readObj(s, i);

    out = to_string(obj);
    std::cout << out << std::endl;

    //std::cout << std::get<int>(std::get<std::shared_ptr<json::Object> >(obj.data["a"][0].value)->data["b"][1].value);
    std::string input = "a.b[1]";
    /*json::Element elem;
    elem.value = std::make_shared<json::Object>(obj);
    i = 0;
    while (i < input.size()) {
        std::cout << json::to_string(elem) << std::endl;
        std::string name = "";
        while (input[i] != '.' && input[i] != '[' && i < input.size()) {
            name += input[i];
            i++;
        }
        if (input[i] == '.') {
            elem = std::get<std::shared_ptr<json::Object> >(elem.value)->data[name][0];
            i++;
        } else if (input[i] == '[') {
            i++;
            int pos = s[i] - '0';
            elem = std::get<std::shared_ptr<json::Object> >(elem.value)->data[name][pos];
            i++;
        } else {
            i++;
        }
    }
    std::cout << json::to_string(elem);*/
}
