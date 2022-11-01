# PilePoxyl
The ultimate MCBE creator toolkit from [PixelPoly](https://tools.pixelpoly.co), condensed into a Discord application.
## Features
* Near-perfect feature parity with the reference website
* Useful parameter descriptions from /help
* Semi-cross-platform with CMake
* Super-easy setup interface
* Minimal dependencies
## Some hurdles yet to be overcome
* Detect `config/config.json` instead of a separate `FIRSTRUN` file to determine first run
* UTF-16 handling for filenames on Windows, required by `generate_tests`
* Instead of hard-coding the URLs, move them to a separate config file
* Limit code-copying and redundancy at `restapi::respond_interaction`
* C++ functions and class methods should take in reference arguments
## CMake targets
* `generate_tests` - Test program that generates/transforms data from source code + the resources folder
* `runpp` - The actual program to launch and host a Discord application
## How to build
### GNU/Linux
By far the easiest OS to work with. Install CURL, libwebsockets from your package manager, and you're ready to go!
* `runpp` builds and runs correctly on Gentoo using GCC. Other distributions have not been tested but they may work as well.
* `generate_tests` builds and runs correctly on Gentoo using GCC. Other distributions have not been tested but they may work as well.
### Windows
A headache to work with and support when it comes to dependencies.
* `runpp` builds and runs correctly (with a bunch of warnings) on Windows 10 and Windows 11 using the MSVC toolkit.
* `generate_tests` requires an implementation of `file_management.cpp` that handles filenames in UTF-16, which doesn't exist yet.
### BSD/Darwin (macOS)
Support for such system is nonexistent. Contributions are welcome.
## External dependencies
* CURL (HTTP handling with an easy-to-use interface)
* libwebsockets (WebSocket handling with a reasonable but hard-to-use interface)
## Self-configuring the program
The first run interface provided by runpp is the best way to bootstrap the bot, but occasionally one might want to edit the generated config file. The configuration is saved in `config/config.json` in JSON for ease of editing. Go to `config/README.md` for more information.