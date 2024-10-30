#include <iostream>
#include "json.h"

class Tests {
private:
    void printResult(const std::string &testName, bool passed) {
        std::cout << testName << ": " << (passed ? "PASSED" : "FAILED") << std::endl;
    }

    bool runInvalidJSONTest(const std::string &text) {
        bool res;
        try {
            int pos = 0;
            json::Object obj = json::parseObject(text, pos);
            res = false;
        } catch (const json::Error &e) {
            res = true;
        }
        return res;
    }

    bool runValidJSONTest(const std::string text, const std::string result) {
        int pos = 0;
        try {
            json::Object obj = json::parseObject(text, pos);
            return json::to_string(obj) == result;
        } catch (const json::Error &e) {
            return false;
        }
    }

    bool runInvalidExpressionTest(const json::Object &root, const std::string input) {
        try {
            int pos = 0;
            json::Element e = json::readElement(root, input, pos);
            return false;
        } catch (const json::ExpressionError &error) {
            return true;
        }
    }

    bool runValidExpressionTest(json::Object root, std::string input, const std::string result) {
        int pos = 0;
        try {
            json::Element element = json::readElement(root, input, pos);
            return json::to_string(element) == result;
        } catch (const json::ExpressionError &error) {
            return false;
        }
    }

public:
    void testInvalidJSON() {
        printResult("Empty JSON", runInvalidJSONTest(""));

        printResult("Missing closing curly bracket in JSON", runInvalidJSONTest(R"({"marko":"wahoo")"));

        printResult("Invalid number in JSON", "{\"a\":1.45.56}");

        printResult("Missing closing square bracket in JSON", "{\"a\":[1,2,3}");

        std::cout << std::endl;
    }

    void testValidJSON() {
        bool t;

        t = runValidJSONTest(R"({"a": { "b"  : [ 1.000, 2.34, { "c": "test" }, [11, 12] ], "d": -9 }})",
                             R"({"a": {"b": [1, 2.34, {"c": test}, [11, 12]], "d": -9}})");
        printResult("Valid JSON 1", t);

        std::cout << std::endl;
    }

    void testInvalidExpressions() {
        std::string text = R"({"a": { "b"  : [ 1.000, 2.34, { "c": "test" }, [11, 12] ], "d": -9 }})";
        int pos = 0;
        json::Object obj = json::parseObject(text, pos);

        printResult("Object as index expression", runInvalidExpressionTest(obj, "a.b[a.b[2]]"));

        printResult("Non-existent object in expression", runInvalidExpressionTest(obj, "a.c"));

        printResult("max of non-array", runInvalidExpressionTest(obj, "max(a.b[2])"));

        printResult("max of non-numbers", runInvalidExpressionTest(obj, "max(1,2,a.b)"));

        std::cout << std::endl;
    }

    void testValidExpressions() {
        std::string text = R"({"a": { "b"  : [ 1, 2.34, { "c": "test" }, [11, 12] ], "d": -9 }})";
        int pos = 0;
        json::Object obj = json::parseObject(text, pos);

        printResult("Min of elements as array index",
                    runValidExpressionTest(obj, "a.b[min(a.b[1],a.b[3][1],2)]", R"({"c": test})"));

        printResult("Max of elements as array index",
                    runValidExpressionTest(obj, "a.b[3][max(0,a.b[0])]", R"(12)"));

        printResult("Size of an object",
                    runValidExpressionTest(obj, "size(a)", R"(2)"));

        printResult("Object in array in object",
                    runValidExpressionTest(obj, "a.b[2].c", R"(test)"));

        std::cout << std::endl;
    }

    void test() {
        testInvalidJSON();
        testValidJSON();
        testInvalidExpressions();
        testValidExpressions();
    }
};

int main() {
    Tests tests;
    tests.test();
    return 0;
}
