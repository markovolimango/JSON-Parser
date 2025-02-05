#ifndef JSON_H
#define JSON_H

#endif
#include <iostream>
#include <map>
#include <variant>
#include <vector>

namespace json {
    class Object;

    class Element {
    public:
        std::variant<int, float, std::string, std::shared_ptr<Object>, std::vector<Element>> value;

        [[nodiscard]] bool isInt() const;

        [[nodiscard]] bool isFloat() const;

        [[nodiscard]] bool isNumber() const;

        [[nodiscard]] bool isString() const;

        [[nodiscard]] bool isObjectPtr() const;

        [[nodiscard]] bool isVector() const;

        [[nodiscard]] int getInt() const;

        [[nodiscard]] float getFloat() const;

        [[nodiscard]] std::string getString() const;

        [[nodiscard]] std::shared_ptr<Object> getObjectPtr() const;

        [[nodiscard]] std::vector<Element> getVector() const;
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

    float makeFloat(const Element &element);

    float max(const std::vector<Element> &vec);

    float min(const std::vector<Element> &vec);

    std::variant<int, float> readNumber(const std::string &input, int &pos);

    std::string readString(const std::string &input, int &pos);

    float readMax(const Object &root, const std::string &input, int &pos);

    float readMin(const Object &root, const std::string &input, int &pos);

    Element readElement(const Object &root, const std::string &input, int &pos);

    size_t findOperation(const std::string &input);

    float performOperation(const Object &root, const std::string &input, const size_t &op);

    class Error : public std::exception {
    protected:
        std::string msg;

    public:
        explicit Error(const std::string &message);

        [[nodiscard]] const char *what() const noexcept override;
    };

    class JSONError : public Error {
    public:
        JSONError();
    };

    class ExpressionError : public Error {
    public:
        ExpressionError();
    };
} // namespace json
