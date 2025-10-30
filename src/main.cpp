#include "../include/spotcli.hpp"

int main() {
  std::cout << "spotcli main" << '\n';
  uint32_t choice{askForChoice()};

  processChoice(choice);
}

std::string urlEncode(const std::string &value) {
  std::ostringstream escaped;
  escaped.fill('0');
  escaped << std::hex;

  for (char c : value) {
    if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
      escaped << c;
    } else if (c == ' ') {
      escaped << "%20";
    } else {
      escaped << '%' << std::uppercase << std::setw(2) << int((unsigned char)c)
              << std::nouppercase;
    }
  }

  return escaped.str();
}

std::unordered_map<std::string, std::string> loadEnv(const std::string &path) {
  std::unordered_map<std::string, std::string> env;
  std::ifstream file(path);
  std::string line;
  while (std::getline(file, line)) {
    if (line.empty() || line[0] == '#')
      continue;
    auto pos = line.find('=');
    if (pos == std::string::npos)
      continue;
    std::string key = line.substr(0, pos);
    std::string value = line.substr(pos + 1);
    env[key] = value;
  }
  return env;
}

uint32_t askForChoice() {
  uint32_t choice = {5};

  while (choice > 4) {
    std::cout << "You have these options:" << '\n'
              << "auth: 0" << '\n'
              << "play playlist: 1" << '\n'
              << "play liked songs: 2" << '\n'
              << "play artist: 3" << '\n'
              << "play song: 4" << '\n';
    if (!(std::cin >> choice)) {
      std::cin.clear();
      std::cin.ignore(1000, '\n');
      std::cout << "invalid input, 1-4" << '\n';
    }
  }
  return choice;
}

void processChoice(uint32_t choice) {
  switch (choice) {
  case 0:
    authSpotify();
    break;
  }
}
void authSpotify() {
  std::unordered_map<std::string, std::string> env = loadEnv("../.env");

  std::string client_id = env["SPOTIFY_CLIENT_ID"];
  std::string redirect_uri = env["REDIRECT_URI"];
  std::string scope = "user-library-read playlist-read-private streaming";

  std::ostringstream url;
  url << "https://accounts.spotify.com/authorize?"
      << "client_id=" << urlEncode(client_id) << "&response_type=code"
      << "&redirect_uri=" << urlEncode(redirect_uri)
      << "&scope=" << urlEncode(scope);

  std::string urlStr = url.str();
  std::cout << "Opening: " << urlStr << '\n';

#if defined(_WIN32) || defined(_WIN64)
  system(("start \"\" \"" + urlStr + "\"").c_str());
#elif defined(__APPLE__)
  system(("open \"" + urlStr + "\"").c_str());
#else
  system(("xdg-open \"" + urlStr + "\"").c_str());
#endif
}
