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
    try {
        root = json::parseObject(text, i);
    } catch (const json::Error &erke) {
        std::cerr << erke.what() << std::endl;
        exit(1);
    }
    std::string s;
    std::getline(std::cin, s);
    i = 0;

    json::Element e;
    try {
        e = json::readElement(root, s, i);
    } catch (const json::Error &erke) {
        std::cerr << erke.what() << std::endl;
        exit(1);
    }
    std::cout << json::to_string(e) << std::endl;
}



