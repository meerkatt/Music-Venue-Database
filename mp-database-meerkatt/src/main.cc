#include <stdlib.h>

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "guestbook_models.hpp"
#include "httplib.hpp"
#include "json.hpp"
#include "music_models.hpp"
#include "sqlite_orm.hpp"

using namespace httplib;
using namespace sqlite_orm;
using string = std::string;
using json = nlohmann::json;

int main(int argc, char* argv[]) {
  auto storage = make_storage(
      argv[1],
      make_table("artist",
                 make_column("id", &Artist::id, autoincrement(), primary_key()),
                 make_column("name", &Artist::name)),
      make_table("album",
                 make_column("id", &Album::id, autoincrement(), primary_key()),
                 make_column("name", &Album::name),
                 make_column("albumArtist", &Album::albumArtist),
                 foreign_key(&Album::albumArtist).references(&Artist::id)),
      make_table("guest",
                 make_column("id", &Guest::id, autoincrement(), primary_key()),
                 make_column("name", &Guest::name),
                 make_column("note", &Guest::note)));

  storage.sync_schema();

  Server svr;

  svr.Get("/", [&](const Request& req, Response& res) {
    res.set_content("The server works", "text/plain");
  });

  svr.Get("/hi", [&](const Request& req, Response& res) {
    res.status = 404;
    res.set_content("The server doesn't work", "text/plain");
  });

  svr.Post("/guests", [&](const Request& req, Response& res) {
    if (req.has_param("name") && req.has_param("note")) {
      Guest temp;
      temp.name = req.get_param_value("name");
      temp.note = req.get_param_value("note");
      storage.insert(temp);
    } else if (req.has_param("name")) {
      Guest temp;
      temp.name = req.get_param_value("name");
      temp.note = "";
      storage.insert(temp);
    }
  });

  svr.Get("/guests", [&](const Request& req, Response& res) {
    auto jason_array = nlohmann::json::array();
    nlohmann::json temp_json_object;
    std::vector<Guest> guest_list = storage.get_all<Guest>();
    for (size_t i = 0; i < guest_list.size(); ++i) {
      // int id = guest_list.at(i).id;
      string name = guest_list.at(i).name;
      string note = guest_list.at(i).note;
      nlohmann::json temp_json_object;
      // temp_json_object["id"] = id;
      temp_json_object["name"] = name;
      temp_json_object["note"] = note;
      jason_array += temp_json_object;
    }

    res.set_content(jason_array.dump(), "application/json");
  });

  svr.Get("/music/artist", [&](const Request& req, Response& res) {
    auto jason_array = nlohmann::json::array();
    nlohmann::json temp_json_object;
    std::vector<Artist> artist_list = storage.get_all<Artist>();
    for (size_t i = 0; i < artist_list.size(); ++i) {
      Artist temp = storage.get<Artist>(i + 1);
      string name = temp.name;
      int id = temp.id;
      nlohmann::json temp_json_object;
      temp_json_object["id"] = id;
      temp_json_object["name"] = name;
      jason_array += temp_json_object;
    }
    res.set_content(jason_array.dump(), "application/json");
  });

  svr.Post("/music/artist", [&](const Request& req, Response& res) {
    if (req.has_param("name")) {
      Artist temp;
      temp.name = req.get_param_value("name");
      temp.id = storage.insert(temp);
      res.status = 201;
      string url = "/music/artist/" + std::to_string(temp.id);
      res.set_header("Location", url);
    }
  });

  svr.Get("/music/album", [&](const Request& req, Response& res) {
    auto jason_array = nlohmann::json::array();
    nlohmann::json temp_json_object;
    std::vector<Album> album_list = storage.get_all<Album>();
    for (size_t i = 0; i < album_list.size(); ++i) {
      Album temp = storage.get<Album>(i + 1);
      string name = temp.name;
      int id = temp.id;
      nlohmann::json temp_json_object;
      temp_json_object["id"] = id;
      temp_json_object["name"] = name;
      temp_json_object["artist"] = *(temp.albumArtist);
      jason_array += temp_json_object;
    }
    res.set_content(jason_array.dump(), "application/json");
  });

  svr.Post("/music/album", [&](const Request& req, Response& res) {
    if (req.has_param("name") && req.has_param("artistID")) {
      Album temp;
      temp.name = req.get_param_value("name");
      temp.albumArtist = std::unique_ptr<int>(
          new int(std::stoi(req.get_param_value("artistID"))));
      temp.id = storage.insert(temp);
      res.status = 201;
      string url = "/music/album/" + std::to_string(temp.id);
      res.set_header("Location", url);
    } else {
      res.status = 400;
    }
  });

  svr.Get(R"(/music/artist/(\d+))", [&](const Request& req, Response& res) {
    auto artist_id = req.matches[1];
    auto jason_array = nlohmann::ordered_json::array();
    auto artist =
        storage.get_all<Artist>(where(c(&Artist::id) == stoi(artist_id)));
    nlohmann::ordered_json main_json_object;
    if (artist.size() == 0) {
      res.status = 404;
    }
    main_json_object["name"] = artist.at(0).name;
    nlohmann::ordered_json temp_json_object;
    int count = 0;
    std::vector<Album> album_list = storage.get_all<Album>();
    for (size_t i = 0; i < album_list.size(); ++i) {
      Album temp = storage.get<Album>(i + 1);
      if (artist.at(0).id == *(temp.albumArtist)) {
        string name = temp.name;
        int id = temp.id;
        temp_json_object["id"] = id;
        temp_json_object["name"] = name;
        temp_json_object["artist"] = *(temp.albumArtist);
        jason_array += temp_json_object;
        ++count;
      }
    }
    main_json_object["albums"] = jason_array;
    res.set_content(main_json_object.dump(), "application/json");
    res.status = 200;
  });

  svr.Delete("/guests", [&](const Request& req, Response& res) {
    storage.remove_all<Guest>();
  });
  svr.listen("0.0.0.0", 1234);
}