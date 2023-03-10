#! /bin/bash

# Variables
base_dir="/opt"
logfilegen_dir="logfilegen"
logfilegen_version="${logfilegen_version}"
logfilegen_compiler="${logfilegen_compiler}"

# Get sources
git clone -b "$logfilegen_version" https://github.com/psemiletov/logfilegen.git "$base_dir/$logfilegen_dir"

# Compile
if [[ "$logfilegen_compiler" == "g++" ]]; then
  make -C "$base_dir/$logfilegen_dir" CXX=g++
else
  make -C "$base_dir/$logfilegen_dir"
fi

# Install
make -C "$base_dir/$logfilegen_dir" install


# Install logfilegen from git release
# curl https://github.com/psemiletov/logfilegen/releases/latest/download/logfilegen -L -o logfilegen
# install -m 0755 logfilegen /usr/bin/logfilegen
