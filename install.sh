#! /bin/bash

echo "Starting installation service..."
sleep 1

echo "Cloning repository..."
git clone https://github.com/Comicly69/mod.git > /dev/null
cd ~/mod/

echo "Compiling binaries..."
gcc mod.c -o mod -ljansson
mv mod bin/
cd bin/

echo "Moving to /usr/local/bin (requires sudo)..."
sudo cp mod /usr/local/bin

echo "Elevating permissions of binaries"
sudo chmod +x /usr/local/bin/mod

echo "Install complete! run mod get/install <mod name> to install a mod"