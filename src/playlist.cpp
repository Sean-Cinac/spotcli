#include "../include/playlist.hpp"
#include "../include/spotcli.hpp"
#include "../include/token.hpp"
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

size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
  std::string *s = static_cast<std::string *>(userp);
  size_t total = size * nmemb;
  s->append(static_cast<char *>(contents), total);
  return total;
}

std::string base64Encode(const std::string &in) {
  static const std::string chars =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

  std::string out;
  int val = 0, valb = -6;
  for (unsigned char c : in) {
    val = (val << 8) + c;
    valb += 8;
    while (valb >= 0) {
      out.push_back(chars[(val >> valb) & 0x3F]);
      valb -= 6;
    }
  }
  if (valb > -6) {
    out.push_back(chars[((val << 8) >> (valb + 8)) & 0x3F]);
  }
  while (out.size() % 4) {
    out.push_back('=');
  }
  return out;
}

std::string getAccessToken(const std::string &authCode,
                           const std::string &clientId,
                           const std::string &clientSecret,
                           const std::string &redirectUri) {

  CURL *curl = curl_easy_init();
  if (!curl) {
    std::cerr << "ERROR: curl init failed\n";
    return "";
  }

  std::string readBuffer;

  std::string postFields = "grant_type=authorization_code&code=" + authCode +
                           "&redirect_uri=" + redirectUri;

  std::string authStr = clientId + ":" + clientSecret;

  std::string encodedAuth = "Authorization: Basic " + base64Encode(authStr);

  struct curl_slist *headers = nullptr;
  headers = curl_slist_append(headers, encodedAuth.c_str());
  headers = curl_slist_append(
      headers, "content-Type: application/x-www-form-urlencoded");

  curl_easy_setopt(curl, CURLOPT_URL, "https://accounts.spotify.com/api/token");
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postFields.c_str());
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

  CURLcode res = curl_easy_perform(curl);
  curl_slist_free_all(headers);
  curl_easy_cleanup(curl);

  if (res != CURLE_OK) {
    std::cerr << "ERROR: curl request failed: " << curl_easy_strerror(res)
              << "\n";
    return "";
  }

  // std::cout << "token response: " << readBuffer << "\n";
  Json::CharReaderBuilder reader;
  Json::Value jsonData;
  std::istringstream s(readBuffer);
  std::string errs;

  if (!Json::parseFromStream(reader, s, &jsonData, &errs)) {
    std::cerr << "ERROR: Failed to parse JSON: " << errs << "\n";
    return "";
  }

  if (!jsonData["access_token"].isString()) {
    std::cerr << "ERROR: access_token missing in response\n";
    return "";
  }

  return jsonData["access_token"].asString();
}

void showPlaylists(const char *authCode, const std::string &clientId,
                   const std::string &clientSecret,
                   const std::string &redirectUri) {
  if (!authCode || std::strlen(authCode) == 0) {
    std::cerr << "ERROR: auth code is empty" << '\n';
    return;
  }

  Tokens tokens;

  if (loadTokens(tokens) && !isExpired(tokens)) {
    std::cout << "using locally saved token" << '\n';

  } else {
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
