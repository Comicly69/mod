# mod
A cli for fetching minecraft mods in a simple way not requiring a browser.

NOTE: currently only for linux, windows version coming to release 1.0

# Install

run `install.sh`

# build from source

`git clone https://github.com/Comicly69/mod.git`

Move to bin directory:

`cd bin/`

Compile:

`run gcc mod.c -o mod -ljansson`

Install/get a mod

`run ./mod install/get <mod name>`

Move to /usr/local/bin for systemwide acsess:

`sudo mv mod /usr/local/bin`

Elevate permissions:

`sudo chmod +x mod`

Install/get a mod

`mod get cloth-config`
