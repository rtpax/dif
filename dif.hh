#ifndef DIF_HH
#define DIF_HH

#include <vector>
#include "dif_ostream.hh"

namespace cmp {

enum segment_type {
	insertion,
	deletion,
	preserved,
	modified,
	none	
};

template<class T>
struct dif_segment {
	segment_type type;
	T s;
};

template<class T>
struct dif {
	std::vector<dif_segment<T>> ds;
};



template<class T>
dif<T> calc_dif(const T& orginal, const T& final);

}




#endif