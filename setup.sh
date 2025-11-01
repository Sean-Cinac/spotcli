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
  echo "Installing dependencies for Debian/Ubuntu..."
  sudo apt install -y $DEPENDENCIES build-essential curl git libasound2-dev pkg-config
  echo "Installing Rust..."
  curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- -y
  source $HOME/.cargo/env
  echo "Building librespot..."
  git clone https://github.com/librespot-org/librespot.git /tmp/librespot
  cd /tmp/librespot
  cargo build --release
  sudo cp target/release/librespot /usr/local/bin/
}

install_packages_dnf() {
  echo "Installing dependencies for Fedora..."
  sudo dnf install -y cmake curl jsoncpp ncurses-devel @development-tools git alsa-lib-devel pkgconfig
  echo "Installing Rust..."
  curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- -y
  source $HOME/.cargo/env
  echo "Building librespot..."
  git clone https://github.com/librespot-org/librespot.git /tmp/librespot
  cd /tmp/librespot
  cargo build --release
  sudo cp target/release/librespot /usr/local/bin/
}

install_packages_pacman() {
  echo "Installing dependencies for Arch..."
  echo "Installing librespot from AUR..."
  yay -S --noconfirm librespot
  sudo pacman -S --needed --noconfirm $DEPENDENCIES base-devel curl git alsa-lib
}

install_packages_zypper() {
  echo "Installing dependencies for openSUSE/SUSE..."
  sudo zypper install -y cmake curl jsoncpp-devel ncurses-devel gcc make git alsa-devel
  echo "Installing Rust..."
  curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- -y
  source $HOME/.cargo/env
  echo "Building librespot..."
  git clone https://github.com/librespot-org/librespot.git /tmp/librespot
  cd /tmp/librespot
  cargo build --release
  sudo cp target/release/librespot /usr/local/bin/
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
