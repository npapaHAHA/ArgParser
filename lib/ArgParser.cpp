
#include "ArgParser.h"
#include <iostream>
#include <sstream>
#include <iterator>


namespace ArgumentParser {

ArgParser::ArgParser(const std::string& program_name) : program_name_(program_name) {}

ArgParser& ArgParser::AddStringArgument(const std::string& name, const std::string& help) {
    return AddStringArgument('\0', name, help);
}

ArgParser& ArgParser::AddStringArgument(char short_name, const std::string& name, const std::string& help) {
    Argument arg;
    arg.type = Argument::STRING;
    arg.name = name;
    arg.short_name = short_name;
    arg.help = help;
    arguments_.push_back(arg);
    auto it = std::prev(arguments_.end());
    name_to_arg_[name] = &(*it);
    if (short_name) {
        short_name_to_arg_[short_name] = &(*it);
    }
    current_arg_ = &(*it);

    return *this;
}

ArgParser& ArgParser::AddIntArgument(const std::string& name, const std::string& help) {
    return AddIntArgument('\0', name, help);
}

ArgParser& ArgParser::AddIntArgument(char short_name, const std::string& name, const std::string& help) {
    Argument arg;
    arg.type = Argument::INT;
    arg.name = name;
    arg.short_name = short_name;
    arg.help = help;
    arguments_.push_back(arg);
    auto it = std::prev(arguments_.end());
    name_to_arg_[name] = &(*it);
    if (short_name) {
        short_name_to_arg_[short_name] = &(*it);
    }
    current_arg_ = &(*it);

    return *this;
}

ArgParser& ArgParser::AddFlag(const std::string& name, const std::string& help) {
    return AddFlag('\0', name, help);
}

ArgParser& ArgParser::AddFlag(char short_name, const std::string& name, const std::string& help) {
    Argument arg;
    arg.type = Argument::FLAG;
    arg.name = name;
    arg.short_name = short_name;
    arg.help = help;
    arguments_.push_back(arg);
    auto it = std::prev(arguments_.end());
    name_to_arg_[name] = &(*it);
    if (short_name) {
        short_name_to_arg_[short_name] = &(*it);
    }
    current_arg_ = &(*it);

    return *this;
}

ArgParser& ArgParser::AddHelp(char short_name, const std::string& name, const std::string& description) {
    help_description_ = description;
    Argument arg;
    arg.type = Argument::FLAG;
    arg.name = name;
    arg.short_name = short_name;
    arg.help = "Display this help and exit";
    arguments_.push_back(arg);
    auto it = std::prev(arguments_.end());
    name_to_arg_[name] = &(*it);
    if (short_name) {
        short_name_to_arg_[short_name] = &(*it);
    }
    current_arg_ = &(*it);

    return *this;
}

// Модификаторы
ArgParser& ArgParser::Default(const std::string& value) {
    if (current_arg_ && current_arg_->type == Argument::STRING) {
        current_arg_->has_default = true;
        current_arg_->default_string_value = value;
    }
    return *this;
}

ArgParser& ArgParser::Default(int value) {
    if (current_arg_ && current_arg_->type == Argument::INT) {
        current_arg_->has_default = true;
        current_arg_->default_int_value = value;
    }
    return *this;
}

ArgParser& ArgParser::Default(bool value) {
    if (current_arg_ && current_arg_->type == Argument::FLAG) {
        current_arg_->has_default = true;
        current_arg_->default_bool_value = value;
        current_arg_->bool_value = value;
        if (current_arg_->store_bool) {
            *(current_arg_->store_bool) = value;
        }
    }
    return *this;
}

ArgParser& ArgParser::MultiValue(size_t min_count) {
    if (current_arg_) {
        current_arg_->is_multi_value = true;
        current_arg_->min_count = min_count;
    }
    return *this;
}

ArgParser& ArgParser::Positional() {
    if (current_arg_) {
        current_arg_->is_positional = true;
        positional_args_.push_back(current_arg_);
    }
    return *this;
}

ArgParser& ArgParser::Required() {
    if (current_arg_) {
        current_arg_->required = true;
    }
    return *this;
}

ArgParser& ArgParser::StoreValue(std::string& value) {
    if (current_arg_ && current_arg_->type == Argument::STRING) {
        current_arg_->store_string = &value;
        value = current_arg_->has_default ? current_arg_->default_string_value : "";
    }
    return *this;
}

ArgParser& ArgParser::StoreValue(int& value) {
    if (current_arg_ && current_arg_->type == Argument::INT) {
        current_arg_->store_int = &value;
        value = current_arg_->has_default ? current_arg_->default_int_value : 0;
    }
    return *this;
}

ArgParser& ArgParser::StoreValue(bool& value) {
    if (current_arg_ && current_arg_->type == Argument::FLAG) {
        current_arg_->store_bool = &value;
        value = current_arg_->bool_value;
    }
    return *this;
}

ArgParser& ArgParser::StoreValues(std::vector<std::string>& values) {
    if (current_arg_ && current_arg_->type == Argument::STRING) {
        current_arg_->store_string_vector = &values;
    }
    return *this;
}

ArgParser& ArgParser::StoreValues(std::vector<int>& values) {
    if (current_arg_ && current_arg_->type == Argument::INT) {
        current_arg_->store_int_vector = &values;
    }
    return *this;
}

void ArgParser::ResetParserState() {
    help_ = false;
    for (Argument& arg : arguments_) {
        arg.value_provided = false;
        if (arg.type == Argument::FLAG) {
            arg.bool_value = arg.has_default ? arg.default_bool_value : false;
            if (arg.store_bool) {
                *(arg.store_bool) = arg.bool_value;
            }
        } else if (arg.type == Argument::STRING) {
            arg.string_values.clear();
            if (arg.store_string) {
                *(arg.store_string) = "";
            }
            if (arg.store_string_vector) {
                arg.store_string_vector->clear();
            }
        } else if (arg.type == Argument::INT) {
            arg.int_values.clear();
            if (arg.store_int) {
                *(arg.store_int) = 0;
            }
            if (arg.store_int_vector) {
                arg.store_int_vector->clear();
            }
        }
    }
}

bool ArgParser::Parse(const std::vector<std::string>& args) {
    ResetParserState();
    size_t positional_index = 0;
    size_t i = 1;


    for (const auto& pair : name_to_arg_) {
    }
    std::cout << std::endl;

    while (i < args.size()) {
        const std::string& arg = args[i];

        if (arg[0] == '-') {
            if (arg.size() == 1) {
                return false;
            }
            if (arg[1] == '-') {
                if (arg.size() == 2) {
                    ++i;
                    break;
                }
                size_t eq_pos = arg.find('=');
                std::string name, value;
                if (eq_pos != std::string::npos) {
                    name = arg.substr(2, eq_pos - 2);
                    value = arg.substr(eq_pos + 1);
                } else {
                    name = arg.substr(2);
                    value = "";
                }


                auto it = name_to_arg_.find(name);
                if (it != name_to_arg_.end()) {
                    Argument* arg_ptr = it->second;
                    if (arg_ptr->type == Argument::FLAG) {
                        if (!value.empty()) {
                            return false;
                        }
                        arg_ptr->bool_value = true;
                        arg_ptr->value_provided = true;
                        if (arg_ptr->store_bool) {
                            *(arg_ptr->store_bool) = true;
                        }
                        if (name == "help") {
                            help_ = true;
                        }
                    } else {
                        if (value.empty()) {
                            if (i + 1 < args.size()) {
                                value = args[++i];
                            } else {
                                return false;
                            }
                        }
                        arg_ptr->value_provided = true;
                        if (arg_ptr->type == Argument::STRING) {
                            arg_ptr->string_values.push_back(value);
                            if (arg_ptr->store_string) {
                                *(arg_ptr->store_string) = value;
                            }
                            if (arg_ptr->store_string_vector) {
                                arg_ptr->store_string_vector->push_back(value);
                            }
                        } else if (arg_ptr->type == Argument::INT) {
                            try {
                                int int_value = std::stoi(value);
                                arg_ptr->int_values.push_back(int_value);
                                if (arg_ptr->store_int) {
                                    *(arg_ptr->store_int) = int_value;
                                }
                                if (arg_ptr->store_int_vector) {
                                    arg_ptr->store_int_vector->push_back(int_value);
                                }
                            } catch (...) {
                                return false;
                            }
                        }
                    }
                } else {
                    return false;
                }
            } else {
                size_t j = 1;
                while (j < arg.size()) {
                    char ch = arg[j];
                    auto it = short_name_to_arg_.find(ch);
                    if (it != short_name_to_arg_.end()) {
                        Argument* arg_ptr = it->second;
                        if (arg_ptr->type == Argument::FLAG) {
                            arg_ptr->bool_value = true;
                            arg_ptr->value_provided = true;
                            if (arg_ptr->store_bool) {
                                *(arg_ptr->store_bool) = true;
                            }
                            if (arg_ptr->name == "help") {
                                help_ = true;
                            }
                            ++j;
                        } else {
                            std::string value;
                            if (j + 1 < arg.size() && arg[j + 1] == '=') {
                                value = arg.substr(j + 2);
                                j = arg.size();
                            } else if (j + 1 < arg.size()) {
                                value = arg.substr(j + 1);
                                j = arg.size();
                            } else {
                                if (i + 1 < args.size()) {
                                    value = args[++i];
                                } else {
                                    return false;
                                }
                            }
                            arg_ptr->value_provided = true;
                            if (arg_ptr->type == Argument::STRING) {
                                arg_ptr->string_values.push_back(value);
                                if (arg_ptr->store_string) {
                                    *(arg_ptr->store_string) = value;
                                }
                                if (arg_ptr->store_string_vector) {
                                    arg_ptr->store_string_vector->push_back(value);
                                }
                            } else if (arg_ptr->type == Argument::INT) {
                                try {
                                    int int_value = std::stoi(value);
                                    arg_ptr->int_values.push_back(int_value);
                                    if (arg_ptr->store_int) {
                                        *(arg_ptr->store_int) = int_value;
                                    }
                                    if (arg_ptr->store_int_vector) {
                                        arg_ptr->store_int_vector->push_back(int_value);
                                    }
                                } catch (...) {
                                    return false;
                                }
                            }
                            break;
                        }
                    } else {
                        return false;
                    }
                }
            }
        } else {
            if (positional_index < positional_args_.size()) {
                Argument* arg_ptr = positional_args_[positional_index];
                if (arg_ptr->type == Argument::STRING) {
                    arg_ptr->string_values.push_back(arg);
                    arg_ptr->value_provided = true;
                    if (arg_ptr->store_string_vector) {
                        arg_ptr->store_string_vector->push_back(arg);
                    }
                } else if (arg_ptr->type == Argument::INT) {
                    try {
                        int int_value = std::stoi(arg);
                        arg_ptr->int_values.push_back(int_value);
                        arg_ptr->value_provided = true;
                        if (arg_ptr->store_int_vector) {
                            arg_ptr->store_int_vector->push_back(int_value);
                        }
                    } catch (...) {
                        return false;
                    }
                }
                if (!arg_ptr->is_multi_value) {
                    ++positional_index;
                } else {
                    if (positional_index == positional_args_.size() - 1) {
                    } else {
                        size_t count = arg_ptr->type == Argument::STRING ? arg_ptr->string_values.size() : arg_ptr->int_values.size();
                        if (count >= arg_ptr->min_count) {
                            ++positional_index;
                        }
                    }
                }
            } else {
                return false;
            }
        }
        ++i;
    }

    while (i < args.size()) {
        const std::string& arg = args[i];
        if (positional_index < positional_args_.size()) {
            Argument* arg_ptr = positional_args_[positional_index];
            if (arg_ptr->type == Argument::STRING) {
                arg_ptr->string_values.push_back(arg);
                arg_ptr->value_provided = true;
                if (arg_ptr->store_string_vector) {
                    arg_ptr->store_string_vector->push_back(arg);
                }
            } else if (arg_ptr->type == Argument::INT) {
                try {
                    int int_value = std::stoi(arg);
                    arg_ptr->int_values.push_back(int_value);
                    arg_ptr->value_provided = true;
                    if (arg_ptr->store_int_vector) {
                        arg_ptr->store_int_vector->push_back(int_value);
                    }
                } catch (...) {
                    return false;
                }
            }
            if (!arg_ptr->is_multi_value) {
                ++positional_index;
            }
        } else {
            return false;
        }
        ++i;
    }

    if (help_) {
        return true;
    }

    for (const Argument& arg : arguments_) {
        if (arg.type == Argument::STRING) {
            for (const auto& val : arg.string_values) {
            }
        } else if (arg.type == Argument::INT) {
            for (const auto& val : arg.int_values) {
            }
        } else if (arg.type == Argument::FLAG) {
        }
    }

    for (Argument& arg : arguments_) {
        if (!arg.value_provided) {
            if (arg.has_default) {
                arg.value_provided = true;
                if (arg.type == Argument::STRING) {
                    arg.string_values.push_back(arg.default_string_value);
                    if (arg.store_string) {
                        *(arg.store_string) = arg.default_string_value;
                    }
                } else if (arg.type == Argument::INT) {
                    arg.int_values.push_back(arg.default_int_value);
                    if (arg.store_int) {
                        *(arg.store_int) = arg.default_int_value;
                    }
                } else if (arg.type == Argument::FLAG) {
                    arg.bool_value = arg.default_bool_value;
                    if (arg.store_bool) {
                        *(arg.store_bool) = arg.default_bool_value;
                    }
                }
            } else if (arg.is_multi_value && arg.min_count == 0) {
            } else if (arg.required) {
                return false;
            }
        } else {
            size_t count = arg.type == Argument::STRING ? arg.string_values.size() : arg.int_values.size();
            if (arg.is_multi_value && arg.min_count > count) {
                return false;
            }
        }
    }

    return true;
}

bool ArgParser::Parse(int argc, char** argv) {
    std::vector<std::string> args(argv, argv + argc);
    return Parse(args);
}


std::string ArgParser::GetStringValue(const std::string& name) {
    return GetStringValue(name, 0);
}

std::string ArgParser::GetStringValue(const std::string& name, size_t index) {
    auto it = name_to_arg_.find(name);
    if (it != name_to_arg_.end()) {
        Argument* arg_ptr = it->second;
        if (index < arg_ptr->string_values.size()) {
            std::cout << "Returning value: '" << arg_ptr->string_values[index] << "'" << std::endl;
            return arg_ptr->string_values[index];
        } else {
        }
    } else {
    }
    return "";
}

int ArgParser::GetIntValue(const std::string& name) {
    return GetIntValue(name, 0);
}

int ArgParser::GetIntValue(const std::string& name, size_t index) {
    auto it = name_to_arg_.find(name);
    if (it != name_to_arg_.end()) {
        Argument* arg_ptr = it->second;
        if (index < arg_ptr->int_values.size()) {
            return arg_ptr->int_values[index];
        } else {
        }
    } else {
    }
    return 0;
}

bool ArgParser::GetFlag(const std::string& name) {
    auto it = name_to_arg_.find(name);
    if (it != name_to_arg_.end()) {
        Argument* arg_ptr = it->second;
        return arg_ptr->bool_value;
    }
    return false;
}

bool ArgParser::Help() const {
    return help_;
}

std::string ArgParser::HelpDescription() const {
    std::ostringstream oss;
    oss << program_name_ << "\n";
    if (!help_description_.empty()) {
        oss << help_description_ << "\n";
    }
    oss << "\n";
    for (const Argument& arg : arguments_) {
        oss << "  ";
        if (arg.short_name) {
            oss << "-" << arg.short_name << ", ";
        } else {
            oss << "    ";
        }
        oss << "--" << arg.name;
        if (arg.type != Argument::FLAG) {
            oss << "=<";
            if (arg.type == Argument::STRING) {
                oss << "string";
            } else if (arg.type == Argument::INT) {
                oss << "int";
            }
            oss << ">";
        }
        oss << ", " << arg.help;
        if (arg.is_multi_value) {
            oss << " [repeated";
            if (arg.min_count > 0) {
                oss << ", min args = " << arg.min_count;
            }
            oss << "]";
        }
        if (arg.has_default) {
            oss << " [default = ";
            if (arg.type == Argument::STRING) {
                oss << arg.default_string_value;
            } else if (arg.type == Argument::INT) {
                oss << arg.default_int_value;
            } else if (arg.type == Argument::FLAG) {
                oss << (arg.default_bool_value ? "true" : "false");
            }
            oss << "]";
        }
        oss << "\n";
    }
    return oss.str();
}

}
