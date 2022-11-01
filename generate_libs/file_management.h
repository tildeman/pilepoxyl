#ifndef fileman
#define fileman

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <regex>
#include "minizip/headers/win32/zip.h"
#include "uuid_generator.h"


typedef std::basic_string<unsigned char> bstring;

// ***************************************
// * Note to self: use the std namespace *
// ***************************************

// *******************************
// * Lookup table for file types *
// *******************************
// +-------------+------------+
// | Filetype ID |  Used for  |
// +-------------+------------+
// |      0      |   Binary   |
// +-------------+------------+
// |      1      |    Text    |
// +-------------+------------+
// |      2      |  Directory |
// +-------------+------------+


class file{
	private:
		//Strips and reduces forward slashes
		std::string truncate_slashes(std::string strink);
		// Recursively add files to a minizip zipFile of the "zfptr" pointer
		void add_files_to_zip(std::string subdirectory,zipFile* zfptr);
		// Sets the date of of the instance object zip_fi
		void initialize_zfi();
	public:
		int filetype;
		bstring binary_contents;
		std::string text_contents;
		std::vector<file> directory_contents;
		std::string filename;
		static zip_fileinfo zip_fi;
		// Creates an empty file
		file();
		// Creates a folder with a table of contents
		file(std::string dirname,std::vector<file> contents);
		// Creates a file with predefined contents
		file(std::string filename,std::string contents);
		// Reads a file, whether text or binary
		void read_file(std::string filename,bool is_binary=false);
		// Writes a file to the path given by the instance variable "filename"
		void write_file();
		// Creates a .zip file and returns the corresponding filename
		std::string make_zip(const char* extension=".zip");
		// Retrieve the string representation of the directory structure
		std::string get_repr(int indent=0);
};

#endif