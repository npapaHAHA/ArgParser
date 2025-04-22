
#pragma once

#include <string>
#include <vector>
#include <map>
#include <list>

namespace ArgumentParser {

class ArgParser {
public:
    ArgParser(const std::string& program_name);

    // Add argument methods
    ArgParser& AddStringArgument(const std::string& name, const std::string& help = "");
    ArgParser& AddStringArgument(char short_name, const std::string& name, const std::string& help = "");

    ArgParser& AddIntArgument(const std::string& name, const std::string& help = "");
    ArgParser& AddIntArgument(char short_name, const std::string& name, const std::string& help = "");

    ArgParser& AddFlag(const std::string& name, const std::string& help = "");
    ArgParser& AddFlag(char short_name, const std::string& name, const std::string& help = "");

    ArgParser& AddHelp(char short_name, const std::string& name, const std::string& description);

    // Modifiers
    ArgParser& Default(const std::string& value);
    ArgParser& Default(int value);
    ArgParser& Default(bool value);

    ArgParser& MultiValue(size_t min_count = 0);
    ArgParser& Positional();
    ArgParser& Required();
    ArgParser& StoreValue(std::string& value);
    ArgParser& StoreValue(int& value);
    ArgParser& StoreValue(bool& value);
    ArgParser& StoreValues(std::vector<std::string>& values);
    ArgParser& StoreValues(std::vector<int>& values);

    // Parsing methods
    bool Parse(int argc, char** argv);
    bool Parse(const std::vector<std::string>& args);

    // Getters
    std::string GetStringValue(const std::string& name);
    std::string GetStringValue(const std::string& name, size_t index);
    int GetIntValue(const std::string& name);
    int GetIntValue(const std::string& name, size_t index);
    bool GetFlag(const std::string& name);

    std::string HelpDescription() const;
    bool Help() const;

private:
    // Internal methods
    void ResetParserState();

    struct Argument {
        enum Type { STRING, INT, FLAG } type;
        std::string name;
        char short_name = '\0';
        std::string help;
        bool is_positional = false;
        bool is_multi_value = false;
        size_t min_count = 0;
        bool has_default = false;
        bool required = false;
        std::string default_string_value;
        int default_int_value = 0;
        bool default_bool_value = false;
        bool value_provided = false;
        std::vector<std::string> string_values;
        std::vector<int> int_values;
        bool bool_value = false;
        bool* store_bool = nullptr;
        std::string* store_string = nullptr;
        int* store_int = nullptr;
        std::vector<std::string>* store_string_vector = nullptr;
        std::vector<int>* store_int_vector = nullptr;
    };

    std::string program_name_;
    std::string help_description_;
    bool help_ = false;
    std::list<Argument> arguments_;
    std::map<std::string, Argument*> name_to_arg_;
    std::map<char, Argument*> short_name_to_arg_;
    std::vector<Argument*> positional_args_;
    Argument* current_arg_ = nullptr;
};

}