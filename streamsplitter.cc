#include "streamsplitter.hh"

namespace cmp {

enum chartype {
    word,white,other
};

bool is_word_char(char c) {
    return (c >= 'a' && c <= 'z') || 
            (c >= 'A' && c <= 'Z') || 
            (c >= '0' && c <= '9') ||
            c == '_' || c == '$'; 
}

bool is_whitespace(char c) {
    return (c == ' ' || c == '\r' || c == '\n' || c == '\t');
}

chartype get_chartype(char c) {
    if(is_word_char(c))
        return word;
    else if(is_whitespace(c))
        return white;
    else
        return other;
}

std::vector<std::string> split_by_line(std::istream& file) {
    std::vector<std::string> out;

    std::string line;

    while(std::getline(file, line)) {
        out.push_back(line + "\n");
        //while \n this may not be the line ending used
        //it will display the same
    }
    return out;
}

std::vector<std::string> split_by_token(std::istream& file) {
    std::vector<std::string> out;

    std::string line;
    std::string buffer;
    chartype lasttype = other;

    while(std::getline(file, line)) {
        line += "\n";
        for(char c : line) {
            chartype type = get_chartype(c);
            if((type != lasttype || type == white) && !buffer.empty()) {
                out.push_back(buffer);
                buffer.clear();
            }
            lasttype = type;
            buffer.push_back(c);
        }
    }
    if(!buffer.empty())
        out.push_back(buffer);
    return out;
}

std::string split_by_character(std::istream& file) {
    std::string out;
    out.assign(std::istreambuf_iterator<char>(file),
            std::istreambuf_iterator<char>());
    return out;
}


}