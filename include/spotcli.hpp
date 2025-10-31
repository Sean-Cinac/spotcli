#pragma once

#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>

uint32_t askForChoice();
void processChoice(uint32_t choice);
void authSpotify();
std::unordered_map<std::string, std::string> loadEnv();
std::string urlEncode(const std::string &value);
void showPlaylists();
