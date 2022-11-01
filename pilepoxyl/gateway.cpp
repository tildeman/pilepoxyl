#include "pilepoxyl.h"

// Definiton of a Windows function for UNIX-like systems
#ifndef _WIN32
void Sleep(int dwMilliseconds){
	struct timespec hbits;
	hbits.tv_sec=dwMilliseconds/1000;
	hbits.tv_nsec=(dwMilliseconds-hbits.tv_sec*1000)*1000000;
	nanosleep(&hbits,nullptr);
}
#endif

// Gateway Interface

const lws_protocols gateway::protocols[]={
	{
		"myrobod",
		gateway::say_hello,
		sizeof(gateway*),0,0,nullptr,0
	},
	LWS_PROTOCOL_LIST_TERM
};

// I think I figured this out:
// (1) LWS will allocate some bytes for the user data
// (2) The user has the size of a normal pointer (8 bytes) 
// (3) The user is a reference to a reference
// (4) That reference points to the current instance
int gateway::say_hello(struct lws* intf,enum lws_callback_reasons reason,void* user,void* message,size_t len){
	const char* strmessage=(const char*)message;
	gateway* that=nullptr; // Pretend the whole function is nonstatic and this value equates to "this"
	if (user!=nullptr) that=*(gateway**)user;
	json_value rcvdata;
	int closecode=0;
	switch(reason){
		case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
			lwsl_err("No 10 for you for now: %s\n",message?strmessage:"???");
			that->interface=nullptr;
			break;
		case LWS_CALLBACK_CLIENT_ESTABLISHED:
			lwsl_user("Connection establised, let's listen!");
			break;
		case LWS_CALLBACK_WS_PEER_INITIATED_CLOSE:
			closecode=((unsigned char)strmessage[0]<<8)+(unsigned char)strmessage[1];
			lwsl_warn("Connection is closing with close code %d",closecode);
			switch(closecode){
				case GC_AUFAILED:
					lwsl_err("Failed to authenticate. Perhaps double-check your bot token?");
					that->can_reconnect=false;
					break;
			}
			break;
		case LWS_CALLBACK_CLIENT_RECEIVE:
			rcvdata=json_value::parse(std::string(strmessage));

			switch(rcvdata.val_object["op"].val_int){
				case GW_RECONNEK: // Reconnect immediately
					lwsl_user("Received response with opcode 7");
					lwsl_user("Reconnect required. Restarting connection.");
					lws_close_reason(intf,LWS_CLOSE_STATUS_NOSTATUS,nullptr,0ul);
					closecode=7; // Make the bot go offline as soon as the request is acknowledged
					break;
				case GW_INVLSESS: // Invalid session, let's party!
					lwsl_user("Received response with opcode 9");
					lwsl_err("Cannot reconnect and resume session.");
					that->can_reconnect=rcvdata.val_object["d"].val_int;
					closecode=9;
					break;
				default:
					if (len==4096){
						that->gateway_rcol.data=(char*)realloc(that->gateway_rcol.data,that->gateway_rcol.size+4097);
						memcpy(that->gateway_rcol.data+that->gateway_rcol.size,message,4096);
						that->gateway_rcol.size+=4096;
						that->gateway_rcol.data[that->gateway_rcol.size]=0;
						that->new_chunk=true;
					}
					else if (that->new_chunk){
						that->gateway_rcol.data=(char*)realloc(that->gateway_rcol.data,that->gateway_rcol.size+len+1);
						memcpy(that->gateway_rcol.data+that->gateway_rcol.size,message,len);
						that->gateway_rcol.size+=len;
						that->gateway_rcol.data[that->gateway_rcol.size]=0;

						rcvdata=json_value::parse(std::string(that->gateway_rcol.data,that->gateway_rcol.size));
						that->received_message=rcvdata;
						free(that->gateway_rcol.data);
						that->gateway_rcol.data=(char*)calloc(1,1);
						that->gateway_rcol.size=0ul;
						that->new_chunk=false;
						that->new_payload=true;
					}
					else{
						that->received_message=rcvdata;
						that->new_payload=true;
					}
			}
			break;
		case LWS_CALLBACK_CLIENT_CLOSED:
			lwsl_user("Connection closed, perhaps reconnect?");
			that->interface=nullptr;
			break;
		// default:
		// 	lwsl_user("Unknown opcode %d",reason);
	}
	return closecode;
}

