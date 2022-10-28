#include "pilepoxyl.h"

// Helper functions to initialize, install commands and more

void firstrun::set_btk(){
	std::cout << "As this is your first time running the program, a bot token will be required.\n";
	std::cout << "You will only be asked to enter it once, and the information will be stored in config/config.json\n";
	std::cout << "Your bot's bot token: ";
	std::cin  >> this->bot_token;
}

void firstrun::install_one_command(std::string command_name){
	file comf;
	comf.read_file("commands/"+command_name+".json");

	std::cout << "Installing command " << command_name << "\n";

	CURL* handle=curl_easy_init();
	curl_easy_setopt(handle,CURLOPT_URL,("https://discord.com/api/v10/applications/"+this->bot_id+"/commands").c_str());
	char* auth_header=(char*) malloc(150);
	sprintf(auth_header,"Authorization: Bot %s",this->bot_token.c_str());
	curl_slist* headers=curl_slist_append(curl_slist_append(curl_slist_append(nullptr,"User-Agent: DiscordBot (http://0.0.0.0, 0.0.1)"),"Content-Type: application/json; charset=UTF-8"),auth_header);
	curl_easy_setopt(handle,CURLOPT_POST,1);
	curl_easy_setopt(handle,CURLOPT_HTTPHEADER,headers);

	curl_response cr{0};
	curl_easy_setopt(handle,CURLOPT_POSTFIELDSIZE,comf.text_contents.length());
	curl_easy_setopt(handle,CURLOPT_POSTFIELDS,comf.text_contents.c_str());
	curl_easy_setopt(handle,CURLOPT_WRITEFUNCTION,restapi::write_callback);
	curl_easy_setopt(handle,CURLOPT_WRITEDATA,(void*)&cr);
	CURLcode value = curl_easy_perform(handle);
	curl_easy_cleanup(handle);
	if (value!=CURLE_OK){
		puts(curl_easy_strerror(value));
	}
	free(auth_header);
	free(cr.data);
}

void firstrun::fetch_id(){
	CURL* handle=curl_easy_init();
	curl_easy_setopt(handle,CURLOPT_URL,"https://discord.com/api/v10/users/@me");
	char* auth_header=(char*) malloc(150);
	sprintf(auth_header,"Authorization: Bot %s",this->bot_token.c_str());
	curl_slist* headers=curl_slist_append(
		curl_slist_append(
			curl_slist_append(
				nullptr,
				"User-Agent: DiscordBot (http://0.0.0.0, 0.0.1)"
			),
			"Content-Type: application/json; charset=UTF-8"
		),
		auth_header
	);
	curl_response cr{0};
	curl_easy_setopt(handle,CURLOPT_HTTPHEADER,headers);
	curl_easy_setopt(handle,CURLOPT_WRITEFUNCTION,restapi::write_callback);
	curl_easy_setopt(handle,CURLOPT_WRITEDATA,(void*)&cr);
	CURLcode value = curl_easy_perform(handle);
	curl_easy_cleanup(handle);
	if (value!=CURLE_OK){
		puts(curl_easy_strerror(value));
	}
	json_value jdata=json_value::parse(std::string(cr.data));
	this->bot_id=jdata.val_object["id"].val_string;

	free(auth_header);
	free(cr.data);
}

void firstrun::install_commands(){
	std::vector available={"obfuscator","manifest","item","entity","help"};
	for (std::string a:available){
		this->install_one_command(a);
	}
}

void firstrun::export_json(){
	json_value cf(json_object<json_value>{});
	cf.val_object["bot_token"]=this->bot_token;
	cf.val_object["gateway_url"]="gateway.discord.gg";
	file cfgfile("config/config.json",cf.get_repr());
	cfgfile.write_file();
}