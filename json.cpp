#include "json.h"

namespace json {
    bool Element::isInt() const {
        return std::holds_alternative<int>(value);
    }

    bool Element::isFloat() const {
        return std::holds_alternative<float>(value);
    }

    bool Element::isNumber() const {
        return std::holds_alternative<int>(value) || std::holds_alternative<float>(value);
    }

    bool Element::isString() const {
        return std::holds_alternative<std::string>(value);
    }

    bool Element::isObjectPtr() const {
        return std::holds_alternative<std::shared_ptr<Object> >(value);
    }

    bool Element::isVector() const {
        return std::holds_alternative<std::vector<Element> >(value);
    }

    int Element::getInt() const {
        return std::get<int>(value);
    }

    float Element::getFloat() const {
        return std::get<float>(value);
    }

    std::string Element::getString() const {
        return std::get<std::string>(value);
    }

    std::shared_ptr<Object> Element::getObjectPtr() const {
        return std::get<std::shared_ptr<Object> >(value);
    }

    std::vector<Element> Element::getVector() const {
        return std::get<std::vector<Element> >(value);
    }


    void skipEmpty(const std::string &text, int &pos) {
        while (text[pos] == ' ' || text[pos] == '\n') {
            pos++;
        }
    }

    std::variant<int, float> json::parseNumber(const std::string &text, int &pos) {
        float res = 0, sign = 1;
        if (text[pos] == '-') {
            pos++;
            sign = -1;
        }
        while (isdigit(text[pos])) {
            res = res * 10 + static_cast<float>(text[pos]) - '0';
            pos++;
        }
        if (text[pos] == '.') {
            pos++;
            float decimalPower = 10;
            while (isdigit(text[pos])) {
                res += static_cast<float>(text[pos] - '0') / decimalPower;
                decimalPower *= 10;
                pos++;
            }
            return res * sign;
        }
        return static_cast<int>(res * sign);
    }

    std::string json::parseString(const std::string &text, int &pos) {
        std::string res;
        pos++;
        while (text[pos] != '"') {
            if (pos >= text.size()) {
                std::cerr << "Error: Missing closing quotation mark in JSON." << std::endl;
                exit(1);
            }
            res += text[pos];
            pos++;
        }
        pos++;
        return res;
    }

    Element parseElement(const std::string &text, int &pos) {
        Element res;
        if (isdigit(text[pos]) || text[pos] == '-') {
            if (std::variant<int, float> number = parseNumber(text, pos); holds_alternative<float>(number)) {
                res.value = get<float>(number);
            } else {
                res.value = get<int>(number);
            }
        } else if (text[pos] == '"') {
            res.value = parseString(text, pos);
        } else if (text[pos] == '{') {
            res.value = std::make_shared<Object>(parseObject(text, pos));
        } else if (text[pos] == '[') {
            res.value = parseVector(text, pos);
        } else {
            std::cerr << "Error: Invalid character " << text[pos] << " in JSON." << std::endl;
            exit(1);
        }
        skipEmpty(text, pos);
        return res;
    }

    std::vector<Element> parseVector(const std::string &text, int &pos) {
        std::vector<Element> res;
        pos++;
        skipEmpty(text, pos);
        while (text[pos] != ']') {
            if (pos >= text.size()) {
                std::cerr << "Error: Missing closing square bracket in JSON." << std::endl;
                exit(1);
            }
            if (text[pos] == ',') {
                pos++;
                skipEmpty(text, pos);
            }
            Element element = parseElement(text, pos);
            res.push_back(element);
        }
        pos++;
        return res;
    }

