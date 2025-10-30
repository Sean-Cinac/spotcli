#include <cstdint>
#include <iostream>

uint32_t askForChoice();
void processChoice(uint32_t choice);
void authSpotify();

int main() {
  std::cout << "spotcli main" << '\n';
  uint32_t choice{askForChoice()};

  processChoice(choice);
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

void authSpotify() {}
