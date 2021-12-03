#ifndef DB_MUSIC_MODELS_HPP
#define DB_MUSIC_MODELS_HPP

#include <string>
#include "json.hpp"

using json = nlohmann::json;

struct Artist {
    int id;
    std::string name;
};

struct Album {
    int id;
    std::string name;
    std::unique_ptr<int> albumArtist;
};

#endif /* DB_MODELS_HPP */
