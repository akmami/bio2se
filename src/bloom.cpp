#ifndef BLOOM_CPP
#define BLOOM_CPP


#include <iostream>
#include <vector>
#include <unordered_set>
#include <utility>
#include <functional>
#include "../bloom/bloom_filter.hpp"
#include "process.cpp"



namespace bloom {

    /**
     * Code Clone smell detection tool
     */
    std::vector<std::pair<int, std::string>> detectCC(const std::string &segment, int bloomFilterSize, double falsePositivityProbability, int chunkSize) {

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
        std::vector<std::pair<int, std::string>> detectedCNVs;

        for (std::size_t i = 0; i < length; i += chunkSize) {

            std::size_t hashValue = hasher( segment.substr(i, chunkSize) );

            if (flaggedSegments.count(hashValue) > 0) {
                detectedCNVs.emplace_back( i, segment.substr(i, chunkSize) );
            }
        }

        return detectedCNVs;
    };

};

#endif