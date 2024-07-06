#ifndef __util_string_h__
#define __util_string_h__

#include <algorithm> 
#include <cctype>
#include <locale>
#include <vector>

// trim from start (in place)
static inline string& ltrim(std::string& s)
  {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
    return !std::isspace(ch);
    }));

  return s;
  }

// trim from end (in place)
static inline string& rtrim(std::string& s)
  {
  s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
    return !std::isspace(ch);
    }).base(), s.end());

  return s;
  }

// trim from both ends (in place)
static inline string& trim(std::string& s)
  {
  return rtrim(ltrim(s));
  }

// trim from start (copying)
static inline std::string ltrim_copy(std::string s)
  {
  ltrim(s);
  return s;
  }

// trim from end (copying)
static inline std::string rtrim_copy(std::string s)
  {
  rtrim(s);
  return s;
  }

// trim from both ends (copying)
static inline std::string trim_copy(std::string s)
  {
  trim(s);
  return s;
  }

static inline std::vector<std::string> split(const std::string& str, char sep)
  {
  std::vector <std::string> strings;
  // Create custom split() function.  
  int start = 0;
  for (int i = 0; i <= str.size(); i++)
    {
    // If we reached the end of the word or the end of the input.
    if (str[i] == sep || i == str.size())
      {
      strings.push_back(str.substr(start, i - start));
      start = i + 1;
      }
    }

  return strings;
  }

#endif

