#include "pilepoxyl.h"

// REST Interface

size_t restapi::write_callback(char* data,size_t size,size_t dsize, void* userdata){
	struct curl_response* udata=(struct curl_response*) userdata;
	size_t actual_size=size * dsize;
	udata->data=(char*)realloc(udata->data,udata->size+actual_size+1);
	memcpy(udata->data+udata->size,data,actual_size);
	udata->size+=actual_size;
	udata->data[udata->size]=0;
	return actual_size;
}

size_t restapi::dummy_write_callback(char* data,size_t size,size_t dsize, void* userdata){
	size_t actual_size=size * dsize;
	return actual_size;
}

void restapi::deferred_respond_interaction(std::string link){
	CURL* handle=curl_easy_init();
	
	curl_easy_setopt(handle,CURLOPT_URL,link.c_str());
	std::string auth_header="Authorization: Bot "+this->bot_token;
	curl_slist* headers=curl_slist_append(
		curl_slist_append(
			curl_slist_append(
				nullptr,
				"User-Agent: DiscordBot (http://0.0.0.0, 0.0.1)"
			),
			"Content-Type: application/json; charset=UTF-8"
		),
		auth_header.c_str()
	);
	curl_easy_setopt(handle,CURLOPT_POST,1);
	curl_easy_setopt(handle,CURLOPT_HTTPHEADER,headers);
	std::string data="{\"type\": 5}";
	curl_easy_setopt(handle,CURLOPT_POSTFIELDSIZE,data.length());
	curl_easy_setopt(handle,CURLOPT_POSTFIELDS,data.c_str());
	curl_easy_setopt(handle,CURLOPT_WRITEFUNCTION,restapi::dummy_write_callback);
	CURLcode value = curl_easy_perform(handle);
	curl_easy_cleanup(handle);
	if (value!=CURLE_OK){
		lwsl_err(curl_easy_strerror(value));
	}
}

