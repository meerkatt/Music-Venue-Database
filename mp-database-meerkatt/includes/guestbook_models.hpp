#ifndef DB_GUESTBOOK_MODELS_HPP
#define DB_GUESTBOOK_MODELS_HPP

#include <string>

struct Guest {
  int id;
  std::string name;
  std::string note;
};
#endif /* DB_MODELS_HPP */
