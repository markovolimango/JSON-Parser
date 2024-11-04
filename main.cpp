#include "json.h"
#include <fstream>
#include <sstream>
#include <vector>

int main() {
  std::string filename, text, input;
  std::stringstream buffer;
  json::Object root;
  json::Element res;
  std::ifstream file;

  std::cin >> filename;
  file.open(filename);
  if (!file) {
    std::cerr << "Error opening file " << filename << "." << std::endl;
    return 1;
  }
  buffer << file.rdbuf();
  text = buffer.str();
  int i = 0;
  try {
    root = json::parseObject(text, i);
  } catch (const json::Error &error) {
    std::cerr << error.what() << std::endl;
    return 1;
  }

  std::cin >> input;

  i = 0;
  size_t op = json::findOperation(input);
  if (op != std::string::npos) {
    float x;
    try {
      x = json::performOperation(root, input, op);
    } catch (const json::Error &error) {
      std::cerr << error.what() << std::endl;
      return 1;
    }
    std::cout << json::to_string(x) << std::endl;
    return 0;
  }

  try {
    res = json::readElement(root, input, i);
  } catch (const json::Error &error) {
    std::cerr << error.what() << std::endl;
    return 1;
  }
  std::cout << json::to_string(res) << std::endl;

  return 0;
}