void restapi::respond_interaction(json_value payload){
	std::string id=payload.val_object["id"].val_string;
	std::string token=payload.val_object["token"].val_string;
	std::string channel_link="https://discord.com/api/v10/interactions/"+id+"/"+token+"/callback";
	std::string auth_header="Authorization: Bot "+this->bot_token;
	json_value data_json(json_object<json_value>{});
	int response_type=payload.val_object["type"].val_int;

	if (response_type==2){
		data_json.val_object["type"]=json_value(4);
		data_json.val_object["data"]=json_value(json_object<json_value>());

		std::string funcname=payload.val_object["data"].val_object["name"].val_string;
		if (funcname=="hello"){
			// Say hello, that's it
			data_json.val_object["data"].val_object["content"]=json_value("Hello!");
		}
		else if (funcname=="obfuscate"){
			std::string value_id=payload.val_object["data"].val_object["options"].val_list[0].val_object["value"].val_string;
			json_value attachment=payload.val_object["data"].val_object["resolved"].val_object["attachments"].val_object[value_id];
			std::string content_type=attachment.val_object["content_type"].val_string;
			std::pair<std::string,std::string> ctbd=this->identify_filetype(content_type);
			if (ctbd.first!="application/json"){
				data_json.val_object["data"].val_object["content"]=this->notifmsg["obfuscator_invfex"];
				data_json.val_object["data"].val_object["flags"]=64; // Error messages should be ephemeral
			}
			else{
				bool minify=false;
				if (payload.val_object["data"].val_object["options"].val_list.size()>=2){
					// Minify option
					minify=payload.val_object["data"].val_object["options"].val_list[1].val_object["value"].val_int;
				}
				std::string cset_warn=this->notifmsg["splash"];
				if (ctbd.second!="utf-8"){
					cset_warn=this->notifmsg["obfuscator_noutf8"];
				}
				std::string json_file=this->fetch(attachment.val_object["url"].val_string);
				generatelibs::obfuscator obf;
				this->upload_data_df(channel_link,attachment.val_object["filename"].val_string,obf.obfuscateMain(json_file,minify),cset_warn);
				return;
			}
		}
		else if (funcname=="manifest"){
			json_value mpload=payload.val_object["data"].val_object["options"].val_list[0];
			std::string sfuncname=mpload.val_object["name"].val_string;
			if (sfuncname=="independent"){
				// Independent manifest file. **useit**
				std::map<std::string,json_value> witdt={
					{"pack_type","data"},
					{"pack_name","manifest"},
					{"pack_description",""},
					{"pack_version","1.17.0"},
					{"pack_use_metadata",false},
					{"pack_authors",""},
					{"pack_author_url",""},
					{"pack_use_gametest",false},
					{"pack_gametest_entry_file","modules/Main.js"}
				};
				std::string key;
				std::string fetch_file;
				std::string msgcntn=this->notifmsg["manifest_splash"];
				// The folder name is only "bp" if witdt.pack_type is data
				std::string folder_name="rp";
				json_value valu;
				file image;
				// Code to analyze parameters goes here

				for (json_value option:mpload.val_object["options"].val_list){
					key=option.val_object["name"].val_string;
					valu=option.val_object["value"];
					if (key=="pack_type"){
						if (valu.val_string=="data") folder_name="bp";
					}
					if (key=="pack_icon"){
						json_value icon_atch=payload.val_object["data"]
							.val_object["resolved"]
							.val_object["attachments"]
							.val_object[valu.val_string];
						if (icon_atch.val_object["content_type"].val_string=="image/png"){
							fetch_file=icon_atch.val_object["url"].val_string;
						}
						else{
							msgcntn=this->notifmsg["manifest_ivpico"]+"\n"+msgcntn;
						}
					}
					else{
						witdt[key]=valu;
					}
				}

				generatelibs::manifest mfg;
				std::string result=mfg.generate_nondependent(
					witdt["pack_type"].val_string,
					witdt["pack_name"].val_string,
					witdt["pack_description"].val_string,
					generatelibs::split_version(witdt["pack_version"].val_string),
					witdt["pack_use_metadata"].val_int,
					generatelibs::split_authors(witdt["pack_authors"].val_string),
					witdt["pack_author_url"].val_string,
					witdt["pack_use_gametest"].val_int,
					witdt["pack_gametest_entry_file"].val_string
				);
				file bundled_manifest("manifest.json",result);
				file combo_pack("",std::vector<file>{file(folder_name,std::vector<file>{bundled_manifest})});
				if (fetch_file!=""){
					image.filename="pack_icon.png";
					image.filetype=0;
					image.binary_contents=this->raw_fetch(fetch_file);
					combo_pack.directory_contents[0].directory_contents.push_back(image);
				}
				std::string mdzd=combo_pack.make_zip();

				this->upload_file_df(channel_link,witdt["pack_name"].val_string+".mcpack",mdzd,msgcntn);
				remove(mdzd.c_str());
			}
			else if (sfuncname=="dependent"){
				// Dependent manifest file. **dontuseit**
				std::map<std::string,json_value> witdt={
					{"bp_name","manifest"},
					{"bp_description",""},
					{"bp_version","1.17.0"},
					{"bp_use_metadata",false},
					{"bp_authors",""},
					{"bp_author_url",""},
					{"bp_use_gametest",false},
					{"bp_gametest_entry_file","modules/Main.js"},
					{"bp_use_scripts",false},
					{"rp_name","manifest"},
					{"rp_description",""},
					{"rp_version","1.17.0"},
					{"rp_use_metadata",false},
					{"rp_authors",""},
					{"rp_author_url",""},
					{"rp_use_ui",false},
				};
				std::string key;
				std::string fetch_file;
				std::string msgcntn=this->notifmsg["manifest_splash"];
				// The folder name is only "bp" if witdt.pack_type is data
				json_value valu;
				file image;
				// Code to analyze parameters goes here

				for (json_value option:mpload.val_object["options"].val_list){
					key=option.val_object["name"].val_string;
					valu=option.val_object["value"];
					if (key=="pack_icon"){
						json_value icon_atch=payload.val_object["data"]
							.val_object["resolved"]
							.val_object["attachments"]
							.val_object[valu.val_string];
						if (icon_atch.val_object["content_type"].val_string=="image/png"){
							fetch_file=icon_atch.val_object["url"].val_string;
						}
						else{
							msgcntn=this->notifmsg["manifest_ivpico"]+"\n"+msgcntn;
						}
					}
					else{
						witdt[key]=valu;
					}
				}

				generatelibs::manifest mfg;
				std::vector<std::string> result=mfg.generate_dependent(
					witdt["bp_name"].val_string,
					witdt["bp_description"].val_string,
					generatelibs::split_version(witdt["bp_version"].val_string),
					witdt["bp_use_scripts"].val_int,
					witdt["bp_use_metadata"].val_int,
					generatelibs::split_authors(witdt["bp_authors"].val_string),
					witdt["bp_author_url"].val_string,
					witdt["rp_name"].val_string,
					witdt["rp_description"].val_string,
					generatelibs::split_version(witdt["rp_version"].val_string),
					witdt["rp_use_metadata"].val_int,
					generatelibs::split_authors(witdt["rp_authors"].val_string),
					witdt["rp_author_url"].val_string,
					witdt["rp_use_ui"].val_int,
					witdt["bp_use_gametest"].val_int,
					witdt["bp_gametest_entry_file"].val_string
				);
				file bp_manifest("manifest.json",result[0]);
				file rp_manifest("manifest.json",result[1]);
				file combo_pack("",std::vector<file>{
					file("rp",std::vector<file>{rp_manifest}),
					file("bp",std::vector<file>{bp_manifest})
				});
				if (fetch_file!=""){
					image.filename="pack_icon.png";
					image.filetype=0;
					image.binary_contents=this->raw_fetch(fetch_file);
					combo_pack.directory_contents[0].directory_contents.push_back(image);
					combo_pack.directory_contents[1].directory_contents.push_back(image);
				}
				std::string mdzd=combo_pack.make_zip();

				this->upload_file_df(channel_link,witdt["rp_name"].val_string+".mcpack",mdzd,msgcntn);
				remove(mdzd.c_str());
			}
			else{
				data_json.val_object["data"].val_object["content"]=this->notifmsg["manifest_nonext"];
			}
			return;
		}
		else if (funcname=="item"){
			json_value mpload=payload.val_object["data"].val_object["options"].val_list[0];
			std::string sfuncname=mpload.val_object["name"].val_string;
			if (sfuncname=="start"){
				json_value step1_json=restapi::reduce_options(mpload.val_object["options"]);
				std::string step1_repr=step1_json.get_repr();
				this->upload_data_df(channel_link,"step1.json",step1_repr,this->notifmsg["item_step1_snotif"]);
				return;
			}
			else if (sfuncname=="checkpoint"){
				json_value step2_json=restapi::reduce_options(mpload.val_object["options"]);
				std::string value_id=step2_json.val_object["step1_file"].val_string;
				json_value step1_atch=payload.val_object["data"].val_object["resolved"].val_object["attachments"].val_object[value_id];
				
				std::string step1_ctyp=step1_atch.val_object["content_type"].val_string;
				std::pair<std::string,std::string> ctbd=this->identify_filetype(step1_ctyp);
				if (ctbd.first=="application/json"){
					std::string step1_data=this->fetch(step1_atch.val_object["url"].val_string);
					json_value step1_json=json_value::parse(step1_data);
					step2_json.val_object.object_pair.insert(
						step2_json.val_object.object_pair.begin(),
						step1_json.val_object.object_pair.begin(),
						step1_json.val_object.object_pair.end()
					);
					std::string step2_repr=step2_json.get_repr();
					this->upload_data_df(channel_link,"step2.json",step2_repr,this->notifmsg["item_step2_snotif"]);
					return;
				}
				else{
					data_json.val_object["data"].val_object["content"]=json_value(this->notifmsg["item_stepg_ivlfmt"]);
					data_json.val_object["data"].val_object["flags"]=64; // Error messages should be ephemeral
				}
			}
			else if (sfuncname=="finish"){
				json_value step3_json=restapi::reduce_options(mpload.val_object["options"]);
				std::string value_id=step3_json.val_object["step2_file"].val_string;
				json_value step2_atch=payload.val_object["data"].val_object["resolved"].val_object["attachments"].val_object[value_id];
				
				std::string step2_ctyp=step2_atch.val_object["content_type"].val_string;
				std::pair<std::string,std::string> ctbd=this->identify_filetype(step2_ctyp);
				if (ctbd.first=="application/json"){
					std::string msgcntn=this->notifmsg["item_step3_splash"];
					std::string step2_data=this->fetch(step2_atch.val_object["url"].val_string);
					json_value step2_json=json_value::parse(step2_data);
					step3_json.val_object.object_pair.insert(
						step3_json.val_object.object_pair.begin(),
						step2_json.val_object.object_pair.begin(),
						step2_json.val_object.object_pair.end()
					);
					std::map<std::string,json_value> witdt={
						{"format_version","1.16.100"}, {"identifier","custom:item"},
						{"category","Items"}, {"is_experimental",false},
						{"texture_path","textures/items/my_item"},
						{"generate_item_atlas",false}, {"display_name","Nice item"},
						{"max_stack_size",0ll}, {"is_enchanted",false},
						{"use_animation","none"}, {"use_duration",32.0},
						{"stacked_by_data",false}, {"hand_equipped",false},
						{"can_destroy_in_creative",true}, {"creative_category","none"},
						{"is_food",false}, {"food_nutrition",-1.0},
						{"food_saturation_modifier","low"}, {"food_can_always_eat",false},
						{"food_using_converts_to",""}, {"is_explodable",true},
						{"allow_in_off_hand",false}, {"wearable_slot","none"},
						{"armor_protection",-1ll}, {"cooldown",false},
						{"cooldown_category",""}, {"cooldown_duration",-1.0},
						{"damage",-1ll}, {"place_block",false}, {"block_to_place",""},
						{"use_block_description",false}, {"place_entity",false},
						{"entity_to_place",""}, {"entity_use_on",""},
						{"entity_dispense_on",""}, {"enchantable",false},
						{"enchantable_slot","none"}, {"enchantable_value",-1ll},
						{"can_shoot",false}, {"shooter_max_draw_duration",0.0},
						{"shooter_charge_on_draw",false},
						{"shooter_scale_power_on_duration",false},
						{"shooter_use_ammo",false},
						{"shooter_ammo_search_in_inventory",false},
						{"shooter_ammo_item_to_use",""},
						{"shooter_ammo_use_offhand",false},
						{"shooter_ammo_use_in_creative",false},
						{"fertilizer_duration",-1ll}, {"ignore_permission",false},
						{"mining_speed",-1ll}, {"liquid_clipped",false},
						{"should_despawn",true}, {"mirrored_art",false},
						{"projectile",false}, {"projectile_entity",false},
						{"projectile_min_crit_power",-1.0}, {"dye_powder",""},
						{"throwable",false}, {"throwable_do_swing_animation",false},
						{"throwable_max_draw_duration",-1.0},
						{"throwable_scale_pow_by_draw_dura",false},
						{"knockback_resistance",-1.0}, {"fuel_duration",-1.0},
						{"record",false}, {"record_sound_event",""},
						{"record_duration",-1ll}, {"record_comparator_signal",false},
						{"item_icon",""}
					}; // 68 parameters to handle, that's going to be tiresome

					file ab("","");

					std::string key;
					json_value valu;
					for (auto kvl:step3_json.val_object.object_pair){
						key = kvl.first;
						valu = kvl.second;
						witdt[key]=valu;
					}

					if (witdt["item_icon"].val_string.length()){
						json_value icon_atch=payload.val_object["data"]
							.val_object["resolved"]
							.val_object["attachments"]
							.val_object[witdt["item_icon"].val_string];
						if (icon_atch.val_object["content_type"].val_string=="image/png"){
							std::string fetch_file=icon_atch.val_object["url"].val_string;
							ab.filename="brillium.png";
							ab.filetype=0;
							ab.binary_contents=this->raw_fetch(fetch_file);
						}
						else{
							msgcntn=this->notifmsg["item_step3_imgerr"]+"\n"+msgcntn;
						}
					}

					generatelibs::item generator;
					
					file target_file=generator.writeItem(
						witdt["format_version"].val_string, witdt["identifier"].val_string,
						witdt["category"].val_string, witdt["is_experimental"].val_int,
						witdt["texture_path"].val_string, witdt["display_name"].val_string,
						witdt["max_stack_size"].lick(), witdt["use_animation"].val_string!="none",
						witdt["use_animation"].val_string, witdt["is_enchanted"].val_int,
						witdt["use_duration"].ding(), witdt["stacked_by_data"].val_int,
						witdt["hand_equipped"].val_int, witdt["can_destroy_in_creative"].val_int,
						witdt["creative_category"].val_string!="none", witdt["creative_category"].val_string,
						witdt["is_food"].val_int, witdt["food_nutrition"].ding(),
						witdt["food_saturation_modifier"].val_string, witdt["food_can_always_eat"].val_int,
						witdt["food_using_converts_to"].val_string, witdt["is_explodable"].val_int,
						witdt["allow_in_off_hand"].val_int, witdt["wearable_slot"].val_string!="none",
						witdt["wearable_slot"].val_string, witdt["armor_protection"].lick(),
						witdt["cooldown"].val_int, witdt["cooldown_category"].val_string,
						witdt["cooldown_duration"].ding(), witdt["damage"].lick(), witdt["place_block"].val_int,
						witdt["block_to_place"].val_string, witdt["use_block_description"].val_int,
						witdt["place_entity"].val_int, witdt["entity_to_place"].val_string,
						generatelibs::split_authors(witdt["entity_use_on"].val_string),
						generatelibs::split_authors(witdt["entity_dispense_on"].val_string),
						witdt["enchantable"].val_int, witdt["enchantable_slot"].val_string,
						witdt["enchantable_value"].lick(), witdt["can_shoot"].val_int,
						witdt["shooter_max_draw_duration"].ding(), witdt["shooter_charge_on_draw"].val_int,
						witdt["shooter_scale_power_on_duration"].val_int, witdt["shooter_use_ammo"].val_int,
						witdt["shooter_ammo_search_in_inventory"].val_int, witdt["shooter_ammo_item_to_use"].val_string,
						witdt["shooter_ammo_use_offhand"].val_int, witdt["shooter_ammo_use_in_creative"].val_int,
						witdt["fertilizer_duration"].lick(), witdt["ignore_permission"].val_int,
						witdt["mining_speed"].lick(), witdt["liquid_clipped"].val_int, witdt["should_despawn"].val_int,
						witdt["mirrored_art"].val_int, witdt["projectile"].val_int,
						witdt["projectile_entity"].val_string, witdt["projectile_min_crit_power"].ding(),
						witdt["dye_powder"].val_string!="none", witdt["dye_powder"].val_string,
						witdt["throwable"].val_int, witdt["throwable_do_swing_animation"].val_int,
						witdt["throwable_max_draw_duration"].ding(), witdt["throwable_scale_pow_by_draw_dura"].val_int,
						witdt["knockback_resistance"].ding(), witdt["fuel_duration"].ding(), witdt["record"].val_int,
						witdt["record_sound_event"].val_string, witdt["record_duration"].lick(),
						witdt["record_comparator_signal"].lick(), ab, witdt["generate_item_atlas"].val_int
					);
					std::string fname=generator.get_iname(witdt["identifier"].val_string);
					if (!fname.length()) fname="item";
					std::string tfname=target_file.make_zip();

					this->upload_file_df(channel_link,fname+".mcaddon",tfname,msgcntn);
					remove(tfname.c_str());
					return;
				}
				else{
					data_json.val_object["data"].val_object["content"]=this->notifmsg["item_stepg_ivlfmt"];
					data_json.val_object["data"].val_object["flags"]=64; // Error messages should be ephemeral
				}
			}
			else{
				data_json.val_object["data"].val_object["content"]=this->notifmsg["item_stepg_nonext"];
			}
		} // That code was insanely long, I'm relieved to have completed that monstrosity
		else if (funcname=="entity"){
			std::map<std::string,json_value> witdt={
				{"generate",0},
				{"identifier",""},
				{"texture_file",""},
				{"model_file",""},
				{"is_spawnable",true},
				{"is_experimental",false},
				{"is_summonable",false},
				{"basic_beh",true}, // Otherwise disastrous consequences
				{"spawn_egg_overlay","#563d7c"},
				{"spawn_egg_base","#21367c"},
				{"material","entity_alphatest"}
			};
			json_value mpload=payload.val_object["data"];
			std::string key,msgcntn;
			json_value valu;
			for (json_value option:mpload.val_object["options"].val_list){
				key=option.val_object["name"].val_string;
				valu=option.val_object["value"];
				witdt[key]=valu;
			}
			generatelibs::entity generator;
			file tf("","");
			file mf("","");


			if (witdt["texture_file"].val_string.length()){
				json_value icon_atch=payload.val_object["data"]
					.val_object["resolved"]
					.val_object["attachments"]
					.val_object[witdt["texture_file"].val_string];
				if (icon_atch.val_object["content_type"].val_string=="image/png"){
					std::string fetch_file=icon_atch.val_object["url"].val_string;
					tf.filename=icon_atch.val_object["filename"].val_string;
					tf.filetype=0;
					tf.binary_contents=this->raw_fetch(fetch_file);
				}
				else{
					msgcntn=this->notifmsg["entity_textr_ivfmt"]+"\n"+msgcntn;
				}
			}
			if (witdt["model_file"].val_string.length()){
				json_value icon_atch=payload.val_object["data"]
					.val_object["resolved"]
					.val_object["attachments"]
					.val_object[witdt["model_file"].val_string];
				if (this->identify_filetype(icon_atch.val_object["content_type"].val_string).first=="application/json"){
					std::string fetch_file=icon_atch.val_object["url"].val_string;
					mf.filename=icon_atch.val_object["filename"].val_string;
					mf.text_contents=this->fetch(fetch_file);
				}
				else{
					msgcntn=this->notifmsg["entity_model_ivfmt"]+"\n"+msgcntn;
				}
			}

			file januszobj=generator.make(
				witdt["identifier"].val_string,
				tf, mf,
				witdt["is_spawnable"].val_int,
				witdt["is_summonable"].val_int,
				witdt["is_experimental"].val_int,
				witdt["spawn_egg_overlay"].val_string,
				witdt["spawn_egg_base"].val_string,
				witdt["material"].val_string,
				witdt["generate"].val_string
			);
			if (!(witdt["basic_beh"].val_int)){
				data_json.val_object["data"].val_object["content"]=this->notifmsg["entity_bb_disabled"];
				data_json.val_object["data"].val_object["flags"]=64; // Error messages should be ephemeral
			}
			else if (januszobj.filename!=""){
				std::string jfname=januszobj.filename;
				std::string tfname=januszobj.make_zip();

				this->upload_file_df(channel_link,jfname,tfname,msgcntn);
				remove(tfname.c_str());
				return;
			}
			else{
				data_json.val_object["data"].val_object["content"]=this->notifmsg["entity_invalid_idr"];
				data_json.val_object["data"].val_object["flags"]=64; // Error messages should be ephemeral
			}
		} // And the homie set is done
		else if (funcname=="help"){
			// Create an embed first
			data_json.val_object["data"].val_object["embeds"]=std::vector<json_value>();
			data_json.val_object["data"].val_object["embeds"].val_list.push_back(json_object<json_value>());
			data_json.val_object["data"].val_object["embeds"].val_list[0].val_object["title"]="Creator Tools for Minecraft Bedrock Edition";
			data_json.val_object["data"].val_object["embeds"].val_list[0].val_object["description"]="**Not** made by PixelPoly";
			data_json.val_object["data"].val_object["embeds"].val_list[0].val_object["color"]=16777215; // Decimal numbers the best
			
			data_json.val_object["data"].val_object["components"]=restapi::make_help_with_selection();
		}
		else{
			data_json.val_object["data"].val_object["content"]=this->notifmsg["missing_command"];
		}
	}
	else if (response_type==3){
		data_json.val_object["type"]=json_value(7); // The original message is edited
		data_json.val_object["data"]=json_value(json_object<json_value>());
		std::string funcname=payload.val_object["data"].val_object["custom_id"].val_string;
		if (funcname=="help_commands"){
			std::string selection=payload.val_object["data"].val_object["values"].val_list[0].val_string;
			file help_metadata;
			help_metadata.read_file("data/"+selection+"_0.json",false);
			json_value hmjson=json_value::parse(help_metadata.text_contents);
			int max_pages=hmjson.val_object["length"].val_int;
			file help_data;
			help_data.read_file("data/"+selection+"_1.json",false);
			data_json.val_object["data"]=json_value::parse(help_data.text_contents);

			data_json.val_object["data"].val_object["embeds"].val_list[0].val_object["color"]=hmjson.val_object["color"];
			data_json.val_object["data"].val_object["embeds"].val_list[0].val_object["footer"]=json_object<json_value>();
			data_json.val_object["data"].val_object["embeds"].val_list[0].val_object["footer"].val_object["text"]="Page 1/"+std::to_string(max_pages);
			data_json.val_object["data"].val_object["components"]=restapi::make_help_with_selection(selection,1,max_pages);
		}
		else if (funcname.substr(0ul,3ul)=="hp_"){
			std::pair<std::string,int> cid=restapi::parse_hpcid(payload.val_object["data"].val_object["custom_id"].val_string.substr(3ul));
			file help_metadata;
			help_metadata.read_file("data/"+cid.first+"_0.json",false);
			json_value hmjson=json_value::parse(help_metadata.text_contents);
			int max_pages=hmjson.val_object["length"].val_int;
			if (cid.second==2147483646){
				cid.second=1;
			}
			else if (cid.second==2147483647){
				cid.second=max_pages;
			}
			file help_data;
			help_data.read_file("data/"+cid.first+"_"+std::to_string(cid.second)+".json",false);
			data_json.val_object["data"]=json_value::parse(help_data.text_contents);

			data_json.val_object["data"].val_object["embeds"].val_list[0].val_object["color"]=hmjson.val_object["color"];
			data_json.val_object["data"].val_object["embeds"].val_list[0].val_object["footer"]=json_object<json_value>();
			data_json.val_object["data"].val_object["embeds"].val_list[0].val_object["footer"].val_object["text"]="Page "+std::to_string(cid.second)+"/"+std::to_string(max_pages);
			data_json.val_object["data"].val_object["components"]=restapi::make_help_with_selection(cid.first,cid.second,max_pages);
		}
	}

	CURL* handle=curl_easy_init();
	curl_easy_setopt(handle,CURLOPT_URL,channel_link.c_str());
	curl_slist* headers=curl_slist_append(
		curl_slist_append(
			curl_slist_append(
				nullptr,
				"User-Agent: DiscordBot (http://0.0.0.0, 0.0.1)"
			),
			"Content-Type: application/json; charset=UTF-8"
		),
		auth_header.c_str()
	);
	curl_easy_setopt(handle,CURLOPT_POST,1);
	curl_easy_setopt(handle,CURLOPT_HTTPHEADER,headers);
	std::string data=data_json.get_repr();

	curl_easy_setopt(handle,CURLOPT_POSTFIELDSIZE,data.length());
	curl_easy_setopt(handle,CURLOPT_POSTFIELDS,data.c_str());
	
	curl_easy_setopt(handle,CURLOPT_WRITEFUNCTION,restapi::dummy_write_callback);
	CURLcode value = curl_easy_perform(handle);
	curl_easy_cleanup(handle);
	if (value!=CURLE_OK){
		lwsl_err(curl_easy_strerror(value));
	}
}