int gateway::initialize(std::string config, std::string notifmsg){
	// Convert all given parameters to std::map format
	json_value jvcfg=json_value::parse(config);
	json_value jvntf=json_value::parse(notifmsg);

	this->config={
		{"bot_token", ""},
		{"gateway_url", "gateway.discord.gg"}
	};
	this->http_handler.notifmsg={
		{"obfuscator_splash", ""},
		{"obfuscator_noutf8", ""},
		{"manifest_splash", ""},
		{"manifest_ivpico", ""},
		{"manifest_nonext", ""},
		{"item_step1_notif", ""},
		{"item_step2_notif", ""},
		{"item_stepg_ivfmt", ""},
		{"item_step3_splas", ""},
		{"item_step3_imger", ""},
		{"item_stepg_nonex", ""},
		{"entity_textr_ivfmt", ""},
		{"entity_model_ivfmt", ""},
		{"entity_invalid_idr", ""},
		{"entity_bb_disabled", ""},
		{"missing_command", ""},
	};

	std::string key;
	json_value valu;
	for (auto kvl:jvcfg.val_object.object_pair){
		key = kvl.first;
		valu = kvl.second;
		this->config[key]=valu.val_string;
	}
	for (auto kvl:jvntf.val_object.object_pair){
		key = kvl.first;
		valu = kvl.second;
		this->http_handler.notifmsg[key]=valu.val_string;
	}

	// Actually start the initialization
	this->sending_heartbeats=false;
	this->can_reconnect=false;
	this->new_payload=false;
	this->new_chunk=false;
	this->event_order=0;
	this->received_message="";
	curl_global_init(CURL_GLOBAL_ALL);
	int loglevel=LLL_USER|LLL_ERR|LLL_WARN|LLL_NOTICE;
	lws_set_log_level(loglevel,nullptr);
	// Zero-initialize everything
	memset(&this->info,0,sizeof(this->info));
	memset(&this->cc_info,0,sizeof(this->cc_info));
	// Make a context
	this->info.options=LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;
	this->info.port=CONTEXT_PORT_NO_LISTEN;
	this->info.protocols=gateway::protocols;
	this->info.fd_limit_per_thread=3;
	this->context=lws_create_context(&this->info);
	if (!this->context){
		lwsl_err("Something happened");
		return 9001;
	}
	// Make a connection
	this->cc_info.context=this->context;
	this->cc_info.port=443;
	lwsl_user("Connecting to %s",this->config["gateway_url"].c_str());
	this->cc_info.address=this->config["gateway_url"].c_str();
	this->cc_info.path="/";
	this->cc_info.host=this->cc_info.address;
	this->cc_info.origin=this->cc_info.address;
	this->cc_info.ssl_connection=LCCSCF_USE_SSL;
	this->cc_info.protocol="myrobod";
	this->cc_info.pwsi=&this->interface;
	return 0;
}

int gateway::reconnect_init(){
	this->sending_heartbeats=false;
	this->new_payload=false;
	this->event_order=0;
	this->received_message="";
	// Zero-initialize everything
	memset(&this->info,0,sizeof(this->info));
	memset(&this->cc_info,0,sizeof(this->cc_info));
	// Make a context
	this->info.options=LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;
	this->info.port=CONTEXT_PORT_NO_LISTEN;
	this->info.protocols=gateway::protocols;
	this->info.fd_limit_per_thread=3;
	this->context=lws_create_context(&this->info);
	if (!this->context){
		lwsl_err("Something happened");
		return 9001;
	}
	// Make a connection
	this->cc_info.context=this->context;
	this->cc_info.port=443;
	this->cc_info.address=this->reconnect_url.c_str();
	this->cc_info.path="/";
	this->cc_info.host=this->cc_info.address;
	this->cc_info.origin=this->cc_info.address;
	this->cc_info.ssl_connection=LCCSCF_USE_SSL;
	this->cc_info.protocol="myrobod";
	this->cc_info.pwsi=&this->interface;
	return 0;
}

int gateway::send_heartbeat(){
	lwsl_user("Sending response with opcode 1");
	char payload[100];
	payload[LWS_PRE]=0;
	sprintf(payload+LWS_PRE,"{\"op\":1,\"d\":%d}",this->event_order);
	int msg=lws_write(interface,((unsigned char*) payload+LWS_PRE),strlen(payload+LWS_PRE),LWS_WRITE_TEXT);
	return msg;
}

void gateway::send_heartbeat_loop(){
	while (1){
		Sleep(this->heartbeat_interval);
		if (this->interface&&this->sending_heartbeats){
			lwsl_user("%d milliseconds passed, this implies a heartbeat",this->heartbeat_interval);
			this->send_heartbeat();
		}
	}
}

