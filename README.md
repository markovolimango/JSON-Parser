# JSON Parser

A C++ app that parses JSON and evaluates user expressions.

## Features

- Evaluating trivial expressions with JSON paths
- min() and max() functions
    - Work with an array or multiple arguments
- size() function
    - Works with arrays and objects
- Expressions and functions in the subscript operator []
    - Allows for recursive function and expression calls
- Binary arithmetic operations with expressions

## Using the Application

### Prerequisites
- C++ 20 or later
- CMake 3.29 or later

### Buidling the Project
- Clone the repository.
```bash
git clone https://github.com/markovolimango/JSON-Parser.git
```
- Create a build directory and navigate to it.
```bash
cd JSON-Parser
mkdir build
cd build
```
- Generate the build files using CMake.
```bash
cmake ..
```
- Build the project.
```bash
make
```

### Running the expression evaluation
- Place your JSON file in the build directory.
- Run the *json_eval* executable and input the file name and expression.
```bash
./json_eval
[filename] [expression]
```
Example:
```bash
./json_eval
test.json a.b[1]
```

### Running the tests
- Just run the *tests* executable.
```bash
./tests
```
