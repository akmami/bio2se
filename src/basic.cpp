#ifndef PROCESS_CPP
#define PROCESS_CPP

#include <iostream>
#include <fstream>
#include <regex>


std::regex regex("\\b\\w+\\s+\\w+\\s*=\\s*.*;");


namespace process {

    std::string basic(const std::string& filePath) {
        
        std::string processed = "";

        std::ifstream reader(filePath);
        if( !reader.is_open() ) {
            std::cerr << "Error opening file: " << filePath << std::endl;
            return processed;
        }

        std::string line;
        while( std::getline(reader, line) ) {

            line.erase( std::remove_if( line.begin(), line.end(), ::isspace ), line.end() );

            if (line.find("//") == 0) {
                continue;
            }

            if (line.find("/*") == 0) {
                // Skip lines until the end of the multi-line comment
                while( line.find("*/") == std::string::npos ) {
                    std::getline(reader, line);
                }
                continue;
            }

            if (std::regex_match(line, regex)) {
                processed += line;
                continue;
            }

            // Remove spaces, tabs, and newline characters from the line
            line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());

            processed += line;
        }

        return processed;
    }

}

#endif