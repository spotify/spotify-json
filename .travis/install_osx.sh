#!/bin/bash

function install_or_upgrade {
  if ! brew ls --version $1 &>/dev/null; then
    brew install $1
  else
    brew upgrade $1
  fi
  brew ls --version $1
}

install_or_upgrade boost # at least version 1.60
install_or_upgrade cmake # at least version 3.2
echo "Installed build dependecies."