    Object json::parseObject(const std::string &text, int &pos) {
        Object res;
        skipEmpty(text, pos);
        if (text[pos] != '{') {
            std::cerr << "Error: Missing opening curly bracket in JSON." << std::endl;
            exit(1);
        }
        while (text[pos] != '}') {
            if (text[pos] == '"') {
                if (pos > text.size()) {
                    std::cerr << "Error: Missing closing curly bracket in JSON." << std::endl;
                    exit(1);
                }
                std::string name = parseString(text, pos);
                while (text[pos] != ':') {
                    if (text[pos] != ' ' || text[pos] != '\n') {
                        std::cerr << "Missing colon in JSON file." << std::endl;
                        exit(1);
                    }
                    pos++;
                }
                pos++;
                while (text[pos] == ' ' || text[pos] == '\n') {
                    pos++;
                }
                const Element element = parseElement(text, pos);
                res.data[name] = element;
            } else {
                pos++;
            }
        }
        if (text[pos] == '}') {
            pos++;
        }
        return res;
    }

    std::string to_string(const float &number) {
        std::string res = std::to_string(number);
        int pos = 0;
        while (res.back() == '0' && pos < 6) {
            res.pop_back();
            pos++;
        }
        if (pos == 6) {
            res.pop_back();
        }
        return res;
    }

    std::string to_string(const Element &element) {
        std::string res;
        if (element.isInt()) {
            res += std::to_string(element.getInt());
        } else if (element.isFloat()) {
            res += to_string(element.getFloat());
        } else if (element.isString()) {
            res += get<std::string>(element.value);
        } else if (element.isObjectPtr()) {
            res += to_string(*get<std::shared_ptr<Object> >(element.value));
        } else if (element.isVector()) {
            res += to_string(get<std::vector<Element> >(element.value));
        }
        return res;
    }

    std::string to_string(const std::vector<Element> &vec) {
        std::string res;
        if (vec.size() == 1) {
            res += to_string(vec[0]);
            return res;
        }
        res += "[";
        for (const auto &element: vec) {
            res += to_string(element) + ", ";
        }
        if (!vec.empty()) {
            res.pop_back();
            res.pop_back();
        }
        res += "]";
        return res;
    }

    std::string to_string(const Object &object) {
        std::string res;
        res += "{";
        for (const auto &[name, element]: object.data) {
            res += "\"" + name + "\": " + to_string(element) + ", ";
        }
        if (!object.data.empty()) {
            res.pop_back();
            res.pop_back();
        }
        res += "}";
        return res;
    }

    float makeFloat(const Element &element) {
        float res;
        if (!element.isFloat()) {
            if (!element.isInt()) {
                std::cerr << "Error: " << to_string(element) << "in expression is not a number." << std::endl;
                exit(1);
            }
            res = static_cast<float>(element.getInt());
        } else {
            res = element.getFloat();
        }
        return res;
    }

    float max(const std::vector<Element> &vec) {
        float max = -1 * std::numeric_limits<float>::max();
        for (const auto &element: vec) {
            if (const float current = makeFloat(element); current > max) {
                max = current;
            }
        }
        return max;
    }

    float min(const std::vector<Element> &vec) {
        float min = std::numeric_limits<float>::max();
        for (const auto &element: vec) {
            if (const float current = makeFloat(element); current < min) {
                min = current;
            }
        }
        return min;
    }

    std::variant<int, float> readNumber(const std::string &input, int &pos) {
        return parseNumber(input, pos);
    }

    std::string readString(const std::string &input, int &pos) {
        std::string res;
        while (input[pos] != '.' && input[pos] != '[' && input[pos] != ']' && input[pos] != ')' && pos < input.size()) {
            res += input[pos];
            pos++;
        }
        return res;
    }

    float readMax(const Object &root, const std::string &input, int &pos) {
        float res;
        pos += 4;
        if (Element element = readElement(root, input, pos); element.isVector()) {
            res = max(element.getVector());
        } else {
            std::vector<Element> elements;
            elements.push_back(element);
            pos++;
            while (input[pos] != ')' && pos < input.size()) {
                if (input[pos] == ',') {
                    pos++;
                }
                element = readElement(root, input, pos);
                elements.push_back(element);
            }
            res = max(elements);
        }
        if (input[pos] == ')') {
            pos++;
        }
        return res;
    }

