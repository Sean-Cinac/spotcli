#include "../include/playlist.hpp"
#include <cstring>
#include <curl/curl.h>
#include <iostream>
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
}
