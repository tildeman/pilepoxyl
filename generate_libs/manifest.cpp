#include "generate_libs.h"

std::vector<json_value> generatelibs::split_authors(std::string striq,char delim){
	if (striq.length()){
		std::vector<json_value> ret{std::string()}; // "ret" just sounds much better than "res"
		int cpos=0;
		for (char cc:striq){
			if (cc==delim){
				++cpos;
				ret.push_back(std::string());
			}
			else ret[cpos].val_string+=cc;
		}
		return ret;
	}
	return std::vector<json_value>();
}
std::vector<json_value> generatelibs::split_version(std::string striq){
	std::vector<json_value> strarray=generatelibs::split_authors(striq,'.');
	std::vector<json_value> ret;
	for (json_value liem:strarray){
		ret.push_back(atoi(liem.val_string.c_str()));
	}
	return ret;
}

std::string generatelibs::manifest::generate_nondependent(
	std::string pack_type, std::string pack_name, std::string pack_description,
	std::vector<json_value> pack_version, bool pack_use_metadata,
	std::vector<json_value> pack_authors, std::string pack_author_url,
	bool pack_use_gametest, std::string pack_gametest_entry_file
){
	uuid_generator uuid_generator_obj;
	json_value output(json_object<json_value>{});
	output.val_object.object_pair={
		{"format_version", json_value(2)},
		{"header", json_value(json_object<json_value>{})}
	};
	if (pack_name!=""){
		output.val_object["header"].val_object["name"]=json_value(pack_name);
	}
	if (pack_description!=""&&pack_type!="skin_pack"){
		output.val_object["header"].val_object["description"]=json_value(pack_description);
	}
	output.val_object["header"].val_object["uuid"]=uuid_generator_obj.generate_uuid();
	output.val_object["header"].val_object["version"]=json_value(std::vector<json_value>{1,0,0});
	output.val_object["modules"]=json_value(std::vector<json_value>{});
	output.val_object["modules"].val_list.push_back(json_value(json_object<json_value>{}));
	output.val_object["modules"].val_list[0].val_object["type"]=json_value(pack_type);
	if (pack_type=="scripts") output.val_object["modules"].val_list[0].val_object["type"]=json_value("data");
	output.val_object["modules"].val_list[0].val_object["uuid"]=uuid_generator_obj.generate_uuid();
	output.val_object["modules"].val_list[0].val_object["version"]=json_value(std::vector<json_value>{1,0,0});
	if (pack_type=="scripts"){
		output.val_object["modules"].val_list.push_back(json_value(json_object<json_value>{}));
		if (pack_description!="") output.val_object["modules"].val_list[1].val_object["description"]=json_value(pack_description);
		output.val_object["modules"].val_list[1].val_object["type"]= json_value("client_data");
		output.val_object["modules"].val_list[1].val_object["uuid"]=uuid_generator_obj.generate_uuid();
		output.val_object["modules"].val_list[1].val_object["version"]=json_value(std::vector<json_value>{1,0,0});
	}
	if (pack_version[0].val_int!=0&&pack_type!="skin_pack") output.val_object["header"].val_object["min_engine_version"]=pack_version;
	if (pack_use_metadata){
		output.val_object["metadata"]=json_value(json_object<json_value>{});
		if (!pack_authors.empty()) output.val_object["metadata"].val_object["authors"]=pack_authors;
		if (!pack_author_url.empty()) output.val_object["metadata"].val_object["url"]=pack_author_url;
	}
	if (pack_type!="data"&&pack_type!="scripts") pack_use_gametest=0;
	if (pack_use_gametest){
		output.val_object["dependencies"]=json_value(std::vector<json_value>{});
		output.val_object["dependencies"].val_list.push_back(json_value(json_object<json_value>{}));
		output.val_object["dependencies"].val_list[0].val_object["description"]=std::string("Minecraft module, required to use the Minecraft module");
		output.val_object["dependencies"].val_list[0].val_object["uuid"]=uuid_generator_obj.generate_uuid();
		output.val_object["dependencies"].val_list[0].val_object["version"]=std::vector<json_value>{0,1,0};
		output.val_object["dependencies"].val_list.push_back(json_value(json_object<json_value>{}));
		output.val_object["dependencies"].val_list[1].val_object["description"]=std::string("GameTest module, required to use the GameTest module");
		output.val_object["dependencies"].val_list[1].val_object["uuid"]=uuid_generator_obj.generate_uuid();
		output.val_object["dependencies"].val_list[1].val_object["version"]=std::vector<json_value>{0,1,0};
		if (pack_gametest_entry_file!=""){
			if (pack_type=="scripts"){
				output.val_object["modules"].val_list.push_back(json_value(json_object<json_value>{}));
				output.val_object["modules"].val_list[2].val_object["type"]=std::string("javascript");
				output.val_object["modules"].val_list[2].val_object["version"]=std::vector<json_value>{1,0,0};
				output.val_object["modules"].val_list[2].val_object["uuid"]=uuid_generator_obj.generate_uuid();
				output.val_object["modules"].val_list[2].val_object["entry"]=std::string("scripts/")+pack_gametest_entry_file;
			}
			else{
				output.val_object["modules"].val_list.push_back(json_value(json_object<json_value>{}));
				output.val_object["modules"].val_list[1].val_object["type"]=std::string("javascript");
				output.val_object["modules"].val_list[1].val_object["version"]=std::vector<json_value>{1,0,0};
				output.val_object["modules"].val_list[1].val_object["uuid"]=uuid_generator_obj.generate_uuid();
				output.val_object["modules"].val_list[1].val_object["entry"]=std::string("scripts/")+pack_gametest_entry_file;
			}
		}
	}
	return output.get_repr();
}
std::vector<std::string> generatelibs::manifest::generate_dependent(
	std::string bp_name, std::string bp_description,
	std::vector<json_value> bp_version, bool bp_use_scripts,
	bool bp_use_metadata, std::vector<json_value> bp_authors,
	std::string bp_author_url, std::string rp_name
	, std::string rp_description, std::vector<json_value> rp_version,
	bool rp_use_metadata, std::vector<json_value> rp_authors,
	std::string rp_author_url, bool rp_use_ui, bool bp_use_gametest,
	std::string bp_gametest_entry_file
){
	uuid_generator uuid_generator_obj;
	json_value behaviorpack(json_object<json_value>{});
	behaviorpack.val_object["format_version"]=json_value(2);
	behaviorpack.val_object["header"]=json_value(json_object<json_value>{});
	behaviorpack.val_object["modules"]=json_value(std::vector<json_value>{json_object<json_value>{}});
	behaviorpack.val_object["dependencies"]=json_value(std::vector<json_value>{json_object<json_value>{}});

	if (bp_name!="") behaviorpack.val_object["header"].val_object["name"]=json_value(bp_name);
	if (bp_description!="") behaviorpack.val_object["header"].val_object["description"]=json_value(bp_description);
	behaviorpack.val_object["header"].val_object["uuid"]=json_value(uuid_generator_obj.generate_uuid());
	behaviorpack.val_object["header"].val_object["version"]=json_value(std::vector<json_value>{1,0,0});
	if (bp_version[0].val_int!=0) behaviorpack.val_object["header"].val_object["min_engine_version"]=bp_version;
	behaviorpack.val_object["modules"].val_list.push_back(json_value(json_object<json_value>{}));
	behaviorpack.val_object["modules"].val_list[0].val_object["type"]=std::string("data");
	behaviorpack.val_object["modules"].val_list[0].val_object["uuid"]=uuid_generator_obj.generate_uuid();
	behaviorpack.val_object["modules"].val_list[0].val_object["version"]=json_value(std::vector<json_value>{1,0,0});
	if (bp_use_scripts){
		behaviorpack.val_object["modules"].val_list.push_back(json_value(json_object<json_value>{}));
		behaviorpack.val_object["modules"].val_list[1].val_object["type"]=std::string("client_data");
		behaviorpack.val_object["modules"].val_list[1].val_object["uuid"]=uuid_generator_obj.generate_uuid();
		behaviorpack.val_object["modules"].val_list[1].val_object["version"]=json_value(std::vector<json_value>{1,0,0});
	}
	if(bp_use_metadata){
		behaviorpack.val_object["metadata"]=json_value(json_object<json_value>{});
		if (!bp_authors.empty()) behaviorpack.val_object["metadata"].val_object["authors"]=bp_authors;
		if (!bp_author_url.empty()) behaviorpack.val_object["metadata"].val_object["url"]=bp_author_url;
	}
	json_value resourcepack(json_object<json_value>{});
	resourcepack.val_object["format_version"]=json_value(2);
	resourcepack.val_object["header"]=json_value(json_object<json_value>{});
	resourcepack.val_object["modules"]=json_value(std::vector<json_value>{json_object<json_value>{}});
	resourcepack.val_object["dependencies"]=json_value(std::vector<json_value>{json_object<json_value>{}});

	if (rp_name!="") resourcepack.val_object["header"].val_object["name"]=json_value(rp_name);
	if (rp_description!="") resourcepack.val_object["header"].val_object["description"]=json_value(rp_description);
	resourcepack.val_object["header"].val_object["uuid"]=json_value(uuid_generator_obj.generate_uuid());
	resourcepack.val_object["header"].val_object["version"]=json_value(std::vector<json_value>{1,0,0});
	if (rp_version[0].val_int!=0) resourcepack.val_object["header"].val_object["min_engine_version"]=rp_version;
	resourcepack.val_object["modules"].val_list[0].val_object["type"]=std::string("resources");
	resourcepack.val_object["modules"].val_list[0].val_object["uuid"]=uuid_generator_obj.generate_uuid();
	resourcepack.val_object["modules"].val_list[0].val_object["version"]=json_value(std::vector<json_value>{1,0,0});
	if(rp_use_metadata){
		resourcepack.val_object["metadata"]=json_value(json_object<json_value>{});
		if (!rp_authors.empty()) resourcepack.val_object["metadata"].val_object["authors"]=rp_authors;
		if (!rp_author_url.empty()) resourcepack.val_object["metadata"].val_object["url"]=rp_author_url;
	}

	behaviorpack.val_object["dependencies"].val_list[0].val_object["uuid"]=resourcepack.val_object["header"].val_object["uuid"];
	behaviorpack.val_object["dependencies"].val_list[0].val_object["version"]=resourcepack.val_object["header"].val_object["version"];
	resourcepack.val_object["dependencies"].val_list[0].val_object["uuid"]=behaviorpack.val_object["header"].val_object["uuid"];
	resourcepack.val_object["dependencies"].val_list[0].val_object["version"]=resourcepack.val_object["header"].val_object["version"];
	if (bp_use_scripts&&rp_use_ui){
		resourcepack.val_object["capabilities"]=json_value(std::vector<json_value>{});
		resourcepack.val_object["capabilities"].val_list.push_back(std::string("experimental_custom_ui"));
	}
	if (bp_use_gametest){
		behaviorpack.val_object["dependencies"].val_list[0].val_object["description"]=std::string("Minecraft module, required to use the Minecraft module");
		behaviorpack.val_object["dependencies"].val_list[0].val_object["uuid"]=uuid_generator_obj.generate_uuid();
		behaviorpack.val_object["dependencies"].val_list[0].val_object["version"]=json_value(std::vector<json_value>{0,1,0});
		behaviorpack.val_object["dependencies"].val_list.push_back(json_value(json_object<json_value>{}));
		behaviorpack.val_object["dependencies"].val_list[1].val_object["description"]=std::string("GameTest module, required to use the GameTest module");
		behaviorpack.val_object["dependencies"].val_list[1].val_object["uuid"]=uuid_generator_obj.generate_uuid();
		behaviorpack.val_object["dependencies"].val_list[1].val_object["version"]=json_value(std::vector<json_value>{0,1,0});
		if (bp_use_scripts){
			behaviorpack.val_object["modules"].val_list.push_back(json_value(json_object<json_value>{}));
			behaviorpack.val_object["modules"].val_list[2].val_object["type"]=std::string("javascript");
			behaviorpack.val_object["modules"].val_list[2].val_object["uuid"]=uuid_generator_obj.generate_uuid();
			behaviorpack.val_object["modules"].val_list[2].val_object["version"]=json_value(std::vector<json_value>{1,0,0});
			behaviorpack.val_object["modules"].val_list[2].val_object["entry"]=json_value(std::string("scripts/")+bp_gametest_entry_file);
		}
		else{
			behaviorpack.val_object["modules"].val_list.push_back(json_value(json_object<json_value>{}));
			behaviorpack.val_object["modules"].val_list[1].val_object["type"]=std::string("javascript");
			behaviorpack.val_object["modules"].val_list[1].val_object["uuid"]=uuid_generator_obj.generate_uuid();
			behaviorpack.val_object["modules"].val_list[1].val_object["version"]=json_value(std::vector<json_value>{1,0,0});
			behaviorpack.val_object["modules"].val_list[1].val_object["entry"]=json_value(std::string("scripts/")+bp_gametest_entry_file);
		}
	}
	return std::vector<std::string>{behaviorpack.get_repr(),resourcepack.get_repr()};
}