void gateway::connect_and_receive_events(){
	while(1){
		gateway_rcol={0};
		lws_client_connect_via_info(&this->cc_info);
		gateway** that=(gateway**)lws_wsi_user(this->interface);
		*that=this;
		while (this->interface){
			lws_service(this->context,0);
			if (this->new_chunk){
				std::thread nchk(&gateway::chunk_timeout,this);
				nchk.detach();
			}
			if (this->new_payload){
				std::thread dwtd(&gateway::determine_what_to_do,this,this->received_message);
				dwtd.detach();
				this->new_payload=false;
			}
		}
		this->sending_heartbeats=false;
		lws_context_destroy(this->context);
		if (this->can_reconnect){
			this->reconnect_init();
		}
		else{
			break;
		}
	}
}

void gateway::determine_what_to_do(json_value json_msg){
	enum opcodes received_opcode=(enum opcodes) json_msg.val_object["op"].val_int;
	lwsl_user("Received response with opcode %d",received_opcode);
	switch (received_opcode){
		case GW_DISPATCH: // An event is received, do something I guess
			lwsl_user("Payload type: %s",json_msg.val_object["t"].val_string.c_str());
			if (json_msg.val_object["t"].val_string=="READY"){
				this->reconnect_url=json_msg.val_object["d"].val_object["resume_gateway_url"].val_string.substr(6);
				this->sessid=json_msg.val_object["d"].val_object["session_id"].val_string;
			}
			else if (json_msg.val_object["t"].val_string=="INTERACTION_CREATE"){
				this->http_handler.respond_interaction(json_msg.val_object["d"]);
			}
			this->event_order=json_msg.val_object["s"].val_int;
			break;
		case GW_HRTBTSND: // Additional heartbeat is requested
			this->send_heartbeat();
			break;
		case GW_SAYHELLO: // Connection response
			lwsl_user("Hello, Discord!");
			this->heartbeat_interval=json_msg.val_object["d"].val_object["heartbeat_interval"].val_int;
			lwsl_user("Heartbeats will be sent every %.3f seconds",this->heartbeat_interval/1000.0);

			// Send an identify response here
			file idpl;
			idpl.read_file("config/config.json",0);
			json_value configval=json_value::parse(idpl.text_contents);
			this->http_handler.bot_token=configval.val_object["bot_token"].val_string;

			json_value identify_payload=json_value(json_object<json_value>());
			if (this->can_reconnect){
				identify_payload.val_object["op"]=6;
				identify_payload.val_object["d"]=json_object<json_value>();
				identify_payload.val_object["d"].val_object["token"]=this->http_handler.bot_token;
				identify_payload.val_object["d"].val_object["seq"]=this->event_order;
				identify_payload.val_object["d"].val_object["session_id"]=this->sessid;
			}
			else{
				identify_payload.val_object["op"]=2;
				identify_payload.val_object["d"]=json_object<json_value>();
				identify_payload.val_object["d"].val_object["token"]=this->http_handler.bot_token;
				identify_payload.val_object["d"].val_object["intents"]=0;
				identify_payload.val_object["d"].val_object["properties"]=json_object<json_value>();
				identify_payload.val_object["d"].val_object["properties"].val_object["os"]="linux";
				identify_payload.val_object["d"].val_object["properties"].val_object["browser"]="libwebsockets";
				identify_payload.val_object["d"].val_object["properties"].val_object["device"]="libwebsockets";
				identify_payload.val_object["d"].val_object["presence"]=json_object<json_value>();
				identify_payload.val_object["d"].val_object["presence"].val_object["since"]=false;
				identify_payload.val_object["d"].val_object["presence"].val_object["since"].val_int=-1;
				identify_payload.val_object["d"].val_object["presence"].val_object["activities"]=std::vector<json_value>();
				identify_payload.val_object["d"].val_object["presence"].val_object["status"]="online";
				identify_payload.val_object["d"].val_object["presence"].val_object["afk"]=false;
				this->can_reconnect=true;
			}
			lwsl_user("Sending response with opcode %d",identify_payload.val_object["op"].val_int);

			char payload[1000];
			payload[LWS_PRE]=0;
			strcpy(payload+LWS_PRE,identify_payload.get_repr().c_str());
			int msg=lws_write(interface,((unsigned char*) payload+LWS_PRE),strlen(payload+LWS_PRE),LWS_WRITE_TEXT);

			this->sending_heartbeats=true;
			std::thread hbth(&gateway::send_heartbeat_loop, this);
			hbth.detach();
			break;
	}
}

void gateway::chunk_timeout(){
	Sleep(5); // Set the timeout to 5ms
	if (this->new_chunk&&!this->new_payload){// Message is exactly 4096 bytes
		this->new_chunk=false;
		json_value rcvdata=json_value::parse(std::string(this->gateway_rcol.data,this->gateway_rcol.size));
		free(this->gateway_rcol.data);
		this->gateway_rcol.data=(char*)calloc(1,1);
		this->gateway_rcol.size=0ul;
		this->determine_what_to_do(rcvdata);
	}
}