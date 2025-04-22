# Lab Work 4: Custom Command-Line Argument Parser (C++)

This project implements a custom C++ class to parse command-line arguments, including support for string, integer, and boolean types.

The parser is designed by **reverse engineering test cases** and aims to pass all provided unit tests using GoogleTest.

---

## 🎯 Goal

Design and implement a flexible argument parser based on a provided suite of unit tests.  
The parser should:

- Support common CLI argument types (string, int, bool)
- Parse both `--key=value` and flag-style arguments
- Be extensible for future types (e.g., vectors, enums)

---

## 🔍 Example Use Case

Example usage from tests:

```cpp
ArgParser parser("My Parser");
parser.AddStringArgument("param1");

parser.Parse(SplitString("app --param1=value1"));
parser.GetStringValue("param1"); // returns "value1"
ASSERT_TRUE(parser.Parse(...));
ASSERT_EQ(parser.GetIntValue("some_arg"), 123);
```
---

## ✅ Features Implemented
- 'AddStringArgument(name)'

- 'AddIntArgument(name)'

- 'AddFlag(name) (boolean)'

- 'Parse(argv) and store values'

- 'GetStringValue(name)'

- 'GetIntValue(name)'

- 'GetFlag(name)'

- 'Optional help message ('--help' support)'

---

## 🔬 Testing

- Unit tests are located in:
- tests/argparser_test.cpp

Framework used: GoogleTest

Running tests:
```bash
cmake --build . --target argparser_tests
ctest -V
```

- You are encouraged to add your own test cases to ensure full coverage.

##  📦 Example CLI Program
- A demo application is included that performs arithmetic based on parsed arguments:
```bash
# Sum:
./labwork4 --sum 1 2 3 4 5
# → prints 15

# Multiplication:
./labwork4 --mult 1 2 3 4 5
# → prints 120
```
See 'bin/main.cpp' for example usage of the parser in a real program.

---

## 🛠 Project Structure
```py
lib/
  └── argparser.cpp     # Implementation of the parser
  └── argparser.h       # Parser class interface
tests/
  └── argparser_test.cpp # Unit tests using GoogleTest
bin/
  └── main.cpp          # Demo CLI application
```
- You are allowed to add new '.cpp' or '.h' files as needed, as long as they remain in the 'lib/' folder and are included via CMake.
---

## ⚠️ Notes

- Standard C++ features such as templates and virtual functions are allowed

- Be ready to explain your design decisions during code review

- This parser can be reused in future labs or projects

- Follow clean code practices (naming, ownership, separation of concerns)

  






