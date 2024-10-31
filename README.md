# JSON Parser

A C++ app that parses JSON and evaluates user expressions.

## Features

- Evaluating trivial expressions with JSON paths
- min() and max() functions
    - Work with an array or multiple arguments
- size() function
    - Works with arrays and objects
- Expressions and functions in the subscript operator []
    - Allow for recursive function and expression calls

## Getting Started

### Prerequisites
- C++ 20 or later
- CMake 3.29 or later

### Buidling the Project
- Clone the repository.
- Build the project using CMake.

### Running the expression evaluation
- Place your JSON file in the same directory as the executable.
- Run the *json_eval* executable and input the file name and expression:  
```console
./json_eval [filename] [expression]
```

### Running the tests
- Just run the *tests* executable:
```console
./tests
```
