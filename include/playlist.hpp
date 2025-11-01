#pragma once
#include <iostream>
#include <vector>

void showPlaylists(const char *authCode, const std::string &clientId,
                   const std::string &clientSecret,
                   const std::string &redirectUri);

int selectPlaylist(const std::vector<std::string> &playlists);
void playPlaylist(std::string &accessToken, std::string &playlistId);
