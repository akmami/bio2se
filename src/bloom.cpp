#ifndef BLOOM_CPP
#define BLOOM_CPP


#include <iostream>
#include <vector>
#include <unordered_set>
#include <utility>
#include <functional>
#include "../bloom/bloom_filter.hpp"
#include "process.cpp"

#define MINIMUM_KMER_COUNT 15


namespace bloom {

    /**
     * Code Clone smell detection tool
     */
    std::vector<std::pair<int, std::string>> detectCC(const std::string &code, int bloomFilterSize, double falsePositivityProbability, int chunkSize) {

        std::string segment = process::tokenize(code);

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

        for (std::size_t i = 0; i < length; i += 1) {
            
            std::size_t hashValue = hasher( segment.substr(i, chunkSize) );
            
            if( bloomFilter.contains(hashValue) ) {
                flaggedSegments.insert(hashValue);
            } else {
                bloomFilter.insert(hashValue);
            }
        }

        // Second Pass
        std::vector<std::pair<int, std::string>> detectedCNVs;

        for (std::size_t i = 0; i < length; i += 1) {

            std::size_t hashValue = hasher( segment.substr(i, chunkSize) );

            if( flaggedSegments.count(hashValue) > 0 ) {
                detectedCNVs.emplace_back( i, segment.substr(i, chunkSize) );
            }
        }

        return detectedCNVs;
    };


    bool isClone(const std::string &code1, const std::string &code2, int bloomFilterSize, double falsePositivityProbability, int chunkSize) {

        // Process code
        std::string segment1 = process::tokenize(code1);
        std::string segment2 = process::tokenize(code2);

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
        std::hash<std::string> hasher;
        int length1 = segment1.length();

        for (std::size_t i = 0; i < length1; i += 1) {
            
            std::size_t hashValue = hasher( segment1.substr(i, chunkSize) );
            
            bloomFilter.insert(hashValue);
        }

        // Second Pass
        int length2 = segment2.length();
        int cloneCount = 0;

        for (std::size_t i = 0; i < length2; i += 1) {

            std::size_t hashValue = hasher( segment2.substr(i, chunkSize) );

            if( bloomFilter.contains(hashValue) )  {
                cloneCount++;
            }
        }

        std::cout << "Number of clones: " << cloneCount << std::endl;

        if ( cloneCount >= MINIMUM_KMER_COUNT ) {
            return true;
        }

        return false;
    };

};

#endif