    float readMin(const Object &root, const std::string &input, int &pos) {
        float res;
        pos += 4;
        if (Element element = readElement(root, input, pos); element.isVector()) {
            res = min(element.getVector());
            pos++;
        } else {
            std::vector<Element> elements;
            elements.push_back(element);
            pos++;
            while (input[pos] != ')' && pos < input.size()) {
                if (input[pos] == ',') {
                    pos++;
                }
                element = readElement(root, input, pos);
                elements.push_back(element);
            }
            res = min(elements);
        }
        if (input[pos] == ')') {
            pos++;
        }
        return res;
    }

    Element readElement(const Object &root, const std::string &input, int &pos) {
        skipEmpty(input, pos);
        Element res;
        res.value = std::make_shared<Object>(root);
        if (isdigit(input[pos]) || input[pos] == '-') {
            if (std::variant<int, float> num = readNumber(input, pos); holds_alternative<float>(num)) {
                res.value = std::get<float>(num);
            } else if (holds_alternative<int>(num)) {
                res.value = std::get<int>(num);
            }
        } else if (input.substr(pos, 4) == "max(") {
            res.value = readMax(root, input, pos);
        } else if (input.substr(pos, 4) == "min(") {
            res.value = readMin(root, input, pos);
        } else if (input.substr(pos, 5) == "size(") {
            pos += 5;
            int size;
            res = readElement(root, input, pos);
            if (res.isVector()) {
                size = static_cast<int>(res.getVector().size());
            } else if (res.isObjectPtr()) {
                size = static_cast<int>(res.getObjectPtr()->data.size());
            }
            res.value = size;
            if (input[pos] == ')') {
                pos++;
            }
        } else {
            while (pos < input.size() && input[pos] != ']' && input[pos] != ',' && input[pos] != ')') {
                skipEmpty(input, pos);
                if (input[pos] == '[') {
                    pos++;
                    int index;
                    if (isdigit(input[pos]) || input[pos] == '-') {
                        std::variant<int, float> number = readNumber(input, pos);
                        if (!std::holds_alternative<int>(number)) {
                            std::cerr << "Error: " << std::to_string(std::get<float>(number)) <<
                                    " in expression is not an integer." << std::endl;
                            exit(1);
                        }
                        index = std::get<int>(number);
                        pos++;
                    } else {
                        if (Element element = readElement(root, input, pos); !element.isInt()) {
                            if (element.isFloat()) {
                                if (float indexMaybe = element.getFloat(); to_string(indexMaybe) == std::to_string(
                                                                               static_cast<int>(indexMaybe))) {
                                    index = static_cast<int>(indexMaybe);
                                } else {
                                    std::cerr << "Error: " << to_string(element) << " in expression is not an integer."
                                            << std::endl;
                                    exit(1);
                                }
                            } else {
                                std::cerr << "Error: " << to_string(element) << " in expression is not an integer." <<
                                        std::endl;
                                exit(1);
                            }
                        } else {
                            index = element.getInt();
                        }
                    }
                    if (!res.isVector()) {
                        std::cerr << "Error: " << to_string(res) << " in expression is not an array." << std::endl;
                        exit(1);
                    }
                    if (index < 0) {
                        std::cerr << "Error: Array index " << index << " in expression is negative." << std::endl;
                        exit(1);
                    }
                    auto elements = res.getVector();
                    res = elements[index];
                } else {
                    if (input[pos] == '.') {
                        pos++;
                    }
                    std::string name = readString(input, pos);
                    if (!res.isObjectPtr()) {
                        std::cerr << "Error: " << to_string(res) << " in expression is not an object." << std::endl;
                        exit(1);
                    }
                    auto [data] = *std::get<std::shared_ptr<json::Object> >(res.value);
                    if (!data.contains(name)) {
                        std::cerr << "Error: " << json::to_string(res) << " does not contain object named \"" << name <<
                                "\"." << std::endl;
                        exit(1);
                    }
                    res = data[name];
                }
            }
        }
        if (input[pos] == ']') {
            pos++;
        }
        skipEmpty(input, pos);
        return res;
    }
}
