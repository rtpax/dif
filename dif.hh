#ifndef DIF_HH
#define DIF_HH

#include <vector>
#include "dif_ostream.hh"

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



#endif