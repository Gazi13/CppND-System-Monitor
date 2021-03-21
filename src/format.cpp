#include <string>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) {
  int hh, mm, ss;
  stringstream output;
  hh = (seconds / 3600) % 100; // Number of hours returns to 0 after 99
  mm = (seconds / 60) % 60;
  ss = seconds % 60;
  output << std::setfill('0') << std::setw(2) << hh << ":"
         << std::setfill('0') << std::setw(2) << mm << ":"
         << std::setfill('0') << std::setw(2) << ss;
  return output.str();
}	