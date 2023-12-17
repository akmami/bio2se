#ifndef PREPROCESS_CPP
#define PREPROCESS_CPP

#include <iostream>
#include <string>
#include <vector>

#include "clang/Basic/FileManager.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Lex/Lexer.h"
#include "clang/Lex/Token.h"
#include "clang/Tooling/Tooling.h"

using namespace clang;
using namespace clang::tooling;


namespace process {

    std::vector<std::string> tokenize(const std::string& filePath) {
        
        std::vector<std::string> tokens;

        std::string processed = "";

        std::ifstream reader(filePath);
        if( !reader.is_open() ) {
            std::cerr << "Error opening file: " << filePath << std::endl;
            return tokens;
        }

        std::string line;

        while( std::getline(reader, line) ) {
            processed += line;
        }

        // Create a new Clang tool
        ClangTool tool(getCompilations(), llvm::ArrayRef<std::string>());

        // Tokenize the code
        tool.mapVirtualFile("input.cpp", processed);
        tool.run(newFrontendActionFactory<clang::SyntaxOnlyAction>().get());
        
        // Access the token stream
        SourceManager &sourceManager = tool.getFiles().getSourceManager();
        FileID fileID = sourceManager.getMainFileID();
        const LangOptions langOptions;
        const Token *tokensBegin = Lexer::makeFileCharRange(
            CharSourceRange::getCharRange(sourceManager.getLocForStartOfFile(fileID), 
            sourceManager.getLocForEndOfFile(fileID)), langOptions, sourceManager, getPreprocessor(), false).first;

        // Extract token strings
        for (const Token *tok = tokensBegin; tok->isNot(tok::eof); tok = tok->getLoc().getLocWithOffset(1)) {
            tokens.push_back(tok->getName());
        }

        return tokens;
    };

};

#endif