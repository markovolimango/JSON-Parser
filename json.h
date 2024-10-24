#ifndef JSON_H
#define JSON_H

#endif
#include <iostream>
#include <map>
#include <vector>
#include <variant>

namespace json {
    class Obj;

    class element {
    public:
        std::variant<int, float, std::string, std::shared_ptr<Obj>, std::vector<element> > val;
    };

    class Obj {
    public:
        std::map<std::string, std::vector<element> > data;
    };

    std::variant<int, float> readNum(const std::string &s, int &i);

    std::string readString(const std::string &s, int &i);

    element readElement(const std::string &s, int &i);

    std::vector<element> readVector(const std::string &s, int &i);

    Obj parse(const std::string &s, int &i);

    std::string to_string(const element &el);

    std::string to_string(const std::vector<element> &v);

    std::string to_string(const Obj &obj);
}
