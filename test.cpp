#include <iostream>
#include "json.h"

class Tests {
private:
    void printResult(const std::string &testName, bool passed) {
        std::cout << testName << ": " << (passed ? "PASSED" : "FAILED") << std::endl;
    }

public:
    void testInvalidJSON() {
        json::Object obj;

        bool t1;
        try {
            std::string text = "";
            int pos = 0;
            obj = json::parseObject(text, pos);
            t1 = false;
        } catch (const json::Error &e) {
            t1 = true;
        }
        printResult("Empty JSON", t1);
    }
};

int main() {
    Tests tests;
    tests.testInvalidJSON();
    return 0;
}
