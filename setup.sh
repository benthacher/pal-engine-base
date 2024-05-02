#!/bin/sh

# Add pal-engine submodule if it doesn't exist
if [ ! -d pal-engine ]; then
    git submodule add git@github.com:benthacher/pal-engine.git
fi

# Make sure pal-engine is up to date
git submodule update --remote

# Run setup script for pal-engine
cd ./pal-engine/util
./setup.sh