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

        bool isInt() const;

        bool isFloat() const;

        bool isNumber() const;

        bool isString() const;

        bool isObjectPtr() const;

        bool isVector() const;

        int getInt() const;

        float getFloat() const;

        std::string getString() const;

        std::shared_ptr<Object> getObjectPtr() const;

        std::vector<Element> getVector() const;
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

    std::string to_string(const float &number);

    std::string to_string(const Element &element);

    std::string to_string(const std::vector<Element> &vec);

    std::string to_string(const Object &object);

    float makeFloat(const json::Element &element);

    float max(const std::vector<json::Element> &vec);

    float min(const std::vector<json::Element> &vec);

    std::variant<int, float> readNumber(const std::string &input, int &pos);

    std::string readString(const std::string &input, int &pos);

    float readMax(const json::Object &root, const std::string &input, int &pos);

    float readMin(const json::Object &root, const std::string &input, int &pos);

    json::Element readElement(const json::Object &root, const std::string &input, int &pos);

    class Error : public std::exception {
    protected:
        std::string msg;

    public:
        Error(const std::string &message);

        const char *what() const noexcept override;
    };

    class JSONError : public Error {
    public:
        JSONError(const std::string &message);
    };

    class ExpressionError : public Error {
    public:
        ExpressionError(const std::string &message);
    };
}
