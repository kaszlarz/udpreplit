#include "data_parser.h"
#include <sstream>
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <iostream>

DataParser::DataParser() {
}

std::vector<double> DataParser::parseData(const std::string& data) {
    std::vector<double> values;
    
    if (data.empty()) {
        return values;
    }
    
    // Split the data by common delimiters (space, newline, comma, tab)
    std::vector<std::string> tokens = split(data, " \n\r\t,;");
    
    for (const std::string& token : tokens) {
        std::string trimmed = trim(token);
        
        if (!trimmed.empty() && isValidNumber(trimmed)) {
            try {
                double value = std::stod(trimmed);
                values.push_back(value);
            } catch (const std::exception& e) {
                std::cerr << "Warning: Failed to parse number '" << trimmed << "': " << e.what() << std::endl;
            }
        } else if (!trimmed.empty()) {
            std::cerr << "Warning: Ignoring invalid number '" << trimmed << "'" << std::endl;
        }
    }
    
    return values;
}

bool DataParser::isValidNumber(const std::string& str) const {
    if (str.empty()) {
        return false;
    }
    
    size_t start = 0;
    
    // Skip leading sign
    if (str[0] == '+' || str[0] == '-') {
        start = 1;
        if (str.length() == 1) {
            return false;
        }
    }
    
    bool has_digit = false;
    bool has_decimal = false;
    bool has_exponent = false;
    
    for (size_t i = start; i < str.length(); ++i) {
        char c = str[i];
        
        if (std::isdigit(c)) {
            has_digit = true;
        } else if (c == '.' && !has_decimal && !has_exponent) {
            has_decimal = true;
        } else if ((c == 'e' || c == 'E') && !has_exponent && has_digit) {
            has_exponent = true;
            // Check for exponent sign
            if (i + 1 < str.length() && (str[i + 1] == '+' || str[i + 1] == '-')) {
                ++i; // Skip the sign
            }
        } else {
            return false;
        }
    }
    
    return has_digit;
}

std::string DataParser::trim(const std::string& str) const {
    size_t start = 0;
    size_t end = str.length();
    
    // Find first non-whitespace character
    while (start < end && std::isspace(str[start])) {
        ++start;
    }
    
    // Find last non-whitespace character
    while (end > start && std::isspace(str[end - 1])) {
        --end;
    }
    
    return str.substr(start, end - start);
}

std::vector<std::string> DataParser::split(const std::string& str, const std::string& delimiters) const {
    std::vector<std::string> tokens;
    size_t start = 0;
    size_t end = 0;
    
    while ((end = str.find_first_of(delimiters, start)) != std::string::npos) {
        if (end != start) {
            tokens.push_back(str.substr(start, end - start));
        }
        start = end + 1;
    }
    
    // Add the last token if there's remaining content
    if (start < str.length()) {
        tokens.push_back(str.substr(start));
    }
    
    return tokens;
}
