#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <ctime> 
#include <vector>

using namespace std;

class position_loader {
    public: 

    // returns a vector of first 100 positions in our dataset in FEN format
    vector<string> load_position(const string& filename) {
        vector<string> positions;
        ifstream file(filename);
        if (file.is_open()) {
            string line, header;
            getline(file, header);
            uint32_t i = 0; 
            while (getline(file, line) && i < 100) {
                stringstream ss(line);
                string first_column, fen_line;
                // puts that subset of the string separated by a comma into that variable -- reason for using stringstream 
                getline(ss, first_column, ',');
                getline(ss, fen_line, ',');
                positions.push_back(fen_line);
                i++;
            }
        }
        else {
            cerr << "Error, we could not open the file: " << filename << endl;
        }
        return positions;
    }

    string get_random_position(const vector<string>& positions) {
        if (positions.empty()) {
            cerr << "Warning: no positions are currently loaded.";
            return "";
        }
        else {
            // be sure to include this line at start of program: srand(time(0));
            uint32_t index = rand() % positions.size();
            return positions[index];
        }
    }

    uint32_t get_position_count(const vector<string>& positions) {
        if (positions.empty()) {
            cerr << "Warning: no positions are currently loaded.";
            return 0;
        }
        uint32_t total_count = positions.size();
        return total_count;
    }
};