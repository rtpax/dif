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

std::vector<std::string> get_args(int argc, char**argv) {
	std::vector<std::string> out;
	bool no_switch = false;
	for(int i = 1; i < argc; ++i) {
		std::string str = argv[i];
		if (str == "--")
			no_switch = true;
		if(str.size() >= 2 && str[0] == '-') {
			if(str[1] == '-' || no_switch) {
				out.push_back(str);
			} else {
				for(int j = 1; j < str.size(); ++j) {
					out.push_back(std::string("-") + str[j]);
				}
			}
		} else {
			out.push_back(str);
		}
	}
	return out;
}

int main(int argc, char ** argv) {
	//defaults
	diftype type = TOKEN;
	difsource source = SRCFILE;
	dif_ostream_hide_line_info();
	dif_ostream_show_color();
	dif_ostream_show_all();
	dif_ostream_hide_line_num();

	std::vector<std::string> args = get_args(argc,argv);
	int first_arg_index = args.size();

	for(int i = 0; i < args.size(); ++i) {
		if(matches(args[i], "-h", "--help")) {
			std::cout << 
			"dif file comparison tool\n\n"
			"  -c,--character        compute dif by character\n"
			"  -t,--token            compute dif by token (default)\n"
			"  -l,--line             compute dif by line\n"
			"  -s,--command-line     read contents from command line\n"
			"  -f,--file             read contents from input files (default)\n"
			"  -m,--only-modified    only show lines that were modified\n"
			"  --no-only-modified    show all lines (default)\n"
			"  -n,--line-num         show line numbers\n"
			"  --no-line-num         do not show line numbers (default)\n"
			"  -L,--line-info        print whether each line was modified and how\n"
			"  --no-line-info        do not print line info (default)\n"
			"  -C,--color,--fgcolor  print with foreground color (default)\n"
			"  --bg-color            print with background color. helpful for whitespace\n"
			"  --no-color            print without color. forces --line-info.\n"
			"                        use with --line is recommended.\n"
			"  --                    do not interpret further arguments as switches\n"
			"  -h,--help             print this help message\n"
			"\n";
			return 0;
		} else if (matches(args[i], "-c", "--character")) {
			type = CHARACTER;
		} else if (matches(args[i], "-t", "--token")) {
			type = TOKEN;
		} else if (matches(args[i], "-l", "--line")) {
			type = LINE;
		} else if (matches(args[i], "-s", "--command-line")) {
			source = COMMANDLINE;
		} else if (matches(args[i], "-f", "--file")) {
			source = SRCFILE;
		} else if (matches(args[i], "-L", "--line-info")) {
			dif_ostream_show_line_info();
		} else if (matches(args[i], "--no-line-info")) {
			dif_ostream_hide_line_info();
		} else if (matches(args[i], "-C", "--color", "--fg-color")) {
			dif_ostream_show_color();
		} else if (matches(args[i], "--no-color")) {
			dif_ostream_hide_color();
		} else if (matches(args[i], "--bg-color")) {
			dif_ostream_background_color();
		} else if (matches(args[i], "-m", "--only-modified")) {
			dif_ostream_show_only_modified();
		} else if (matches(args[i], "--no-only-modified")) {
			dif_ostream_show_all();
		} else if (matches(args[i], "-n", "--line-num")) {
			dif_ostream_show_line_num();
		} else if (matches(args[i], "--no-line-num")) {
			dif_ostream_hide_line_num();
		} else if (matches(args[i], "--")) {
			first_arg_index = i + 1;
		} else if (args[i].size() > 0 && args[i][0] == '-'){
			std::cerr << "warning: ignoring unrecognized option " << args[i] << "\n";
		} else {
			first_arg_index = i;
			break;
		}
	}

	//if color is missing this is needed to make sense of the dif
	if(!dif_ostream_has_color()) {
		dif_ostream_show_line_info();
	}

	std::istream* src1 = nullptr;
	std::istream* src2 = nullptr;

	try {
		if(args.size() - first_arg_index < 2) {
			std::cerr << "error: too few arguments\n";
			return 1;
		} else  {
			if(args.size() - first_arg_index > 2) {
				std::cerr << "warning: extra arguments ignored\n";
			}
			if(source == SRCFILE) {
				src1 = new std::ifstream(args[first_arg_index],std::ios_base::in);
				if (!*src1)
						throw std::system_error(errno, std::system_category(), 
								std::string("failed to open ")+args[first_arg_index]);
				src2 = new std::ifstream(args[first_arg_index + 1],std::ios_base::in);
				if (!*src2)
						throw std::system_error(errno, std::system_category(), 
								std::string("failed to open ")+args[first_arg_index + 1]);
			} else { //COMMANDLINE
				src1 = new std::istringstream(args[first_arg_index],std::ios_base::in);		
				src2 = new std::istringstream(args[first_arg_index + 1],std::ios_base::in);		
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
