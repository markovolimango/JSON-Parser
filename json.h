#ifndef JSON_H
#define JSON_H

#endif
#include <iostream>
#include <map>
#include <vector>
#include <variant>

namespace json {
    class Object;

    class Element {
    public:
        std::variant<int, float, std::string, std::shared_ptr<Object>, std::vector<Element> > value;
    };

    class Object {
    public:
        std::map<std::string, Element> data;
    };

    void skipEmpty(const std::string &text, int &pos);

    std::variant<int, float> parseNumber(const std::string &text, int &pos);

    std::string parseString(const std::string &text, int &pos);

    Element parseElement(const std::string &text, int &pos);

    std::vector<Element> parseVector(const std::string &text, int &pos);

    Object parseObject(const std::string &text, int &pos);

    std::string to_string(const Element &element);

    std::string to_string(const std::vector<Element> &vec);

    std::string to_string(const Object &object);
}
