# Pilepoxyl
The ultimate MCBE creator toolkit from [PolyPixel](https://tools.pixelpoly.co), Condensed into a Discord application.
## Features
* Near-perfect feature parity with the reference website
* Useful parameter descriptions from /help
* Semi-cross-platform with CMake
* Super-easy setup interface
* Minimal dependencies
## Some hurdles yet to be overcome
* C++ functions and class methods should take in reference arguments
* Limit code-copying and redundancy at `restapi::respond_interaction`
* Remove the need for a keyfile for OpenSSL (libwebsockets)
* Copying DLLs over to the output folder for Windows
* UTF-16 handling for filenames on Windows
## How to build
### Linux
By far the easiest OS to work with. Install CURL, libwebsockets and minizip (from zlib) from your package manager, and you're ready to go!
### Windows
Windows builds are failing somehow, but that winext folder is there so it is intended to be supported 😅
### BSD/Darwin (macOS)
Support for such system is nonexistent. Meanwhile install brew and hope that the code even compiles. Contributions are welcome.
## CMake targets
* `generate_tests` - Test program that generates/transforms data from source code + the resources folder
* `runpp` - The actual program to launch and host a Discord application
## External dependencies
* CURL (HTTP handling with an easy-to-use interface)
* libwebsockets (WebSocket handling with a reasonable but hard-to-use interface)
* zlib + minizip (lightweight zipfile and deflate handling)