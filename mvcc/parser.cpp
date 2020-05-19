#include "parser.h"

#include <string>
#include <vector>
#include <sstream>
#include <iostream>

std::vector<std::string> split(std::string const& trans_line) {
    std::vector<std::string> results;
    int l=0;
    for (int i = 0; i < trans_line.size(); ++i) {
        if (trans_line.compare(i, 1, " ") == 0) {
            results.push_back(trans_line.substr(l, i-l));
            l = i + 1;
        }
    }
    results.push_back(trans_line.substr(l, trans_line.size()));
    /*
    std::istringstream iss(trans_line);
    std::vector<std::string> results((std::istream_iterator<std::string>(iss)),
                                 std::istream_iterator<std::string>());
    
    */
    return results;
}