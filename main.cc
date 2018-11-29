#include "dif.hh"
#include "streamsplitter.hh"
#include <iostream>
#include <fstream>
#include <sstream>

template <class T, class ... Ts>
bool matches(std::string arg, T comp, Ts ... comps);

template <class T>
bool matches(std::string arg, T comp) {
	return arg == comp;
}

template <class T, class ... Ts>
bool matches(std::string arg, T comp, Ts ... comps) {
	return arg == comp || matches(arg, comps...);
}


enum diftype {
	CHARACTER, TOKEN, LINE
};

enum difsource {
	SRCFILE, COMMANDLINE
};

int main(int argc, char ** argv) {
	//defaults
	diftype type = TOKEN;
	difsource source = SRCFILE;
	int first_arg_index = argc;
	dif_ostream_hide_line_info();
	dif_ostream_show_color();

	for(int i = 1; i < argc; ++i) {
		if(matches(argv[i], "-h", "--help")) {
			std::cout << 
			"dif file comparison tool\n\n"
			"  -c,--character     compute dif by character\n"
			"  -t,--token         compute dif by token (default)\n"
			"  -l,--line          compute dif by line\n"
			"  -s,--command-line  read contents from command line\n"
			"  -f,--file          read contents from input files (default)\n"
			"  -L,--line-info     print whether each line was modified and how\n"
			"  --no-line-info     do not print line info (default)\n"
			"  -C,--color         print with color (default)\n"
			"  --no-color         print without color\n"
			"                     forces --line-info and --line\n"
			"  -h,--help          print this help message\n"
			"\n";
			return 0;
		} else if (matches(argv[i], "-c", "--character")) {
			type = CHARACTER;
		} else if (matches(argv[i], "-t", "--token")) {
			type = TOKEN;
		} else if (matches(argv[i], "-l", "--line")) {
			type = LINE;
		} else if (matches(argv[i], "-s", "--command-line")) {
			source = COMMANDLINE;
		} else if (matches(argv[i], "-f", "--file")) {
			source = SRCFILE;
		} else if (matches(argv[i], "-L", "--line-info")) {
			dif_ostream_show_line_info();
		} else if (matches(argv[i], "--no-line-info")) {
			dif_ostream_hide_line_info();
		} else if (matches(argv[i], "-C", "--color")) {
			dif_ostream_show_color();
		} else if (matches(argv[i], "--no-color")) {
			dif_ostream_hide_color();
		} else {
			first_arg_index = i;
			break;
		}
	}

	//if color is missing these are needed to make sense of the dif
	if(!dif_ostream_has_color()) {
		dif_ostream_show_line_info();
		type = LINE;
	}

	std::istream* src1 = nullptr;
	std::istream* src2 = nullptr;

	try {
		if(argc - first_arg_index < 2) {
			std::cerr << "error: too few arguments\n";
			return 1;
		} else  {
			if(argc - first_arg_index > 2) {
				std::cout << "warning: extra arguments ignored\n";
			}
			if(source == SRCFILE) {
				src1 = new std::ifstream(argv[first_arg_index],std::ios_base::in);
				if (!*src1)
						throw std::system_error(errno, std::system_category(), 
								std::string("failed to open ")+argv[first_arg_index]);
				src2 = new std::ifstream(argv[first_arg_index + 1],std::ios_base::in);
				if (!*src2)
						throw std::system_error(errno, std::system_category(), 
								std::string("failed to open ")+argv[first_arg_index + 1]);
			} else { //COMMANDLINE
				src1 = new std::istringstream(argv[first_arg_index],std::ios_base::in);		
				src2 = new std::istringstream(argv[first_arg_index + 1],std::ios_base::in);		
			}
		}
	} catch (std::system_error& e) {
		std::cerr << "error: " << e.what() << "\n";
		return 1;
	}

	switch(type) {
	case CHARACTER: {
		auto d = calc_dif(split_by_character(*src1),split_by_character(*src2));
		std::cout << d << "\n";
		return 0;
	}
	case TOKEN: {
		auto d = calc_dif(split_by_token(*src1),split_by_token(*src2));
		std::cout << d << "\n";
		return 0;
	}
	case LINE: {
		auto d = calc_dif(split_by_line(*src1),split_by_line(*src2));
		std::cout << d << "\n";
		return 0;
	}
	}
	return 1;
}
