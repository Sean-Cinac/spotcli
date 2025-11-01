#include "../include/playlist.hpp"
#include "../include/authSpotify.hpp"
#include "../include/token.hpp"
#include "../include/utils.hpp"

#include <cstdio>
#include <cstring>
#include <curl/curl.h>
#include <curl/easy.h>
#include <iostream>
#include <json/config.h>
#include <json/json.h>
#include <json/reader.h>
#include <ncurses.h>
#include <sstream>
#include <string>
#include <time.h>
#include <vector>

void showPlaylists(const char *authCode, const std::string &clientId,
                   const std::string &clientSecret,
                   const std::string &redirectUri) {
  Tokens tokens;

  if (loadTokens(tokens) && !isExpired(tokens)) {
    std::cout << "using locally saved token" << '\n';

  } else {
    if (!authCode || std::strlen(authCode) == 0) {
      std::cerr << "ERROR: auth code is empty" << '\n';
      return;
    }
    std::string access_token =
        getAccessToken(authCode, clientId, clientSecret, redirectUri);
    if (access_token.empty()) {
      std::cerr << "ERROR: failed to get Access Token" << '\n';
      return;
    }

    tokens.accessToken = access_token;
    tokens.refreshToken = "TODO";
    tokens.expiresAt = std::time(nullptr) + 3600;
    saveTokens(tokens);
  }
  std::string access_token = tokens.accessToken;

  CURL *curl = curl_easy_init();
  if (!curl) {
    std::cerr << "ERROR: to init curl" << '\n';
    return;
  }

  std::string readBuffer;

  const char *url = "https://api.spotify.com/v1/me/playlists";

  char authHeader[512];
  std::snprintf(authHeader, sizeof(authHeader), "Authorization: Bearer %s",
                access_token.c_str());

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
      std::vector<std::string> playlistNames;

      for (const auto &item : jsonData["items"]) {
        playlistNames.push_back(item["name"].asString());
      }

      int selectedIndex = selectPlaylist(playlistNames);

    } else {
      std::cerr << "WARNING: failed to parse JSON: " << errs << '\n';
      return;
    }
  }

  curl_slist_free_all(headers);
  curl_easy_cleanup(curl);
}
int selectPlaylist(const std::vector<std::string> &playlists) {
  initscr();
  noecho();
  cbreak();
  keypad(stdscr, TRUE);

  int highlight = 0;
  int choice = -1;
  int ch;

  while (true) {
    clear();
    for (size_t i = 0; i < playlists.size(); i++) {
      if ((int)i == highlight)
        attron(A_REVERSE);
      mvprintw(i, 0, "%s", playlists[i].c_str());
      if ((int)i == highlight)
        attroff(A_REVERSE);
    }
    ch = getch();
    switch (ch) {
    case KEY_UP:
      highlight--;
      if (highlight < 0)
        highlight = playlists.size() - 1;
      break;
    case KEY_DOWN:
      highlight++;
      if (highlight >= (int)playlists.size())
        highlight = 0;
      break;
    case 10: // Enter key
      choice = highlight;
      goto endLoop;
    }
  }

endLoop:
  endwin();
  return choice;
}
