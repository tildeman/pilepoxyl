#include "file_management.h"

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


// Parse filenames in zip using UTF-8 with this std::string
const unsigned char exfl[]={
	// Tag
	0x75, 0x78,
	// Length
	0x0B, 0x00,
	// Fixed std::string with unknown properties
	0x01, 0x04, 0xE8, 0x03, 0x00, 0x00, 0x04, 0xE8, 0x03, 0x00, 0x00
};

// External attributes for files
const long ext_attrs=0644 << 16l;

zip_fileinfo file::zip_fi;

std::string file::truncate_slashes(std::string strink){
	std::regex regex_string(".*[\\\\\\/](.+)");
	std::smatch match_string;
	bool a=std::regex_match(strink,match_string,regex_string);
	if (a){
		return match_string[1];
	}
	return strink;
}
file::file(){
	this->filetype=1;
	this->filename="";
}
file::file(std::string dirname,std::vector<file> contents){
	this->filetype=2;
	this->filename=dirname;
	this->directory_contents=contents;
}
file::file(std::string filename,std::string contents){
	this->filename=filename;
	this->filetype=1;
	this->text_contents=contents;
}
void file::initialize_zfi(){
	time_t unixtime=time(NULL);
	struct tm* current_time=localtime(&unixtime);

	file::zip_fi.dosDate=0;
	file::zip_fi.external_fa=ext_attrs;
	file::zip_fi.internal_fa=0;
	file::zip_fi.tmz_date.tm_sec=current_time->tm_sec;
	file::zip_fi.tmz_date.tm_min=current_time->tm_min;
	file::zip_fi.tmz_date.tm_hour=current_time->tm_hour;
	file::zip_fi.tmz_date.tm_mday=current_time->tm_mday;
	file::zip_fi.tmz_date.tm_mon=current_time->tm_mon;
	file::zip_fi.tmz_date.tm_year=current_time->tm_year;
}
void file::read_file(std::string filename,bool is_binary){
	if (is_binary){
		std::ifstream f(filename,std::ios::binary);
		this->filetype=0;
		this->filename=truncate_slashes(filename);
		f.seekg(0,std::ios::end);
		std::streampos length=f.tellg();
		f.seekg(0,std::ios::beg);
		this->binary_contents=bstring(length,0);
		f.read((char*)&(this->binary_contents)[0],length);
	}
	else{
		std::fstream f(filename,std::ios::in|std::ios::ate);
		this->filetype=1;
		this->filename=truncate_slashes(filename);
		f.seekg(0,std::ios::end);
		std::streampos length=f.tellg();
		f.seekg(0,std::ios::beg);
		this->text_contents=std::string(length,'\0');
		f.read(&(this->text_contents)[0],length);
	}
}
void file::write_file(){
	if (this->filetype==0){
		std::ofstream w(filename,std::ios::binary);
		w.write((char*)&(this->binary_contents)[0],this->binary_contents.length());
		w.close();
	}
	else if (this->filetype==1){
		std::ofstream w(filename);
		w << this->text_contents;
		w.close();
	}
}

void file::add_files_to_zip(std::string subdirectory,zipFile* zfptr){
	switch (this->filetype){
		case 0:
			zipOpenNewFileInZip4_64(*zfptr,subdirectory.c_str(),&(file::zip_fi),exfl,15,exfl,15,nullptr,Z_DEFLATED,Z_DEFAULT_COMPRESSION,0,-MAX_WBITS,DEF_MEM_LEVEL,Z_DEFAULT_STRATEGY,nullptr,0,798,2048u,1);
			zipWriteInFileInZip(*zfptr,this->binary_contents.c_str(),this->binary_contents.length());
			zipCloseFileInZip(*zfptr);
			break;
		case 1:
			zipOpenNewFileInZip4_64(*zfptr,subdirectory.c_str(),&(file::zip_fi),exfl,15,exfl,15,nullptr,0,Z_DEFAULT_COMPRESSION,0,-MAX_WBITS,DEF_MEM_LEVEL,Z_DEFAULT_STRATEGY,nullptr,0,798,2048u,1);
			zipWriteInFileInZip(*zfptr,this->text_contents.c_str(),this->text_contents.length());
			zipCloseFileInZip(*zfptr);
			break;
		case 2:
			for (file i:this->directory_contents){
				const char* dl="/";
				i.add_files_to_zip(subdirectory+(dl+(subdirectory.length()==0))+i.filename,zfptr);
			}
			break;
		default:
			break;
	}
}

std::string file::make_zip(const char* extension){
	zip_fileinfo zfi;
	uuid_generator u;
	this->initialize_zfi();
	filename=std::string("tmp/")+u.generate_uuid()+extension;
	zipFile zfp=zipOpen(filename.c_str(),0);
	add_files_to_zip("",&zfp);
	zipClose(zfp,nullptr);
	return filename;
}
std::string file::get_repr(int indent){
	int i;
	std::string s;
	for (i=1;i<indent;i++) s+="| ";
	if (indent) s+="|-";
	s+=this->filename;
	s+="\n";
	if (this->filetype==2){
		for (i=0;i<this->directory_contents.size();i++){
			s+=this->directory_contents[i].get_repr(indent+1);
		}
	}
	return s;
}