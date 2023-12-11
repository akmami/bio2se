#include <iostream>
#include <vector>
#include <unordered_set>
#include <functional>
#include "../bloom/bloom_filter.hpp"
#include "process.cpp"

/**
 * Code Clone smell detection tool
 */
std::vector<std::string> detectCNV(const std::string &segment, int bloomFilterSize, double falsePositivityProbability, int chunkSize) {

    // Declare bloom filter
    bloom_parameters parameters;

    parameters.projected_element_count = bloomFilterSize;
    parameters.false_positive_probability = falsePositivityProbability; // 1 in 10000
    parameters.random_seed = 0xA5A5A5A5;

    parameters.compute_optimal_parameters();

    //Instantiate Bloom Filter
    bloom_filter bloomFilter(parameters);

    if (!parameters) {
        std::cout << "Error - Invalid set of bloom filter parameters!" << std::endl;
        exit(1);
    }

    // First Pass
    std::unordered_set<int> flaggedSegments;
    std::hash<std::string> hasher;
    int length = segment.length();

    for (std::size_t i = 0; i < length; i += chunkSize) {
        
        std::size_t hashValue = hasher( segment.substr(i, chunkSize) );
        
        if (bloomFilter.contains(hashValue)) {
            flaggedSegments.insert(hashValue);
        } else {
            bloomFilter.insert(hashValue);
        }
    }

    // Second Pass
    std::vector<std::string> detectedCNVs;

    for (std::size_t i = 0; i < length; i += chunkSize) {

        std::size_t hashValue = hasher( segment.substr(i, chunkSize) );

        if (flaggedSegments.count(hashValue) > 0) {
            detectedCNVs.push_back( segment.substr(i, chunkSize) );
        }
    }

    return detectedCNVs;
};


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

    std::string processedJavaCode = processJavaFile(filePath);

    std::vector<std::string> detectedCNVs = detectCNV(processedJavaCode, bloomFilterSize, falsePositivityProbability, chunkSize);

    // Output detected copy number variations
    std::cout << "Detected Copy Number Variations: ";
    for (std::string cnv : detectedCNVs) {
        std::cout << cnv << " ";
    }
    std::cout << std::endl;

    return 0;
}
