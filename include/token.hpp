#include <ctime>
#include <json/json.h>
#include <string>

struct Tokens {
  std::string accessToken;
  std::string refreshToken;
  time_t expiresAt;
};

void saveTokens(const Tokens &tokens, const std::string &file = "tokens.json");
bool loadTokens(Tokens &tokens, const std::string &file = "tokens.json");
bool isExpired(const Tokens &tokens);
