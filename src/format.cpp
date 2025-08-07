#include "format.h"

#include <string>

using std::string;

string twoDigits(int n)
{
  string res = std::to_string(n);
  if (n < 10) {
    res = "0" + res;
  }
  return res;
}

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds)
{
  long h, m, s;
  h = seconds / 3600;
  seconds %= 3600;
  m = seconds / 60;
  s = seconds % 60;

  return twoDigits(h) + ":" + twoDigits(m) + ":" + twoDigits(s);
}