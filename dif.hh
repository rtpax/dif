#include <vector>
#include <ostream>

template<class T>
struct dif_segment {
	enum dif_segment_t {
	  insertion,
	  deletion,
	  preserved,
	  modified,
	  none
	} type;
	T s;
};

template<class T>
struct dif {
	std::vector<dif_segment<T>> ds;
};

template<class T>
dif<T> calc_dif(const T& orginal, const T& final);

void dif_ostream_show_line_info();
void dif_ostream_hide_line_info();
bool dif_ostream_has_line_info();
void dif_ostream_show_color();
void dif_ostream_hide_color();
bool dif_ostream_has_color();

template<class T>
std::ostream& operator<<(std::ostream&, const dif<T>& d);

