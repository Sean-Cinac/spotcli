#include "../include/playlist.hpp"
#include <cstdio>
#include <cstring>
#include <curl/curl.h>
#include <curl/easy.h>
#include <iostream>
#include <json/config.h>
#include <json/json.h>
#include <json/reader.h>
#include <sstream>
#include <string>

size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
  std::string *s = static_cast<std::string *>(userp);
  size_t total = size * nmemb;
  s->append(static_cast<char *>(contents), total);
  return total;
}

void showPlaylists(const char *authCode) {
  if (!authCode || std::strlen(authCode) == 0) {
    std::cerr << "ERROR: auth code is empty" << '\n';
    return;
  }

  CURL *curl = curl_easy_init();
  if (!curl) {
    std::cerr << "ERROR: to init curl" << '\n';
    return;
  }

  std::string readBuffer;

  const char *url = "https://api.spotify.com/v1/me/playlists";

  char authHeader[512];
  std::snprintf(authHeader, sizeof(authHeader), "Authorization: Bearer %s",
                authCode);

  struct curl_slist *headers = nullptr;
  headers = curl_slist_append(headers, authHeader);
  headers = curl_slist_append(headers, "Content-Type: application/json");

  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

  CURLcode res = curl_easy_perform(curl);

  if (res != CURLE_OK) {
    std::cerr << "WARNING: Request failed: " << curl_easy_strerror(res) << '\n';
  } else {
    Json::CharReaderBuilder reader;
    Json::Value jsonData;
    Json::String errs;
    std::istringstream s(readBuffer);

    if (Json::parseFromStream(reader, s, &jsonData, &errs)) {
      std::cout << '\n' << "Your playlists" << '\n';
      uint16_t index{1};

      for (const auto &item : jsonData["items"]) {
        std::cout << index++ << ". " << item["name"].asString() << " ("
                  << item["id"].asString() << ")" << '\n';
      }
    } else {
      std::cerr << "WARNING: failed to parse JSON: " << errs << '\n';
    }
  }

  curl_slist_free_all(headers);
  curl_easy_cleanup(curl);
}
