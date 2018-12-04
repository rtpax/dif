#include "dif.hh"

#include <unordered_set>
#include <stdexcept>
#include <string>

#ifdef __GNUC__
#define VARIABLE_IS_NOT_USED __attribute__ ((unused))
#else
#define VARIABLE_IS_NOT_USED
#endif


namespace cmp {

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

VARIABLE_IS_NOT_USED
static std::ostream& operator<<(std::ostream& os, dif_point dp) {
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

	auto dif_append = [&](segment_type type, typename T::value_type c){
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
			dif_append(preserved, final[dp.final_loc]);
			++dp.original_loc;
			++dp.final_loc;
		} else if(path.instructions[i] == dif_hist::ins) {
			dif_append(insertion, final[dp.final_loc]);
			++dp.final_loc;
			++i;
		} else {//del
			dif_append(deletion, original[dp.original_loc]);
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


template dif<std::string> calc_dif<std::string>(
		const std::string&, const std::string&);
template dif<std::vector<std::string>> calc_dif<std::vector<std::string>>(
		const std::vector<std::string>&, const std::vector<std::string>&);


}
