#ifndef STREAMSPLITTER_HH
#define STREAMSPLITTER_HH

#include <istream>
#include <vector>
#include <string>

namespace cmp {

std::vector<std::string> split_by_line(std::istream& file);
std::vector<std::string> split_by_token(std::istream& file);
std::string split_by_character(std::istream& file);

}


#endif