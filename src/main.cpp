#include "../bloom/bloom_filter.hpp"
#include "tokenize.cpp"
#include "basic.cpp"

#include "bloom.cpp"

int main( int argc, char* argv[] ) {

    if( argc != 2 ) {
        std::cout << "Usage: " << argv[0] << " input.java" << std::endl;
        exit(-1);
    } 

    // Declare Bloom Filter parameters
    int bloomFilterSize = 1000;
    double falsePositivityProbability = 0.00001;
    int chunkSize = 30;

    std::string filePath(argv[1]);

    std::string processedJavaCode = process::basic(filePath);

    std::vector<std::pair<int, std::string>> detectedCCs = bloom::detectCC(processedJavaCode, bloomFilterSize, falsePositivityProbability, chunkSize);

    // Output detected copy number variations
    std::cout << "Detected Copy Number Variations: ";
    for (std::string cnv : detectedCCs) {
        std::cout << cnv << " ";
    }
    std::cout << std::endl;

    std::vector<std::string> preprocessedCode = process::tokenize(filePath);

    // Output detected copy number variations
    std::cout << "Detected Copy Number Variations: ";
    for (std::string token : preprocessedCode) {
        std::cout << token << " ";
    }
    std::cout << std::endl;

    return 0;
}
