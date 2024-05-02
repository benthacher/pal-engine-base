#!/bin/sh

if git submodule status | grep --quiet '^-'; then
    git submodule add -b main git@github.com:benthacher/pal-engine.git
fi

if [ ! -d "./pal-engine" ]; then
    git submodule update --init --recursive
fi

# Make sure pal-engine is up to date
git submodule update --remote --recursive

# Run setup script for pal-engine
cd ./pal-engine/util
./setup.sh