#include "pilepoxyl/pilepoxyl.h"

int main(){
	std::ifstream fr("FIRSTRUN");
	if (fr.is_open()){
		firstrun cfgw;
		cfgw.set_btk();
		cfgw.fetch_id();
		if (!cfgw.bot_id.length()){
			std::cout << "Not a valid bot token. Exiting...\n";
			return 1;
		}
		cfgw.install_commands();
		cfgw.export_json();
		remove("FIRSTRUN");
		std::cout << "All done!\nFrom now on, logs are sent using the format used by libwebsockets\n";
	}

	file cfg;
	cfg.read_file("config/config.json",false);

	file ntf;
	ntf.read_file("config/notifmsg.json",false);

	gateway c;
	c.initialize(cfg.text_contents,ntf.text_contents);
	c.connect_and_receive_events();
	return 0;
}