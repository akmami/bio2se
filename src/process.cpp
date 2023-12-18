#ifndef PROCESS_CPP
#define PROCESS_CPP

#include <iostream>
#include <fstream>
#include <regex>

namespace process {

    std::string basic(const std::string& code) {

        std::regex doubleQuotaPattern("\"\"");

        std:: string result = "";

        // Strings use double quote in benchmark dataset
        result = std::regex_replace(code, doubleQuotaPattern, "\"");

        // Remove extra spaces, tabs, and new lines
        result = std::regex_replace(result, std::regex("[\\s\\t\\n]+"), " ");

        // Remove extra spaces, tabs, and new lines at the end
        result = std::regex_replace(result, std::regex("[ \\t]*$", std::regex_constants::multiline), "");

        return result;
    }

    std::string tokenize(const std::string& code) {

        // List of Java keywords
        const std::string keywords[] = {
            "abstract", "assert", "boolean", "break", "byte", "case", "catch", "char", "class", "continue",
            "default", "do", "double", "else", "enum", "extends", "final", "finally", "float", "for", "if",
            "implements", "import", "instanceof", "int", "interface", "long", "native", "new", "null",
            "package", "private", "protected", "public", "return", "short", "static", "strictfp", "super",
            "switch", "synchronized", "this", "throw", "throws", "transient", "try", "void", "volatile", "while"
        };

        // Regular expression for matching Java keywords
        std::string keywordPatternString = "\\b(";
        for (const std::string keyword : keywords) {
            keywordPatternString += keyword + "|";
        }
        keywordPatternString.pop_back();  // Remove the last '|'
        keywordPatternString += ")\\b";

        // Regular expressions for matching types, variables, integers, floats, and strings
        std::regex doubleQuotaPattern("\"\"");
        std::regex keywordPattern(keywordPatternString);
        std::regex booleanPattern("\\b(true|false)\\b");
        std::regex variablePattern("\\b([a-z_][a-zA-Z0-9_]*)\\b");
        std::regex functionPattern("\\b[a-zA-Z_][a-zA-Z0-9_]*\\s*\\(");
        std::regex stringPattern("\"[^\"]*\"");
        std::regex charPattern("'[^']+'");
        std::regex intPattern("[0-9]+");
        std::regex floatPattern("[0-9]+.[0-9]+");
        

        std:: string result = "";

        // Strings use double quote in benchmark dataset
        result = std::regex_replace(code, doubleQuotaPattern, "\"");
        result = std::regex_replace(result, keywordPattern, "K");
        result = std::regex_replace(result, booleanPattern, "B");
        result = std::regex_replace(result, stringPattern, "s");
        result = std::regex_replace(result, charPattern, "c");
        result = std::regex_replace(result, intPattern, "i");
        result = std::regex_replace(result, floatPattern, "f");
        result = std::regex_replace(result, functionPattern, "F ( ");
        result = std::regex_replace(result, variablePattern, "V");
        result = std::regex_replace(result, std::regex("\\s{2,}"), " ");

        // Remove extra spaces, tabs, and new lines at the end
        result = std::regex_replace(result, std::regex("[ \\t]*$", std::regex_constants::multiline), "");

        return result;
    }
}

#endif