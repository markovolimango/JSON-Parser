#include <fstream>
#include <sstream>
#include <vector>
#include "json.h"

int main() {
    std::ifstream json("text.json");
    std::stringstream buffer;
    buffer << json.rdbuf();
    std::string text = buffer.str();
    json::Object root;
    int i = 0, j = 0;
    root = json::parseObject(text, i);

    std::string s;
    std::getline(std::cin, s);
    i = 0;

    json::Element e;
    e = json::readElement(root, s, i);
    std::cout << json::to_string(e) << std::endl;
}



