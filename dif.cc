#include "dif.hh"
#include "Color.hh"

#include <unordered_set>
#include <stdexcept>
#include <string>

#include <iostream>
#include <iomanip>
#include <sstream>

struct dif_point {
	int original_loc;
	int final_loc;

	constexpr dif_point() : original_loc(0), final_loc(0) {}
	constexpr dif_point(int o, int f) : original_loc(o), final_loc(f) {}
	bool operator==(dif_point dp) const {
		return dp.original_loc == original_loc && dp.final_loc == final_loc;
	}
	bool operator!=(dif_point dp) const {
		return dp.original_loc != original_loc || dp.final_loc != final_loc;
	}
};

std::ostream& operator<<(std::ostream& os, dif_point dp) {
	return os << "{" << dp.original_loc << "," << dp.final_loc << "}";
}

struct dif_point_hash {
	size_t operator()(dif_point df) const {
	  static std::hash<int> int_hash;
	  return int_hash((df.original_loc << 16) | df.final_loc);
	};
};

struct dif_hist {
  enum step { ins,del };
	std::vector<step> instructions;
	dif_point current;
	bool last_ins; //record if the last step was insertion
	               //if it was insertion, do not delete until
	               //something other than insertions is done
};

template<class T>
static dif<T> finalize_dif(const dif_hist& path,
		const T& original,
		const T& final) {
	dif_point dp;
	int i = 0;
	dif<T> out;
	dif_segment<T> segment;

	auto dif_append = [&](typename dif_segment<T>::dif_segment_t type, typename T::value_type c){
		if(!segment.s.empty() && segment.type != type) {
			out.ds.push_back(segment);
			segment.s.clear();
		}
		segment.type = type;
		segment.s.push_back(c);
	};

	while(dp.original_loc < original.size() ||
			dp.final_loc < final.size()) {
		if(dp.original_loc < original.size() && dp.final_loc < final.size() &&
				original[dp.original_loc] == final[dp.final_loc]) {
			dif_append(dif_segment<T>::preserved, final[dp.final_loc]);
			++dp.original_loc;
			++dp.final_loc;
		} else if(path.instructions[i] == dif_hist::ins) {
			dif_append(dif_segment<T>::insertion, final[dp.final_loc]);
			++dp.final_loc;
			++i;
		} else {//del
			dif_append(dif_segment<T>::deletion, original[dp.original_loc]);
			++dp.original_loc;
			++i;
		}
	}
	if(segment.s.size() > 0)
		out.ds.push_back(segment);
	return out;
}

static std::vector<const dif_hist*> paths_at_end(
		const std::vector<dif_hist>& paths,
		int original_size,
		int final_size) {
	std::vector<const dif_hist*> out;
	for(const dif_hist& path : paths) {
		if(path.current.original_loc == original_size &&
				path.current.final_loc == final_size)
		out.push_back(&path);
	}
	return out;
}

static bool occurs_in(
		const std::unordered_set<dif_point,dif_point_hash>& past,
		dif_point point) {
	return past.find(point) != past.cend();
}

template<class T>
static dif_hist& dif_follow_preservation(
		dif_hist& path,
		const T& original,
		const T& final) {
	while(path.current.original_loc < original.size() &&
			path.current.final_loc < final.size() &&
			original[path.current.original_loc] ==
				final[path.current.final_loc]) {
		++path.current.original_loc;
		++path.current.final_loc;
		path.last_ins = false;
	}
	return path;
}

template<class T>
static dif_hist& dif_ins(
		dif_hist& path,
		const T& original,
		const T& final) {
	path.instructions.push_back(dif_hist::ins);
	path.current.final_loc += 1;
	path.last_ins = true;
	return dif_follow_preservation(path,original,final);
}

template<class T>
static dif_hist& dif_del(
		dif_hist& path,
		const T& original,
		const T& final) {
	path.instructions.push_back(dif_hist::del);
	path.current.original_loc += 1;
	path.last_ins = false;
	return dif_follow_preservation(path,original,final);
}


