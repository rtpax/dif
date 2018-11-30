#include "dif_ostream.hh"
#include "dif.hh"
#include "term_color.hh"
#include <iostream>
#include <sstream>
#include <iomanip>

static bool show_line_info = false;
void dif_ostream_show_line_info() {
	show_line_info = true;
}
void dif_ostream_hide_line_info() {
	show_line_info = false;
}
bool dif_ostream_has_line_info() {
	return show_line_info;
}

enum color_option {
    FOREGROUND,
    BACKGROUND,
    NOCOLOR
}; 

static color_option show_color = FOREGROUND;
void dif_ostream_show_color() {
	show_color = FOREGROUND;
}
void dif_ostream_hide_color() {
	show_color = NOCOLOR;
}
void dif_ostream_background_color() {
    show_color = BACKGROUND;
}
bool dif_ostream_has_color() {
	return show_color != NOCOLOR;
}
bool dif_ostream_has_background_color() {
    return show_color == BACKGROUND;
}

static bool only_modified = false;
void dif_ostream_show_only_modified() {
	only_modified = true;
}
void dif_ostream_show_all() {
	only_modified = false;
}
bool dif_ostream_only_modified() {
	return only_modified;
}

static bool show_line_num = false;
void dif_ostream_show_line_num() {
	show_line_num = true;
}
void dif_ostream_hide_line_num() {
	show_line_num = false;
}
bool dif_ostream_has_line_num() {
	return show_line_num;
}


//: preserved [default]
//~ deletion  [red]
//> insertion [green]
//% modified  [blue]

template<class T>
std::ostream& operator<<(std::ostream& os, 
		const dif<T>& d) {
	term_color red;
	term_color grn;
	term_color blu;
	term_color def;

	std::string del_line, ins_line, prv_line, mod_line;
	
	if(show_line_info) {
		del_line = "~ ";
		ins_line = "> ";
		prv_line = ": ";
		mod_line = "% ";
	} else if(show_line_num) {//give space after line numbers
		del_line = " ";
		ins_line = " ";
		prv_line = " ";
		mod_line = " ";
    }

	if(show_color == FOREGROUND) {
        red.fg_color = term_color::FG_RED;
        grn.fg_color = term_color::FG_GREEN;
        blu.fg_color = term_color::FG_BLUE;
        def.fg_color = term_color::FG_DEFAULT;
	} else if(show_color == BACKGROUND) {
        red.bg_color = term_color::BG_RED;
        grn.bg_color = term_color::BG_GREEN;
        blu.bg_color = term_color::BG_BLUE;
        def.bg_color = term_color::BG_DEFAULT;
    }

	if (d.ds.empty())
		return os;
	typename dif_segment<T>::dif_segment_t line_type = dif_segment<T>::none;
	std::ostringstream line;

	int final_line_num = 0;

	auto flush_line = [&](){
		switch(line_type) {
		case dif_segment<T>::deletion:
            os << red;
			if(show_line_num)
				os << std::setw(4) << final_line_num;
			os << del_line << line.str() << def << "\n";
			break;
		case dif_segment<T>::insertion:
			++final_line_num;
			os << grn;
            if(show_line_num)
				os << std::setw(4) << final_line_num;
			os << ins_line << line.str() << def << "\n";
			break;
		case dif_segment<T>::preserved:
			++final_line_num;
			if(!only_modified) {
				os << def;
                if(show_line_num)
					os << std::setw(4) << final_line_num;
				os << prv_line << line.str() << def << "\n";
			}
			break;
		case dif_segment<T>::modified:
			++final_line_num;
            os << blu;
			if(show_line_num)
				os << std::setw(4) << final_line_num;
			os << mod_line << line.str() << def << "\n";
			break;
		default:
			os << "error: " << line.str() << def << "\n";
		}
		line.str("");
	};

	for(const dif_segment<T>& ds : d.ds) {
		std::string str;
		for(auto&& s : ds.s)
			str += s; //convert string or char to string
		str += "\n";//getline does not read empty lines after newlines
		std::istringstream iss(str);
		std::string partial_line;
		bool first_line = true;
		while(std::getline(iss, partial_line)) {
			if(line_type == dif_segment<T>::none) {
				line_type = ds.type;
			} else if (line_type != ds.type && !partial_line.empty()) {
				line_type = dif_segment<T>::modified;
			}
			if(first_line) {
				first_line = false;
			} else {
				flush_line();
				if(partial_line.empty()) {
					line_type = dif_segment<T>::none;
					continue;
				} else {
					line_type = ds.type;
				}
			}
			switch(ds.type) {
			case dif_segment<T>::deletion:
				line << red << partial_line;
				break;
			case dif_segment<T>::insertion:
				line << grn << partial_line;
				break;
			case dif_segment<T>::preserved:
			default:
				line << def << partial_line;
			}
		}
	}
	if(!line.str().empty()) {
		flush_line();
	}
	return os;
}

template std::ostream& operator<<(std::ostream& os, const dif<std::string>& d);
template std::ostream& operator<<(std::ostream& os, const dif<std::vector<std::string>>& d);
