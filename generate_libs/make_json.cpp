#include "make_json.h"

template <typename jso> json_object<jso>::json_object(){
	this->object_pair={};
}

template <typename jso> json_object<jso>::json_object(std::vector<std::pair<std::string,jso>> a){
	this->object_pair=a;
}

template <typename jso> jso& json_object<jso>::operator[](std::string s){
	int l=this->object_pair.size();
	for (int i=0;i<l;i++){
		if (object_pair[i].first==s) return object_pair[i].second;
	}
	object_pair.push_back({s,jso()});
	return object_pair[l].second;
}

std::string json_value::indent_json(std::string s){
	std::string o="";
	int l=s.length(),i,j;
	for (i=0;i<l;i++){
		if (s[i]=='\n'&&i<l-2) o+="\n\t";
		else o+=s[i];
	}
	return o;
}
std::string json_value::strip(std::string s){
	int begin,end;
	for (begin=0;begin<s.length();begin++){
		if (!(s[begin]==' '||s[begin]=='\t'||s[begin]=='\n'||s[begin]=='\r')) break;
	}
	for (end=s.length()-1;end>-1;end--){
		if (!(s[end]==' '||s[end]=='\t'||s[end]=='\n'||s[end]=='\r')) break;
	}
	if (begin>end) return "";
	std::string r=s.substr(begin,end-begin+1);
	return r;
}
int json_value::hex_to_int(std::string s){
	int l=s.length(),i;
	int c=-1,r=0;
	std::string lookup="0123456789ABCDEF";
	std::string lookup_lc="0123456789abcdef";
	for (i=l-1;i>-1;i--){
		c=lookup.find(s[i]);
		if (c==std::string::npos) c=lookup_lc.find(s[i]);
		r*=16;
		r+=c;
	}
	return r;
}
//Kept me up for a whole night
std::string json_value::unescapify(std::string s){
	int i,l=s.length();
	std::string r;
	for (i=0;i<l;i++){
		int p1=0,p2=0;
		if (s[i]=='\\'){
			switch (s[i+1]){
			case '"':
				r+='"';
				break;
			case '\\':
				r+='\\';
				break;
			case '/':
				r+='/';
				break;
			case 'b':
				r+='\b';
				break;
			case 'f':
				r+='\f';
				break;
			case 'n':
				r+='\n';
				break;
			case 'r':
				r+='\r';
				break;
			case 't':
				r+='\t';
				break;
			case 'u':
				p1=json_value::hex_to_int(s.substr(i+2,4));
				if (p1>>10==54){
					r+=(char)((p1%1024)*1024+(json_value::hex_to_int(s.substr(i+8,4))%1024));
				}
				break;
			default:
				r+=s[i];
				break;
			}
			i++;
		}
		else r+=s[i];
	}
	return r;
}
json_value::json_value(){
	val_type=0;
	val_int=0;
}
json_value::json_value(int n){
	val_type=0;
	val_int=n;
}
json_value::json_value(std::string s){
	val_type=1;
	val_string=s;
}
json_value::json_value(const char* s){
	val_type=1;
	val_string=s;
}
json_value::json_value(std::vector<json_value> l){
	val_type=2;
	val_list=l;
}
json_value::json_value(json_object<json_value> o){
	val_type=3;
	val_object=o;
}
json_value::json_value(bool n){
	val_type=4;
	val_int=n;
}
json_value::json_value(double d){
	val_type=5;
	val_float=d;
}
json_value::json_value(long long n){
	val_type=6;
	val_llint=n;
}
std::string json_value::get_repr(){
	std::string output="";
	if (this->val_type==0) output=std::to_string(this->val_int);
	else if (this->val_type==1) {
		output="\"";
		int l=this->val_string.length(),i;
		char escape[7];
		for (i=0;i<l;i++){
			switch (this->val_string[i]){
				case '\b':
					output+="\\b";
					break;
				case '\f':
					output+="\\f";
					break;
				case '\n':
					output+="\\n";
					break;
				case '\r':
					output+="\\r";
					break;
				case '\t':
					output+="\\t";
					break;
				case '\v':
					output+="\\v";
					break;
				case '\"':
					output+="\\\"";
					break;
				case '\\':
					output+="\\\\";
					break;
				default:
					if (this->val_string[i]>0&&this->val_string[i]<32){
						sprintf(escape,"\\u%04x",this->val_string[i]);
						output+=escape;
					}
					else output+=this->val_string[i];
					break;
			}
		}
		output+="\"";
	}
	else if (this->val_type==2){
		int l=this->val_list.size(),i;
		output="[\n";
		for (i=0;i<l;i++){
			output+=this->val_list[i].get_repr();
			if (i<l-1) output+=",\n";
		}
		output+="\n]";
	}
	else if (this->val_type==3){
		int o=this->val_object.object_pair.size(),i=0;
		output="{\n";
		std::string key;
		json_value val;
		for (auto kvp:val_object.object_pair){
			key = kvp.first;
			val = kvp.second;
			output+=json_value(key).get_repr();
			output+=": ";
			output+=val.get_repr();
			if (i<o-1) output+=",\n";
			i++;
		}
		output+="\n}";
	}
	else if (this->val_type==4){
		switch(val_int){
			case -1:
				output="null";
				break;
			case 0:
				output="false";
				break;
			case 1:
				output="true";
		}
	}
	else if (this->val_type==5){
		std::ostringstream scc;
		scc << val_float;
		output=scc.str();
	}
	else if (this->val_type==6) output=std::to_string(this->val_llint);
	output=indent_json(output);
	return output;
}
/*
A JSON parsing mini-function made by neither JustAsh nor Zarkmend.
Refer to RFC 7159 if you want to know more about what this is.
I mean it doesn't work on huge numbers but it gets the job done.
(as long as Discord doesn't chuck in some impossibly large number)
*/
json_value json_value::parse(std::string s){
	json_value obj=json_value();
	std::string to_parse;
	bool is_parsing;
	s=json_value::strip(s);
	int i,l=s.length(),in_list=0,in_object=0;
	bool in_string=0;
	if (s[0]=='{'){
		std::string name_parse;
		bool is_parsing_name=1;
		obj.val_type=3;
		obj.val_object=json_object<json_value>{};
		for (i=1;i<l-1;i++){
			if (s[i]=='"') in_string=!in_string; // Double quotes identify a std::string
			if (s[i]=='\\'){ // Escape character, skip a character
				if (is_parsing_name) name_parse+='\\';
				else to_parse+='\\';
				i++;
			}
			if (s[i]=='{') in_object++;
			if (s[i]=='}') in_object--;
			if (s[i]=='[') in_list++;
			if (s[i]==']') in_list--;
			if (s[i]==':'&&in_object==0&&in_string==0&&is_parsing_name){
				is_parsing_name=0;
				continue;
			}
			if (s[i]==','&&in_object==0&&in_list==0&&in_string==0){
				obj.val_object[json_value::parse(name_parse).val_string]=json_value::parse(to_parse);
				name_parse="";
				to_parse="";
				is_parsing_name=1;
				continue;
			}
			if (is_parsing_name) name_parse+=s[i];
			else to_parse+=s[i];
		}
		obj.val_object[json_value::parse(name_parse).val_string]=json_value::parse(to_parse);
	}
	if (s[0]=='['){
		obj.val_type=2;
		obj.val_list=std::vector<json_value>{};
		for (i=1;i<l-1;i++){
			if (s[i]=='"') in_string=!in_string; // Double quotes identify a std::string
			if (s[i]=='\\'){ // Escape character, skip a character
				to_parse+='\\';
				i++;
			}
			if (s[i]=='{') in_object++;
			if (s[i]=='}') in_object--;
			if (s[i]=='[') in_list++;
			if (s[i]==']') in_list--;
			if (s[i]==','&&in_object==0&&in_list==0){
				obj.val_list.push_back(json_value::parse(to_parse));
				to_parse="";
				continue;
			}
			to_parse+=s[i];
		}
		if (to_parse!="") obj.val_list.push_back(json_value::parse(to_parse));
	}
	if (s[0]=='\"'){
		obj.val_type=1;
		obj.val_string=json_value::unescapify(s.substr(1,s.length()-2));
	}
	std::regex pac_int("(-)?(0|[1-9][0-9]*)(\\.[0-9]+)?([eE][-+]?[0-9]+)?");
	std::smatch res_pac;
	if (std::regex_match(s,res_pac,pac_int)){
		if (res_pac[3].matched||res_pac[4].matched){
			obj.val_type=5;
			obj.val_float=stod(s);
		}
		else{
			obj.val_type=6;
			obj.val_llint=stoll(s);
			// Normal integer limits
			if (obj.val_llint<=2147483647ll&&obj.val_llint>=-2147483648ll){
				obj.val_type=0;
				obj.val_int=(int)obj.val_llint;
			}
		}
	}
	if (s=="null"){
		obj.val_type=4;
		obj.val_int=-1;
	}
	if (s=="true"){
		obj.val_type=4;
		obj.val_int=1;
	}
	if (s=="false"){
		obj.val_type=4;
		obj.val_int=0;
	}
	return obj;
}

long long json_value::lick(){
	switch (this->val_type){
		case 0:
			return this->val_int;
			break;
		case 5:
			return this->val_float;
			break;
		default:
			return this->val_llint;
	}
}

double json_value::ding(){
	switch (this->val_type){
		case 0:
			return this->val_int;
			break;
		case 6:
			return this->val_llint;
			break;
		default:
			return this->val_float;
	}
}