void restapi::upload_data_df(std::string url,std::string filename,std::string filedata,std::string msgcont){
	CURL* handle=curl_easy_init();
	curl_easy_setopt(handle,CURLOPT_URL,url.c_str());

	std::string auth_header="Authorization: Bot "+this->bot_token;
	curl_slist* headers=curl_slist_append(
		curl_slist_append(
			nullptr,
			"User-Agent: DiscordBot (http://0.0.0.0, 0.0.1)"
		),
		auth_header.c_str()
	);

	curl_easy_setopt(handle,CURLOPT_HTTPHEADER,headers);
	json_value wrapper_json=json_value(json_object<json_value>());
	wrapper_json.val_object["type"]=json_value(4);
	json_value json_payload=json_value(json_object<json_value>());
	if (msgcont!=""){
		json_payload.val_object["content"]=json_value(msgcont);
	}
	json_payload.val_object["attachments"]=json_value(std::vector<json_value>());
	json_payload.val_object["attachments"].val_list.push_back(json_value(json_object<json_value>()));
	json_payload.val_object["attachments"].val_list[0].val_object["id"]=json_value(0);
	json_payload.val_object["attachments"].val_list[0].val_object["filename"]=json_value(filename);
	wrapper_json.val_object["data"]=json_payload;

	curl_mime* atch=curl_mime_init(handle);
	curl_mimepart* flat=curl_mime_addpart(atch);
	curl_mime_name(flat,"payload_json");
	curl_mime_data(flat,wrapper_json.get_repr().c_str(),CURL_ZERO_TERMINATED);
	flat=curl_mime_addpart(atch);
	curl_mime_name(flat,"files[0]");
	curl_mime_filename(flat,filename.c_str());
	curl_mime_data(flat,filedata.c_str(),CURL_ZERO_TERMINATED);
	curl_easy_setopt(handle,CURLOPT_MIMEPOST,atch);

	curl_easy_setopt(handle,CURLOPT_WRITEFUNCTION,restapi::dummy_write_callback);
	CURLcode value=curl_easy_perform(handle);
	if (value!=CURLE_OK){
		lwsl_err(curl_easy_strerror(value));
	}
	curl_easy_cleanup(handle);
}

