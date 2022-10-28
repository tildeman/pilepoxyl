#include "generate_libs.h"

// ***********************************
// * Note to self: don't get lost :) *
// ***********************************
bool generatelibs::entity::verifyIdentifier(std::string identifier){
	std::regex regex_string("([^!@#$%^&*()+\\-=\\[\\]{};'\"\\\\|,<>\\/?.:][^!@#$%^&*()+\\-=\\[\\]{};'\"\\\\|,<>\\/?]*):([^!@#$%^&*()+\\-=\\[\\]{}:;'\"\\\\|,<>\\/?]+)");
	std::smatch match_string;
	bool a=regex_match(identifier,match_string,regex_string);
	if (a){
		return (match_string[1].str()!="minecraft"&&match_string[1].str()!="minecon");
	}
	return false;
}
std::string generatelibs::entity::get_ename(std::string identifier){
	std::regex regex_string("([^!@#$%^&*()+\\-=\\[\\]{};'\"\\\\|,<>\\/?.:][^!@#$%^&*()+\\-=\\[\\]{};'\"\\\\|,<>\\/?]*):([^!@#$%^&*()+\\-=\\[\\]{}:;'\"\\\\|,<>\\/?]+)");
	std::smatch match_string;
	bool a=regex_match(identifier,match_string,regex_string);
	if (a){
		return match_string[2];
	}
	return "";
}
std::string generatelibs::entity::strippng(std::string filename){
	std::regex regex_string("(.*)\\.png");
	std::smatch match_string;
	bool a=regex_match(filename,match_string,regex_string);
	if (a){
		return match_string[1].str();
	}
	return filename;
}
std::string generatelibs::entity::to_lower(std::string a){
	int l=a.length();
	std::string b;
	for (int i=0;i<l;i++){
		if (a[i]>64&&a[i]<91) b+=a[i]+32;
		else b+=a[i];
	}
	return b;
}
std::string generatelibs::entity::getGeo(std::string model_json){
	json_value pmj=json_value::parse(model_json);
	// Build the power tower, but when I say power I mean parameter
	if (pmj.val_type!=3) return "";
	if (pmj.val_object["minecraft:geometry"].val_type!=2) return "";
	if (pmj.val_object["minecraft:geometry"].val_list[0].val_type!=3) return "";
	if (pmj.val_object["minecraft:geometry"].val_list[0].val_object["description"].val_type!=3) return "";
	if (pmj.val_object["minecraft:geometry"].val_list[0].val_object["description"].val_object["identifier"].val_type!=1){
		return "";
	}
	return pmj.val_object["minecraft:geometry"].val_list[0].val_object["description"].val_object["identifier"].val_string;
}

