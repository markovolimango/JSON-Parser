#include <fstream>
#include <sstream>
#include <vector>
#include "json.h"

int main() {
    std::ifstream json("text.json");
    std::stringstream buffer;
    buffer << json.rdbuf();
    std::string text = buffer.str();
    json::Object obj;
    int i = 0, j = 0;
    obj = json::parseObject(text, i);

    std::string s;
    std::getline(std::cin, s);
    i = 0;
    json::Element elem;
    float res;
    std::vector<json::Element> elems;

    json::Element e = readElement(obj, s, i);
    std::cout << json::to_string(e) << std::endl;
}