void restapi::upload_file_df(std::string url,std::string filename,std::string lclfname,std::string msgcont){
	CURL* handle=curl_easy_init();
	curl_easy_setopt(handle,CURLOPT_URL,url.c_str());

	std::string auth_header="Authorization: Bot "+this->bot_token;
	curl_slist* headers=curl_slist_append(
		curl_slist_append(
			nullptr,
			"User-Agent: DiscordBot (http://0.0.0.0, 0.0.1)"
		),
		auth_header.c_str()
	);

	curl_easy_setopt(handle,CURLOPT_HTTPHEADER,headers);
	json_value wrapper_json=json_value(json_object<json_value>());
	wrapper_json.val_object["type"]=json_value(4);
	json_value json_payload=json_value(json_object<json_value>());
	if (msgcont!=""){
		json_payload.val_object["content"]=json_value(msgcont);
	}
	json_payload.val_object["attachments"]=json_value(std::vector<json_value>());
	json_payload.val_object["attachments"].val_list.push_back(json_value(json_object<json_value>()));
	json_payload.val_object["attachments"].val_list[0].val_object["id"]=json_value(0);
	json_payload.val_object["attachments"].val_list[0].val_object["filename"]=json_value(filename);
	wrapper_json.val_object["data"]=json_payload;

	curl_mime* atch=curl_mime_init(handle);
	curl_mimepart* flat=curl_mime_addpart(atch);
	curl_mime_name(flat,"payload_json");
	curl_mime_data(flat,wrapper_json.get_repr().c_str(),CURL_ZERO_TERMINATED);
	flat=curl_mime_addpart(atch);
	curl_mime_name(flat,"files[0]");
	curl_mime_filename(flat,filename.c_str());
	curl_mime_filedata(flat,lclfname.c_str());
	curl_easy_setopt(handle,CURLOPT_MIMEPOST,atch);

	curl_easy_setopt(handle,CURLOPT_WRITEFUNCTION,restapi::dummy_write_callback);
	CURLcode value=curl_easy_perform(handle);
	if (value!=CURLE_OK){
		lwsl_err(curl_easy_strerror(value));
	}
	curl_easy_cleanup(handle);
}

