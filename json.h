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

    std::variant<int, float> readNumber(const std::string &text, int &pos);

    std::string readString(const std::string &text, int &pos);

    Element readElement(const std::string &text, int &pos);

    std::vector<Element> readVector(const std::string &text, int &pos);

    Object readObj(const std::string &text, int &pos);

    std::string to_string(const Element &element);

    std::string to_string(const std::vector<Element> &vec);

    std::string to_string(const Object &object);
}
