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

    std::cout << std::get<int>(std::get<std::shared_ptr<json::Object> >(obj.data["a"][0].value)->data["b"][1].value);
    std::string input = "a.b[1]";
}