json_value generatelibs::entity::genManifest(bool select_both){
	uuid_generator uuid;
	json_value bp=json_value(json_object<json_value>{});
	json_value rp=json_value(json_object<json_value>{});
	json_value rs=json_value(json_object<json_value>{});
	bp.val_object["format_version"]=2;
	rp.val_object["format_version"]=2;
	bp.val_object["header"]=json_value(json_object<json_value>{});
	rp.val_object["header"]=json_value(json_object<json_value>{});
	bp.val_object["header"].val_object["name"]=std::string("BP");
	rp.val_object["header"].val_object["name"]=std::string("RP");
	bp.val_object["header"].val_object["description"]=std::string("BP Description");
	rp.val_object["header"].val_object["description"]=std::string("RP Description");
	bp.val_object["header"].val_object["uuid"]=uuid.generate_uuid();
	rp.val_object["header"].val_object["uuid"]=uuid.generate_uuid();
	bp.val_object["header"].val_object["version"]=json_value(std::vector<json_value>{0,0,1});
	rp.val_object["header"].val_object["version"]=json_value(std::vector<json_value>{0,0,1});
	bp.val_object["header"].val_object["min_engine_version"]=json_value(std::vector<json_value>{1,16,0});
	rp.val_object["header"].val_object["min_engine_version"]=json_value(std::vector<json_value>{1,16,0});
	bp.val_object["modules"]=json_value(std::vector<json_value>{json_object<json_value>{}});
	rp.val_object["modules"]=json_value(std::vector<json_value>{json_object<json_value>{}});
	bp.val_object["modules"].val_list[0].val_object["type"]=std::string("data");
	rp.val_object["modules"].val_list[0].val_object["type"]=std::string("resources");
	bp.val_object["modules"].val_list[0].val_object["uuid"]=uuid.generate_uuid();
	rp.val_object["modules"].val_list[0].val_object["uuid"]=uuid.generate_uuid();
	bp.val_object["modules"].val_list[0].val_object["version"]=json_value(std::vector<json_value>{0,0,1});
	rp.val_object["modules"].val_list[0].val_object["version"]=json_value(std::vector<json_value>{0,0,1});
	if (select_both){
		bp.val_object["dependencies"]=json_value(std::vector<json_value>{json_object<json_value>{}});
		rp.val_object["dependencies"]=json_value(std::vector<json_value>{json_object<json_value>{}});
		bp.val_object["dependencies"].val_list[0].val_object["uuid"]=rp.val_object["header"].val_object["uuid"];
		rp.val_object["dependencies"].val_list[0].val_object["uuid"]=bp.val_object["header"].val_object["uuid"];
		bp.val_object["dependencies"].val_list[0].val_object["version"]=rp.val_object["header"].val_object["version"];
		rp.val_object["dependencies"].val_list[0].val_object["version"]=rp.val_object["header"].val_object["version"];
	}
	rs.val_object["rp"]=rp;
	rs.val_object["bp"]=bp;
	return rs;
}
json_value generatelibs::entity::genRenderControllers(std::string idv1){
	json_value rcobject=json_value(json_object<json_value>{});
	rcobject.val_object["format_version"]=std::string("1.10.0");
	rcobject.val_object["render_controllers"]=json_value(json_object<json_value>{});
	rcobject.val_object["render_controllers"].val_object[std::string("controller.render.")+idv1]=json_value(json_object<json_value>{});
	rcobject.val_object["render_controllers"].val_object[std::string("controller.render.")+idv1].val_object["geometry"]=std::string("geometry.default");
	rcobject.val_object["render_controllers"].val_object[std::string("controller.render.")+idv1].val_object["materials"]=json_value(std::vector<json_value>{json_object<json_value>{}});
	rcobject.val_object["render_controllers"].val_object[std::string("controller.render.")+idv1].val_object["materials"].val_list[0].val_object["*"]=std::string("material.default");
	rcobject.val_object["render_controllers"].val_object[std::string("controller.render.")+idv1].val_object["textures"]=json_value(std::vector<json_value>{});
	rcobject.val_object["render_controllers"].val_object[std::string("controller.render.")+idv1].val_object["textures"].val_list.push_back(std::string("texture.default"));
	return rcobject;
}
json_value generatelibs::entity::genBehBasic(std::string identifier,bool isSpawnable,bool isSummonable,bool isExperimental){
	json_value entityobj=json_value(json_object<json_value>{});
	entityobj.val_object["minecraft:entity"]=json_value(json_object<json_value>{});
	entityobj.val_object["minecraft:entity"].val_object["description"]=json_value(json_object<json_value>{});
	entityobj.val_object["minecraft:entity"].val_object["description"].val_object["identifier"]=identifier;
	entityobj.val_object["minecraft:entity"].val_object["description"].val_object["is_spawnable"]=isSpawnable;
	entityobj.val_object["minecraft:entity"].val_object["description"].val_object["is_summonable"]=isSummonable;
	entityobj.val_object["minecraft:entity"].val_object["description"].val_object["is_experimental"]=isExperimental;
	entityobj.val_object["minecraft:entity"].val_object["components"]=json_value(json_object<json_value>{});
	entityobj.val_object["minecraft:entity"].val_object["components"].val_object["minecraft:health"]=json_value(json_object<json_value>{});
	entityobj.val_object["minecraft:entity"].val_object["components"].val_object["minecraft:health"].val_object["value"]=20;
	entityobj.val_object["minecraft:entity"].val_object["components"].val_object["minecraft:physics"]=json_value(json_object<json_value>{});
	return entityobj;
}
file generatelibs::entity::make(
	std::string identifier,
	file textureFile,
	file modelFile,
	bool isSpawnable,
	bool isSummonable,
	bool isExperimental,
	std::string spawnEggOverlay,
	std::string spawnEggBase,
	std::string material,
	std::string select
){
	file jszobject=file("",std::vector<file>{}); // It's not JS but whatever //
	std::string fn;
	if (verifyIdentifier(identifier)){
		if (select=="beh") jszobject.directory_contents.push_back(file("behavior_pack",std::vector<file>{}));
		else jszobject.directory_contents.push_back(file("resource_pack",std::vector<file>{}));
		if (select=="both") jszobject.directory_contents.push_back(file("behavior_pack",std::vector<file>{}));
		json_value manifest;
		manifest=genManifest(select=="both");
		json_value respack=manifest.val_object["rp"];
		json_value behpack=manifest.val_object["bp"];
		json_value cliententity=json_object<json_value>();
		cliententity.val_object.object_pair={{
			{"format_version",std::string("1.10.0")},
			{"minecraft:client_entity",json_value(json_object<json_value>{})}
		}};
		cliententity.val_object["minecraft:client_entity"].val_object["description"]=json_value(json_object<json_value>{});
		cliententity.val_object["minecraft:client_entity"].val_object["description"].val_object["identifier"]=identifier;
		if (isSpawnable){
			cliententity.val_object["minecraft:client_entity"].val_object["description"].val_object["spawn_egg"]=json_value(json_object<json_value>{});
			cliententity.val_object["minecraft:client_entity"].val_object["description"].val_object["spawn_egg"].val_object["overlay_color"]=spawnEggOverlay;
			cliententity.val_object["minecraft:client_entity"].val_object["description"].val_object["spawn_egg"].val_object["base_color"]=spawnEggBase;
		}
		if (std::string(textureFile.filename)!=""){
			std::string f=strippng(textureFile.filename);
			cliententity.val_object["minecraft:client_entity"].val_object["description"].val_object["textures"]=json_value(json_object<json_value>{});
			cliententity.val_object["minecraft:client_entity"].val_object["description"].val_object["textures"].val_object["default"]="textures/entity/"+f;
			if (select!="beh") jszobject.directory_contents[0].directory_contents.push_back(file("textures",std::vector<file>{file("entity",std::vector<file>{textureFile})}));
		}
		cliententity.val_object["minecraft:client_entity"].val_object["description"].val_object["materials"]=json_value(json_object<json_value>{});
		cliententity.val_object["minecraft:client_entity"].val_object["description"].val_object["materials"].val_object["default"]=material;
		if (std::string(modelFile.filename)!=""){
			cliententity.val_object["minecraft:client_entity"].val_object["description"].val_object["geometry"]=json_value(json_object<json_value>{});
			cliententity.val_object["minecraft:client_entity"].val_object["description"].val_object["geometry"].val_object["default"]=this->to_lower(this->getGeo(modelFile.text_contents));
			if (select!="beh") jszobject.directory_contents[0].directory_contents.push_back(file("models",std::vector<file>{file("entity",std::vector<file>{modelFile})}));
		}
		cliententity.val_object["minecraft:client_entity"].val_object["description"].val_object["render_controllers"]=json_value(std::vector<json_value>{});
		cliententity.val_object["minecraft:client_entity"].val_object["description"].val_object["render_controllers"].val_list.push_back(std::string("controller.render.")+get_ename(identifier));
		json_value rendercontr=json_value(json_object<json_value>{});
		if (select!="beh"&&std::string(modelFile.filename)!=""&&std::string(textureFile.filename)!=""){
			rendercontr=genRenderControllers(get_ename(identifier));
		}
		// **********************************************
		// * NOTE (for JustAsh): I'm a lazy bum, so     *
		// * I'm not bothering to fix your broken code. *
		// **********************************************
		json_value beh=genBehBasic(identifier,isSpawnable,isSummonable,isExperimental);
		if (select!="beh"){
			jszobject.directory_contents[0].directory_contents.push_back(file("manifest.json",respack.get_repr()));
			jszobject.directory_contents[0].directory_contents.push_back(file("texts",std::vector<file>{file("en_US.lang",std::string("entity")+identifier+".name="+get_ename(identifier))}));
			jszobject.directory_contents[0].directory_contents.push_back(file("entity",std::vector<file>{file(get_ename(identifier)+".client.json",cliententity.get_repr())}));
			jszobject.directory_contents[0].directory_contents.push_back(file("render_controllers",std::vector<file>{file(get_ename(identifier)+".render_controllers.json",rendercontr.get_repr())}));
		}
		if (select!="res"){
			jszobject.directory_contents[(select=="both")].directory_contents.push_back(file("manifest.json",behpack.get_repr()));
			jszobject.directory_contents[(select=="both")].directory_contents.push_back(file("entities",std::vector<file>{file(get_ename(identifier)+".json",beh.get_repr())}));
		}
		if (select=="beh") fn=(get_ename(identifier)+".bp.mcpack");
		else if (select=="res") fn=(get_ename(identifier)+".rp.mcpack");
		else if (select=="both") fn=(get_ename(identifier)+".mcaddon");
	}
	jszobject.filename=fn;
	return jszobject;
}