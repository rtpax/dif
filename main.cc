#include "dif.hh"
#include <iostream>

int main(int argc, char ** argv) {
  std::string original;
	std::string final;

  if(argc >= 3) {
  	original = argv[1];
  	final = argv[2];
  } else {
  	original = "There are strange things done in the midnight sun";
  	final = "There are totally not strange things at midnight";
  }

	dif d = calc_dif(original,final);
	std::cout << d << "\n";
}
