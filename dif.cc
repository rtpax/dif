#include "dif.hh"
#include "Color.hh"

#include <unordered_set>
#include <stdexcept>

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

static dif finalize_dif(const dif_hist& path,
		const std::string& original,
		const std::string& final) {
	dif_point dp;
	int i = 0;
	dif out;
	dif_segment ds;

	auto dif_append = [&](dif_segment::dif_segment_t type, char c){
		if(ds.s != "" && ds.type != type) {
			out.ds.push_back(ds);
			ds.s = "";
		}
		ds.type = type;
		ds.s += c;
	};

	while(dp.original_loc < original.size() ||
			dp.final_loc < final.size()) {
		if(original[dp.original_loc] == final[dp.final_loc]) {
			dif_append(dif_segment::preserved, final[dp.final_loc]);
			++dp.original_loc;
			++dp.final_loc;
		} else if(path.instructions[i] == dif_hist::ins) {
			dif_append(dif_segment::insertion, final[dp.final_loc]);
			++dp.final_loc;
			++i;
		} else {//del
			dif_append(dif_segment::deletion, original[dp.original_loc]);
			++dp.original_loc;
			++i;
		}
	}
	if(ds.s.size() > 0)
		out.ds.push_back(ds);
	return out;
}

static std::vector<const dif_hist*> paths_at_end(
		const std::vector<dif_hist>& paths,
		const std::string& original,
		const std::string& final) {
	std::vector<const dif_hist*> out;
	for(const dif_hist& path : paths) {
		if(path.current.original_loc == original.size() &&
				path.current.final_loc == final.size())
		out.push_back(&path);
	}
	return out;
}

static bool occurs_in(
		const std::unordered_set<dif_point,dif_point_hash>& past,
		dif_point point) {
	return past.find(point) != past.cend();
}

static dif_hist& dif_follow_preservation(
		dif_hist& path,
		const std::string& original,
		const std::string& final) {
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

static dif_hist& dif_ins(
		dif_hist& path,
		const std::string& original,
		const std::string& final) {
	path.instructions.push_back(dif_hist::ins);
	path.current.final_loc += 1;
	path.last_ins = true;
	return dif_follow_preservation(path,original,final);
}

static dif_hist& dif_del(
		dif_hist& path,
		const std::string& original,
		const std::string& final) {
	path.instructions.push_back(dif_hist::del);
	path.current.original_loc += 1;
	path.last_ins = false;
	return dif_follow_preservation(path,original,final);
}



dif calc_dif(const std::string& original, const std::string& final) {
	std::vector<dif_hist> paths;
	std::vector<dif_hist> new_paths;
	std::vector<const dif_hist*> end_paths;
	{
		dif_hist tmp;
		paths.push_back(dif_follow_preservation(tmp,original,final));
	}
	end_paths = paths_at_end(paths,original,final);

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
		end_paths = paths_at_end(paths,original,final);
	}
	//size of end_paths should always be 1
	return finalize_dif(*end_paths.front(),original,final);
}

//  preserved [default]
//# deletion  [red]
//> insertion [green]
//% modified  [blue]


std::ostream& operator<<(std::ostream& os, const dif& d) {
	Color::Modifier red(Color::FG_RED);
	Color::Modifier grn(Color::FG_GREEN);
	Color::Modifier def(Color::FG_DEFAULT);

	for(const dif_segment& ds : d.ds) {
		switch(ds.type) {
		case dif_segment::deletion:
		  os << red << ds.s;
			break;
		case dif_segment::insertion:
		  os << grn << ds.s;
			break;
		case dif_segment::preserved:
		default:
			os << def << ds.s;
		}
	}
  os << def;
	return os;
}
