/*
 * from https://stackoverflow.com/questions/2616906/how-do-i-output-coloured-text-to-a-linux-terminal
 * by user Joel Sjögren https://stackoverflow.com/users/939402/joel-sj%C3%B6gren
 * 
 * some modifications made
 */

#include <ostream>
namespace Color {
    enum Code {
        FG_RED      = 31,
        FG_GREEN    = 32,
        FG_BLUE     = 34,
        FG_DEFAULT  = 39,
        FG_PRESERVE = -1,
        BG_RED      = 41,
        BG_GREEN    = 42,
        BG_BLUE     = 44,
        BG_DEFAULT  = 49,
        BG_PRESERVE = -1,
    };
    class Modifier {
        Code code;
    public:
        Modifier(Code pCode) : code(pCode) {}
        friend std::ostream&
        operator<<(std::ostream& os, const Modifier& mod) {
            if(mod.code == FG_PRESERVE || mod.code == BG_PRESERVE)
                return os;
            return os << "\033[" << mod.code << "m";
        }
    };
}
