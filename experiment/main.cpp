#include "../src/process.cpp"
#include "../src/bloom.cpp"


int main( int argc, char* argv[] ) {

    if( argc != 2 ) {
        std::cout << "Usage: " << argv[0] << " input.csv" << std::endl;
        exit(-1);
    } 

    int eval[4][4] = { {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0} };

    // Declare Bloom Filter parameters
    int bloomFilterSize = 1000;
    double falsePositivityProbability = 0.00001;
    int chunkSize = 10;

    std::ifstream file(argv[1]);

    if ( ! file.is_open() ) {
        std::cout << "Could not open the file" << std::endl;
        exit(-1);
    }

    std::string line;
    std::string row;

    // Skip header row
    std::getline(file, line);

    int checkpointCounter = 0;

    while (std::getline(file, line)) {
        
        row = line;

        // Create a stringstream from the line
        std::stringstream ss(row);

        // Define variables for each column
        std::string function_id_one, function_id_two, syntactic_type, internal, pretty_function_one, pretty_function_two;

        // Read each field from the stringstream, considering commas as delimiters
        std::getline(ss, function_id_one, ',');
        std::getline(ss, function_id_two, ',');
        std::getline(ss, syntactic_type, ',');
        std::getline(ss, internal, ',');

        std::cout << internal << std::endl;

        int type = atoi(syntactic_type.c_str());

        std::getline(ss, pretty_function_one);

        pretty_function_one = pretty_function_one.substr(1);

        while ( std::getline(file, line) && line.substr(0, 3) !=  "\",\"" ) {
            pretty_function_one += line;
        }

        pretty_function_two = line.substr(3);

        while ( std::getline(file, line) && line.compare("\"") != 0 ) {
            pretty_function_two += line;
        }

        std::string tokenized1 = process::tokenize(pretty_function_one);
        std::string tokenized2 = process::tokenize(pretty_function_two);

        //std::cout << "tokenized1 : " << tokenized1 << std::endl;
        //std::cout << "tokenized2 : " << tokenized2 << std::endl;

        bool isClone = bloom::isClone(pretty_function_one, pretty_function_two, bloomFilterSize, falsePositivityProbability, chunkSize);
        
        // Output detected copy number variations
        std::cout << "Result is : " << isClone << std::endl;

        if (internal == "t" && isClone) {
            eval[type][0]++;
        } else if (internal == "f" && isClone) {
            eval[type][1]++;
        } else if (internal == "f" && !isClone) {
            eval[type][2]++;
        } else {
            eval[type][3]++;
        }

        checkpointCounter++;
        if (checkpointCounter % 1000 == 0) {
            std::cout << "Checkpoint " << checkpointCounter << std::endl;
            std::cout << "\tTP\tFP\tFN\tTN" << std::endl;

            std::cout << "Type 1" << std::endl;
            for (int j = 0; j < 4; j++) {
                std::cout << eval[0][j] << "\t";
            }
            std::cout << std::endl;

            std::cout << "Type 2" << std::endl;
            for (int j = 0; j < 4; j++) {
                std::cout << eval[1][j] << "\t";
            }
            std::cout << std::endl;

            std::cout << "Type 3" << std::endl;
            for (int j = 0; j < 4; j++) {
                std::cout << eval[2][j] << "\t";
            }
            std::cout << std::endl;

            std::cout << "Type 4" << std::endl;
            for (int j = 0; j < 4; j++) {
                std::cout << eval[3][j] << "\t";
            }
            std::cout << std::endl;
        }
    }

    // print eval

    file.close();

    return 0;
}