void restapi::upload_data_bf(std::string url,std::string filename,    bstring filedata,std::string msgcont){
	CURL* handle=curl_easy_init();
	curl_easy_setopt(handle,CURLOPT_URL,url.c_str());

	std::string auth_header="Authorization: Bot "+this->bot_token;
	curl_slist* headers=curl_slist_append(
		curl_slist_append(
			nullptr,
			"User-Agent: DiscordBot (http://0.0.0.0, 0.0.1)"
		),
		auth_header.c_str()
	);

	curl_easy_setopt(handle,CURLOPT_HTTPHEADER,headers);
	json_value wrapper_json=json_value(json_object<json_value>());
	wrapper_json.val_object["type"]=json_value(4);
	json_value json_payload=json_value(json_object<json_value>());
	if (msgcont!=""){
		json_payload.val_object["content"]=json_value(msgcont);
	}
	json_payload.val_object["attachments"]=json_value(std::vector<json_value>());
	json_payload.val_object["attachments"].val_list.push_back(json_value(json_object<json_value>()));
	json_payload.val_object["attachments"].val_list[0].val_object["id"]=json_value(0);
	json_payload.val_object["attachments"].val_list[0].val_object["filename"]=json_value(filename);
	wrapper_json.val_object["data"]=json_payload;

	curl_mime* atch=curl_mime_init(handle);
	curl_mimepart* flat=curl_mime_addpart(atch);
	curl_mime_name(flat,"payload_json");
	curl_mime_data(flat,wrapper_json.get_repr().c_str(),CURL_ZERO_TERMINATED);
	flat=curl_mime_addpart(atch);
	curl_mime_name(flat,"files[0]");
	curl_mime_filename(flat,filename.c_str());
	curl_mime_data(flat,(const char*) filedata.c_str(),filedata.length());
	curl_easy_setopt(handle,CURLOPT_MIMEPOST,atch);

	curl_easy_setopt(handle,CURLOPT_WRITEFUNCTION,restapi::dummy_write_callback);
	CURLcode value=curl_easy_perform(handle);
	if (value!=CURLE_OK){
		lwsl_err(curl_easy_strerror(value));
	}
	curl_easy_cleanup(handle);
}

