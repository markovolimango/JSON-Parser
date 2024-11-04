#include <iostream>
#include "json.h"

class Tests {
    static void printResult(const std::string &testName, bool passed) {
        std::cout << testName << ": " << (passed ? "PASSED" : "FAILED") << std::endl;
    }

    static bool runInvalidJSONTest(const std::string &text) {
        bool res;
        try {
            int pos = 0;
            json::Object obj = json::parseObject(text, pos);
            res = false;
        } catch ([[maybe_unused]] const json::JSONError &e) {
            res = true;
        }
        return res;
    }

    static bool runValidJSONTest(const std::string &text, const std::string &expectedResult) {
        try {
            int pos = 0;
            const json::Object obj = json::parseObject(text, pos);
            return to_string(obj) == expectedResult;
        } catch ([[maybe_unused]] const json::JSONError &e) {
            return false;
        }
    }

    static bool runInvalidExpressionTest(const json::Object &root, const std::string &input) {
        try {
            int pos = 0;
            json::Element element = readElement(root, input, pos);
            return false;
        } catch ([[maybe_unused]] const json::ExpressionError &e) {
            return true;
        }
    }

    static bool runValidExpressionTest(const json::Object &root, const std::string &input,
                                       const std::string &expectedResult) {
        try {
            int pos = 0;
            const json::Element element = readElement(root, input, pos);
            return to_string(element) == expectedResult;
        } catch ([[maybe_unused]] const json::ExpressionError &e) {
            return false;
        }
    }

    static bool runOperationTest(const json::Object &root, const std::string &input, const float expectedResult) {
        const size_t op = json::findOperation(input);
        try {
            float x = performOperation(root, input, op);
            return x == expectedResult;
        } catch ([[maybe_unused]] const json::ExpressionError &e) {
            return false;
        }
    }

public:
    static void testInvalidJSON() {
        std::cout << "INVALID JSON" << std::endl;

        printResult("Empty JSON", runInvalidJSONTest(""));

        printResult("Missing closing curly bracket in JSON", runInvalidJSONTest(R"({"marko":"yahoo")"));

        printResult("Invalid number in JSON", runInvalidJSONTest("{\"a\":1.45.56}"));

        printResult("Missing closing square bracket in JSON", runInvalidJSONTest("{\"a\":[1,2,3}"));

        std::cout << std::endl;
    }

    static void testValidJSON() {
        std::cout << "VALID JSON" << std::endl;

        bool passed = runValidJSONTest(R"({"a": { "b"  : [ 1.000, 2.34, { "c": "test" }, [11, 12] ], "d": -9 }})",
                                       R"({"a": {"b": [1, 2.34, {"c": "test"}, [11, 12]], "d": -9}})");
        printResult("Multiple objects and arrays", passed);

        passed = runValidJSONTest(R"({"marko":"volimango","longname":[1,2,"three"]})",
                                  R"({"longname": [1, 2, "three"], "marko": "volimango"})");
        printResult("Longer object names", passed);

        passed = runValidJSONTest(R"({"arr":[{"a":1,"b":2},{"c":3},{"d":4}]})",
                                  R"({"arr": [{"a": 1, "b": 2}, {"c": 3}, {"d": 4}]})");
        printResult("Array of objects", passed);

        std::cout << std::endl;
    }

    static void testInvalidExpressions() {
        std::cout << "INVALID EXPRESSIONS" << std::endl;

        std::string text = R"({"a": { "b"  : [ 1, 2.34, { "c": "test" }, [11, 12] ], "d": -9 }})";
        int pos = 0;
        json::Object obj = json::parseObject(text, pos);

        printResult("Object as index", runInvalidExpressionTest(obj, "a.b[a.b[2]]"));

        printResult("Non-existent object", runInvalidExpressionTest(obj, "a.c"));

        printResult("Max of non-array", runInvalidExpressionTest(obj, "max(a.b[2])"));

        printResult("Min of non-numbers", runInvalidExpressionTest(obj, "min(1,2,a.b)"));

        text = R"({"arr":[{"a":1,"b":2},{"c":3},{"d":4}]})";
        pos = 0;
        obj = json::parseObject(text, pos);

        printResult("Multiplication sign in the wrong place", runInvalidExpressionTest(obj, "arr[0].*aarr[1].c"));

        std::cout << std::endl;
    }

    static void testValidExpressions() {
        std::cout << "VALID EXPRESSIONS" << std::endl;

        std::string text = R"({"a": { "b"  : [ 1, 2.34, { "c": "test" }, [11, 12] ], "d": -9 }})";
        int pos = 0;
        json::Object obj = json::parseObject(text, pos);

        printResult("Min of elements as array index",
                    runValidExpressionTest(obj, "a.b[min(a.b[1],a.b[3][1],2)]", R"({"c": "test"})"));

        printResult("Max of elements as array index", runValidExpressionTest(obj, "a.b[3][max(0,a.b[0])]", R"(12)"));

        printResult("Size of an object", runValidExpressionTest(obj, "size(a)", R"(2)"));

        printResult("Object in array in object", runValidExpressionTest(obj, "a.b[2].c", R"("test")"));

        printResult("Multiplication", runOperationTest(obj, "a.d*a.b[3][1]", -108));

        printResult("Addition with functions", runOperationTest(obj, "max(a.b[0],a.b[1],1.89)+size(a)", 4.34));

        std::cout << std::endl;
    }

    static void test() {
        testInvalidJSON();
        testValidJSON();
        testInvalidExpressions();
        testValidExpressions();
    }
};

int main() {
    Tests::test();
    return 0;
}
