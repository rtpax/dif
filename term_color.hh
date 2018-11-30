
#ifndef TERM_COLOR_HH
#define TERM_COLOR_HH

#include <ostream>

struct term_color {
    enum fg_color_t {
        FG_RED      = 31,
        FG_GREEN    = 32,
        FG_BLUE     = 34,
        FG_DEFAULT  = 39,
        FG_PRESERVE = -1,
    };
    enum bg_color_t {
        BG_RED      = 41,
        BG_GREEN    = 42,
        BG_BLUE     = 44,
        BG_DEFAULT  = 49,
        BG_PRESERVE = -1,
    };
    fg_color_t fg_color = FG_PRESERVE;
    bg_color_t bg_color = BG_PRESERVE;
};

std::ostream& operator<<(std::ostream& os, const term_color& tc) {
    if(tc.fg_color != term_color::FG_PRESERVE) {
        os << "\033[" << tc.fg_color << "m";
    }
    if(tc.bg_color != term_color::BG_PRESERVE) {
        os << "\033[" << tc.bg_color << "m";
    }
    return os;
}


#endif