std::string restapi::send_message(std::string channel_id,std::string content){
	CURL* handle=curl_easy_init();
	std::string channel_link="https://discord.com/api/v10/channels/"+channel_id+"/messages";
	curl_easy_setopt(handle,CURLOPT_URL,channel_link.c_str());
	std::string auth_header="Authorization: Bot "+this->bot_token;
	curl_slist* headers=curl_slist_append(
		curl_slist_append(
			curl_slist_append(
				nullptr,
				"User-Agent: DiscordBot (http://0.0.0.0, 0.0.1)"
			),
			"Content-Type: application/json; charset=UTF-8"
		),
		auth_header.c_str()
	);
	curl_easy_setopt(handle,CURLOPT_POST,1);
	curl_easy_setopt(handle,CURLOPT_HTTPHEADER,headers);
	json_value data_json(json_object<json_value>{});
	data_json.val_object["content"]=json_value(content);
	std::string data=data_json.get_repr();
	curl_easy_setopt(handle,CURLOPT_POSTFIELDSIZE,data.length());
	curl_easy_setopt(handle,CURLOPT_POSTFIELDS,data.c_str());
	char* response=(char*) malloc(2000);
	struct curl_response cr={response,0ul};
	curl_easy_setopt(handle,CURLOPT_WRITEFUNCTION,restapi::write_callback);
	curl_easy_setopt(handle,CURLOPT_WRITEDATA,(void*)&cr);
	CURLcode value = curl_easy_perform(handle);
	curl_easy_cleanup(handle);
	if (value!=CURLE_OK){
		lwsl_err(curl_easy_strerror(value));
	}
	std::string sresp=response;
	free(response);
	return sresp;
}

