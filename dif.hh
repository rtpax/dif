#include <string>
#include <vector>
#include <ostream>

struct dif_segment {
	enum dif_segment_t {
	  insertion,
	  deletion,
	  preserved
	} type;
	std::string s;
};

struct dif {
	std::vector<dif_segment> ds;
};

dif calc_dif(const std::string& orginal, const std::string& final);

std::ostream& operator<<(std::ostream&, const dif& d);
