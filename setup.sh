#!/bin/bash
set -e

DEPENDENCIES="cmake curl jsoncpp ncurses build-essential git"

echo "Detecting Linux distribution..."
if [ -f /etc/os-release ]; then
  . /etc/os-release
  DISTRO=$ID
else
  echo "Cannot detect Linux distribution."
  exit 1
fi

echo "Detected distro: $DISTRO"

install_packages_apt() {
  sudo apt install -y $DEPENDENCIES
}

install_packages_dnf() {
  sudo dnf install -y cmake curl jsoncpp ncurses-devel @development-tools git
}

install_packages_pacman() {
  sudo pacman -S --needed --noconfirm cmake curl jsoncpp ncurses base-devel git
}

install_packages_zypper() {
  sudo zypper install -y cmake curl jsoncpp-devel ncurses-devel gcc make git
}

case "$DISTRO" in
ubuntu | debian)
  install_packages_apt
  ;;
fedora)
  install_packages_dnf
  ;;
arch)
  install_packages_pacman
  ;;
opensuse* | suse)
  install_packages_zypper
  ;;
*)
  echo "Unsupported Linux distribution: $DISTRO"
  exit 1
  ;;
esac

echo "All dependencies installed successfully."