bstring restapi::raw_fetch(std::string url){
	CURL* handle=curl_easy_init();
	std::string fetch_link=url;
	curl_easy_setopt(handle,CURLOPT_URL,fetch_link.c_str());
	struct curl_response cr={0};
	curl_easy_setopt(handle,CURLOPT_WRITEFUNCTION,restapi::write_callback);
	curl_easy_setopt(handle,CURLOPT_WRITEDATA,(void*)&cr);
	CURLcode value = curl_easy_perform(handle);
	curl_easy_cleanup(handle);
	if (value!=CURLE_OK){
		lwsl_err(curl_easy_strerror(value));
	}
	bstring sresp((unsigned char*)cr.data,cr.size);
	free(cr.data);
	return sresp;
}

std::string restapi::fetch(std::string url){
	CURL* handle=curl_easy_init();
	std::string fetch_link=url;
	curl_easy_setopt(handle,CURLOPT_URL,fetch_link.c_str());
	struct curl_response cr={0};
	curl_easy_setopt(handle,CURLOPT_WRITEFUNCTION,restapi::write_callback);
	curl_easy_setopt(handle,CURLOPT_WRITEDATA,(void*)&cr);
	CURLcode value = curl_easy_perform(handle);
	curl_easy_cleanup(handle);
	if (value!=CURLE_OK){
		lwsl_err(curl_easy_strerror(value));
	}
	std::string sresp(cr.data);
	free(cr.data);
	return sresp;
}

std::pair<std::string,std::string> restapi::identify_filetype(std::string content_type){
	std::regex regexstr("^(.+?)(; charset=(.+))?$");
	std::pair<std::string,std::string> ret("","");
	std::smatch matchstr;
	if (std::regex_match(content_type,matchstr,regexstr)){
		ret.first=matchstr[1];
		if (matchstr.length()>=4){
			ret.second=matchstr[3];
		}
	}
	return ret;
}

json_value restapi::reduce_options(json_value options){
	json_value ret(json_object<json_value>{});
	for (json_value liem:options.val_list){
		ret.val_object[liem.val_object["name"].val_string]=liem.val_object["value"];
	}
	return ret;
}

