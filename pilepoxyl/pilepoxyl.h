#include <libwebsockets.h>
#include <string.h>
#include <signal.h>
#include <string>
#include <thread>
// The actual JSON library doesn't use maps, but this will help with function arguments
#include <map>
#include <curl/curl.h>
#include "../generate_libs/make_json.h"
#include "../generate_libs/file_management.h"
#include "../generate_libs/generate_libs.h"
// This should work with most POSIX systems + win32
// I will only support Linux for now, though
#ifdef _WIN32
#include <windows.h>
#else
#include <time.h>
#endif

#ifndef _WIN32
void Sleep(int dwMilliseconds);
#endif

// Responses in curl (and libwebsockets) are sent in this format
struct curl_response{
	char* data;
	size_t size;
};

/**
 * A class for REST-based API functions using curl
 * 
 * I prefer using the synchronous "easy" interface to the
 * more advanced "multi" interface as it is synchronous
 * and (well) easier to use
 */
class restapi{
	private:
	void deferred_respond_interaction(std::string link);
	public:
	// Connect there
	std::string url_base;
	// Use this to authorize connections
	std::string bot_token;
	// Splash messages on command responses
	std::map<std::string,std::string> notifmsg;
	// A typical curl write callback
	static size_t write_callback(char* data,size_t size,size_t dsize, void* userdata);
	static size_t dummy_write_callback(char* data,size_t size,size_t dsize, void* userdata);
	static std::vector<json_value> make_help_with_selection(std::string hpcid="", int current_pos=0, int max_pos=0);
	static std::pair<std::string,int> parse_hpcid(std::string hpcid);
	static json_value reduce_options(json_value options);
	std::string send_message(std::string channel_id,std::string content);
	std::string fetch(std::string url);
	bstring raw_fetch(std::string url);
	std::pair<std::string,std::string> identify_filetype(std::string content_type);
	void upload_file_df(std::string url, std::string filename, std::string lclfname, std::string msgcont);
	void upload_data_bf(std::string url, std::string filename,     bstring filedata, std::string msgcont);
	void upload_data_df(std::string url, std::string filename, std::string filedata, std::string msgcont);
	void respond_interaction(json_value payload);
};

/**
 * A class for WebSocket-based API functions using libwebsockets
 */
class gateway{
	public:
	struct lws_context_creation_info info;
	struct lws_client_connect_info cc_info;
	struct lws_context* context;
	// Heartbeat interval obtained from the Hello (opcode 10) payload
	int heartbeat_interval;
	// Event order used to send heartbeats and resume sessions
	int event_order;
	// Received Opcode
	int received_opcode;
	// If heartbeats should be continued
	bool sending_heartbeats;
	// Determines if the payload is completely received
	bool new_payload;
	// For payloads larger than 4096 bytes, Offload it into a chunk
	bool new_chunk;
	// Determines if the application reconnects or terminates itself
	bool can_reconnect;
	// Sometimes Gateway events require HTTP handling
	restapi http_handler;
	// Configuration file
	std::map<std::string,std::string> config;
	// Opcodes
	enum opcodes{
		GW_DISPATCH, // An event was dispatched.
		GW_HRTBTSND, // Fired periodically by the client to keep the connection alive.
		GW_IDENTIFY, // Starts a new session during the initial handshake.
		GW_PRESCUPD, // Update the client's presence.
		GW_VOISTUPD, // Used to join/leave or move between voice channels.
		GW_USRDEFED, // Not sure what this is for.
		GW_DORESUME, // Resume a previous session that was disconnected.
		GW_RECONNEK, // You should attempt to reconnect and resume immediately.
		GW_REQGMEMB, // Request information about offline guild members in a large guild.
		GW_INVLSESS, // The session has been invalidated. You should reconnect and identify/resume accordingly.
		GW_SAYHELLO, // Sent immediately after connecting, contains the [heartbeat_interval] to use.
		GW_HRTBTACK  // Sent in response to receiving a heartbeat to acknowledge that it has been received.
	};
	// Close error codes
	enum cecodes{
		GC_NOSTATUS=0   , // We're not sure what went wrong. Try reconnecting?
		GC_HRTBTSKP=1000, // You forgot to send a heartbeat. Don't do that!
		GC_GOINGAWY=1001, // You forgot to disconnect on opcode 7.
		GC_UNKWNERR=4000, // We're not sure what went wrong. Try reconnecting?
		GC_UNKWNOPC=4001, // You sent an invalid Gateway opcode or an invalid payload for an opcode. Don't do that!
		GC_DECODERR=4002, // You sent an invalid payload to Discord. Don't do that!
		GC_NOTAUTHD=4003, // You sent us a payload prior to identifying.
		GC_AUFAILED=4004, // The account token sent with your identify payload is incorrect.
		GC_ALRDAUTH=4005, // You sent more than one identify payload. Don't do that!
		GC_USRDEFED=4006, // Not sure what this is for.
		GC_INVLDSEQ=4007, // The sequence sent when resuming the session was invalid. Reconnect and start a new session.
		GC_RATLIMIT=4008, // Woah nelly! You're sending payloads to us too quickly. Slow it down! You will be disconnected on receiving this.
		GC_SESTMOUT=4009, // Your session timed out. Reconnect and start a new one.
		GC_INVLDSRD=4010, // You sent us an invalid shard when identifying.
		GC_SHRDREQD=4011, // The session would have handled too many guilds - you are required to shard your connection in order to connect.
		GC_INVLDAPI=4012, // You sent an invalid version for the gateway.
		GC_INVLDINT=4013, // You sent an invalid intent for a Gateway Intent. You may have incorrectly calculated the bitwise value.
		GC_DSALWINT=4014  // You sent a disallowed intent for a Gateway Intent. You may have tried to specify an intent that you have not enabled or are not approved for.
	};
	// libwebsockets response chunks accumulate here
	struct curl_response gateway_rcol;
	// libwebsockets interface
	struct lws* interface;
	// Other than saying hello, this function serves as a Gateway callback
	static int say_hello(struct lws* intf,enum lws_callback_reasons reason,void* user,void* message,size_t len);
	// Always "myrobod", a reference to 20w14∞ (JE)
	static const lws_protocols protocols[];
	// Wait for 5ms and handles the 4096*x bytes case
	void chunk_timeout();
	// The current received message
	json_value received_message;
	// The URL used to resume, obtained from the READY payload
	std::string reconnect_url;
	// Session ID used to resume
	std::string sessid;
	// The function name is self-explanatory
	void determine_what_to_do(json_value json_msg);
	// Connect to the server, handles reconnects and handle server callbacks
	void connect_and_receive_events();
	// Loops the heartbeat send duration
	void send_heartbeat_loop();
	// Initialize a reconnect session
	int reconnect_init();
	// Send a heartbeat payload
	int send_heartbeat();
	// Initialize a new session
	int initialize(std::string config, std::string notifmsg);
};

/**
 * A class for first-run initialization, with the following features:
 * 
 * (+) User prompt
 * (+) Bot token setup
 * (+) Command installation
 * (+) Auto-fetch Gateway address
*/
class firstrun{
	private:
	void install_one_command(std::string command_name);
	public:
	std::string bot_token;
	std::string bot_id;
	std::string gwconnurl;
	void fetch_id();
	void install_commands();
	void set_btk();
	void fetch_gw_addr();
	void export_json();
};