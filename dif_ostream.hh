#ifndef DIF_OSTREAM_HH
#define DIF_OSTREAM_HH
#include <ostream>

namespace cmp {
template<class T>
struct dif;

void dif_ostream_show_line_info();
void dif_ostream_hide_line_info();
bool dif_ostream_has_line_info();

void dif_ostream_show_color();
void dif_ostream_hide_color();
void dif_ostream_background_color();
bool dif_ostream_has_color();
bool dif_ostream_has_background_color();

void dif_ostream_show_only_modified();
void dif_ostream_show_all();
bool dif_ostream_only_modified();

void dif_ostream_show_line_num();
void dif_ostream_hide_line_num();
bool dif_ostream_has_line_num();

}

template<class T>
std::ostream& operator<<(std::ostream&, const cmp::dif<T>& d);


#endif