std::vector<json_value> restapi::make_help_with_selection(std::string hpcid, int current_pos, int max_pos){
	bool index=current_pos;
	std::vector<json_value> ret;
	// Then come messasge components
	if (index){
		ret.push_back(json_object<json_value>());
		ret[0].val_object["type"]=1;
		ret[0].val_object["components"]=std::vector<json_value>();

		ret[0].val_object["components"].val_list.push_back(json_object<json_value>());
		ret[0].val_object["components"].val_list[0].val_object["type"]=2;
		ret[0].val_object["components"].val_list[0].val_object["style"]=1;
		ret[0].val_object["components"].val_list[0].val_object["custom_id"]="hp_"+hpcid+"_2147483646";
		ret[0].val_object["components"].val_list[0].val_object["label"]="⏮";

		ret[0].val_object["components"].val_list.push_back(json_object<json_value>());
		ret[0].val_object["components"].val_list[1].val_object["type"]=2;
		ret[0].val_object["components"].val_list[1].val_object["style"]=1;
		ret[0].val_object["components"].val_list[1].val_object["custom_id"]="hp_"+hpcid+"_"+std::to_string(current_pos-1);
		ret[0].val_object["components"].val_list[1].val_object["label"]="◀";
		if (current_pos==1){
			ret[0].val_object["components"].val_list[0].val_object["disabled"]=true;
			ret[0].val_object["components"].val_list[1].val_object["disabled"]=true;
		}

		ret[0].val_object["components"].val_list.push_back(json_object<json_value>());
		ret[0].val_object["components"].val_list[2].val_object["type"]=2;
		ret[0].val_object["components"].val_list[2].val_object["style"]=1;
		ret[0].val_object["components"].val_list[2].val_object["custom_id"]="hp_"+hpcid+"_"+std::to_string(current_pos+1);
		ret[0].val_object["components"].val_list[2].val_object["label"]="▶";

		ret[0].val_object["components"].val_list.push_back(json_object<json_value>());
		ret[0].val_object["components"].val_list[3].val_object["type"]=2;
		ret[0].val_object["components"].val_list[3].val_object["style"]=1;
		ret[0].val_object["components"].val_list[3].val_object["custom_id"]="hp_"+hpcid+"_2147483647";
		ret[0].val_object["components"].val_list[3].val_object["label"]="⏭";
		if (current_pos==max_pos){
			ret[0].val_object["components"].val_list[2].val_object["disabled"]=true;
			ret[0].val_object["components"].val_list[3].val_object["disabled"]=true;
		}
	}
	ret.push_back(json_object<json_value>());
	ret[index].val_object["type"]=1;
	ret[index].val_object["components"]=std::vector<json_value>();
	ret[index].val_object["components"].val_list.push_back(json_object<json_value>());
	ret[index].val_object["components"].val_list[0].val_object["type"]=3;
	ret[index].val_object["components"].val_list[0].val_object["placeholder"]="Search commands";  
	ret[index].val_object["components"].val_list[0].val_object["custom_id"]="help_commands";  
	ret[index].val_object["components"].val_list[0].val_object["options"]=std::vector<json_value>();

	ret[index].val_object["components"].val_list[0].val_object["options"].val_list.push_back(json_object<json_value>());
	ret[index].val_object["components"].val_list[0].val_object["options"].val_list[0].val_object["label"]="Manifest generator";
	ret[index].val_object["components"].val_list[0].val_object["options"].val_list[0].val_object["value"]="manifest";
	ret[index].val_object["components"].val_list[0].val_object["options"].val_list[0].val_object["emoji"]=json_object<json_value>();
	ret[index].val_object["components"].val_list[0].val_object["options"].val_list[0].val_object["emoji"].val_object["id"]=-1;
	ret[index].val_object["components"].val_list[0].val_object["options"].val_list[0].val_object["emoji"].val_object["id"].val_type=4;
	ret[index].val_object["components"].val_list[0].val_object["options"].val_list[0].val_object["emoji"].val_object["name"]="📄";
	ret[index].val_object["components"].val_list[0].val_object["options"].val_list[0].val_object["description"]="Unusable for normies, unnecessary for developers.";

	ret[index].val_object["components"].val_list[0].val_object["options"].val_list.push_back(json_object<json_value>());
	ret[index].val_object["components"].val_list[0].val_object["options"].val_list[1].val_object["label"]="JSON obfuscator";
	ret[index].val_object["components"].val_list[0].val_object["options"].val_list[1].val_object["value"]="obfuscator";
	ret[index].val_object["components"].val_list[0].val_object["options"].val_list[1].val_object["emoji"]=json_object<json_value>();
	ret[index].val_object["components"].val_list[0].val_object["options"].val_list[1].val_object["emoji"].val_object["id"]=-1;
	ret[index].val_object["components"].val_list[0].val_object["options"].val_list[1].val_object["emoji"].val_object["id"].val_type=4;
	ret[index].val_object["components"].val_list[0].val_object["options"].val_list[1].val_object["emoji"].val_object["name"]="🔏";
	ret[index].val_object["components"].val_list[0].val_object["options"].val_list[1].val_object["description"]="Don't spend countless hours obfuscating everything.";

	ret[index].val_object["components"].val_list[0].val_object["options"].val_list.push_back(json_object<json_value>());
	ret[index].val_object["components"].val_list[0].val_object["options"].val_list[2].val_object["label"]="Item generator";
	ret[index].val_object["components"].val_list[0].val_object["options"].val_list[2].val_object["value"]="item";
	ret[index].val_object["components"].val_list[0].val_object["options"].val_list[2].val_object["emoji"]=json_object<json_value>();
	ret[index].val_object["components"].val_list[0].val_object["options"].val_list[2].val_object["emoji"].val_object["id"]=-1;
	ret[index].val_object["components"].val_list[0].val_object["options"].val_list[2].val_object["emoji"].val_object["id"].val_type=4;
	ret[index].val_object["components"].val_list[0].val_object["options"].val_list[2].val_object["emoji"].val_object["name"]="⛏️";
	ret[index].val_object["components"].val_list[0].val_object["options"].val_list[2].val_object["description"]="Can't think of one? Perhaps chopsticks might do.";

	ret[index].val_object["components"].val_list[0].val_object["options"].val_list.push_back(json_object<json_value>());
	ret[index].val_object["components"].val_list[0].val_object["options"].val_list[3].val_object["label"]="Entity generator";
	ret[index].val_object["components"].val_list[0].val_object["options"].val_list[3].val_object["value"]="entity";
	ret[index].val_object["components"].val_list[0].val_object["options"].val_list[3].val_object["emoji"]=json_object<json_value>();
	ret[index].val_object["components"].val_list[0].val_object["options"].val_list[3].val_object["emoji"].val_object["id"]=-1;
	ret[index].val_object["components"].val_list[0].val_object["options"].val_list[3].val_object["emoji"].val_object["id"].val_type=4;
	ret[index].val_object["components"].val_list[0].val_object["options"].val_list[3].val_object["emoji"].val_object["name"]="🐙";
	ret[index].val_object["components"].val_list[0].val_object["options"].val_list[3].val_object["description"]="Currently under beta development. (More like early alpha)";

	ret[index].val_object["components"].val_list[0].val_object["options"].val_list.push_back(json_object<json_value>());
	ret[index].val_object["components"].val_list[0].val_object["options"].val_list[4].val_object["label"]="Help";
	ret[index].val_object["components"].val_list[0].val_object["options"].val_list[4].val_object["value"]="help";
	ret[index].val_object["components"].val_list[0].val_object["options"].val_list[4].val_object["emoji"]=json_object<json_value>();
	ret[index].val_object["components"].val_list[0].val_object["options"].val_list[4].val_object["emoji"].val_object["id"]=-1;
	ret[index].val_object["components"].val_list[0].val_object["options"].val_list[4].val_object["emoji"].val_object["id"].val_type=4;
	ret[index].val_object["components"].val_list[0].val_object["options"].val_list[4].val_object["emoji"].val_object["name"]="❓";
	ret[index].val_object["components"].val_list[0].val_object["options"].val_list[4].val_object["description"]="Self-reference; sometimes catastrophic, sometimes helpful.";

	return ret;
}

std::pair<std::string,int> restapi::parse_hpcid(std::string hpcid){
	std::string pnum; // Page number
	for (int l=hpcid.length()-1;l>=0;l--){
		if (hpcid[l]=='_'){
			return {hpcid.substr(0ul,l),atoi(hpcid.substr(l+1).c_str())};
		}
	}
	return {hpcid,1};
}