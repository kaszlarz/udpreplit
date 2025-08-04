#ifndef DATA_PARSER_H
#define DATA_PARSER_H

#include <vector>
#include <string>

class DataParser {
public:
    DataParser();
    
    // Parse incoming data string and extract numeric values
    std::vector<double> parseData(const std::string& data);
    
    // Validate if a string represents a valid number
    bool isValidNumber(const std::string& str) const;
    
private:
    // Helper function to trim whitespace
    std::string trim(const std::string& str) const;
    
    // Split string by delimiters
    std::vector<std::string> split(const std::string& str, const std::string& delimiters) const;
};

#endif // DATA_PARSER_H