template<class T>
dif<T> calc_dif(const T& original, const T& final) {
	std::vector<dif_hist> paths;
	std::vector<dif_hist> new_paths;
	std::vector<const dif_hist*> end_paths;
	{
		dif_hist tmp{{},{0,0},0};
		paths.push_back(dif_follow_preservation(tmp,original,final));
	}
	end_paths = paths_at_end(paths,original.size(),final.size());

	//since points are inserted right away, prevent further insertions
	//even at the same level. favors paths that delete first.
	std::unordered_set<dif_point,dif_point_hash> past;

	while(end_paths.empty()) {
		if(paths.size() == 0)
		  throw std::logic_error("calc_dif: all paths failed");

		for(int i = 0; i < paths.size(); ++i) {
			const dif_hist& path = paths[i];

			if(path.current.original_loc < original.size() && !path.last_ins) {
				dif_hist del_path = path;
				dif_del(del_path, original, final);

				if(!occurs_in(past, del_path.current)) {
					new_paths.push_back(del_path);
					past.insert(del_path.current);
				}
			}

			if(path.current.final_loc < final.size()) {
				dif_hist ins_path = path;
				dif_ins(ins_path, original, final);

				if(!occurs_in(past, ins_path.current)) {
					new_paths.push_back(ins_path);
					past.insert(ins_path.current);
				}
			}
		}
		paths.clear();
		paths = std::move(new_paths);
		new_paths.clear();
		end_paths = paths_at_end(paths,original.size(),final.size());
	}
	//size of end_paths should always be 1
	return finalize_dif(*end_paths.front(),original,final);
}

static bool show_line_info = false;

void dif_ostream_show_line_info() {
	show_line_info = true;
}
void dif_ostream_hide_line_info() {
	show_line_info = false;
}

static bool show_color = true;

void dif_ostream_show_color() {
	show_color = true;
}
void dif_ostream_hide_color() {
	show_color = false;
}

static bool only_modified = false;

void dif_ostream_show_only_modified() {
	only_modified = true;
}
void dif_ostream_show_all() {
	only_modified = false;
}

static bool show_line_num = false;

void dif_ostream_show_line_num() {
	show_line_num = true;
}
void dif_ostream_hide_line_num() {
	show_line_num = false;
}

bool dif_ostream_has_line_info() {
	return show_line_info;
}

bool dif_ostream_has_color() {
	return show_color;
}

bool dif_ostream_only_modified() {
	return only_modified;
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
	Color::Modifier red(Color::FG_RED);
	Color::Modifier grn(Color::FG_GREEN);
	Color::Modifier blu(Color::FG_BLUE);
	Color::Modifier def(Color::FG_DEFAULT);
	Color::Modifier no_color(Color::FG_PRESERVE);

	std::string del_line, ins_line, prv_line, mod_line;
	
	if(show_line_info) {
		del_line = "~ ";
		ins_line = "> ";
		prv_line = ": ";
		mod_line = "% ";
	}
	if(!show_color) {
		red = grn = blu = def = no_color;
	}

	if (d.ds.empty())
		return os;
	typename dif_segment<T>::dif_segment_t line_type = dif_segment<T>::none;
	std::ostringstream line;

	int original_line_num = 0;
	int final_line_num = 0;

	auto flush_line = [&](){
		switch(line_type) {
		case dif_segment<T>::deletion:
			++original_line_num;
			if(show_line_num)
				os << red << std::setw(4) << original_line_num;
			os << red << del_line << line.str() << def << "\n";
			break;
		case dif_segment<T>::insertion:
			++final_line_num;
			if(show_line_num)
				os << grn << std::setw(4) << final_line_num;
			os << grn << ins_line << line.str() << def << "\n";
			break;
		case dif_segment<T>::preserved:
			++original_line_num;
			++final_line_num;
			if(!only_modified) {
				if(show_line_num)
					os << def << std::setw(4) << final_line_num;
				os << def << prv_line << line.str() << def << "\n";
			}
			break;
		case dif_segment<T>::modified:
			++original_line_num;
			++final_line_num;
			if(show_line_num)
				os << blu << std::setw(4) << final_line_num;
			os << blu << mod_line << line.str() << def << "\n";
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

template dif<std::string> calc_dif<std::string>(const std::string&, const std::string&);
template dif<std::vector<std::string>> calc_dif<std::vector<std::string>>(const std::vector<std::string>&, const std::vector<std::string>&);
template std::ostream& operator<<(std::ostream& os, const dif<std::string>& d);
template std::ostream& operator<<(std::ostream& os, const dif<std::vector<std::string>>& d);


