#include "generate_libs.h"

bool generatelibs::item::verifyIdentifier(std::string identifier){
	std::regex regex_string("([^!@#$%^&*()+\\-=\\[\\]{};'\"\\\\|,<>\\/?.:][^!@#$%^&*()+\\-=\\[\\]{};'\"\\\\|,<>\\/?]*):([^!@#$%^&*()+\\-=\\[\\]{}:;'\"\\\\|,<>\\/?]+)");
	std::smatch match_string;
	bool a=regex_match(identifier,match_string,regex_string);
	if (a){
		return (match_string[1].str()!="minecraft"&&match_string[1].str()!="minecon");
	}
	return false;
}
std::string generatelibs::item::to_lower(std::string a){
	int l=a.length();
	std::string b;
	for (int i=0;i<l;i++){
		if (a[i]>64&&a[i]<91) b+=a[i]-32;
		else b+=a[i];
	}
	return b;
}
std::string generatelibs::item::get_iname(std::string identifier){
	std::regex regex_string("([^!@#$%^&*()+\\-=\\[\\]{};'\"\\\\|,<>\\/?.:][^!@#$%^&*()+\\-=\\[\\]{};'\"\\\\|,<>\\/?]*):([^!@#$%^&*()+\\-=\\[\\]{}:;'\"\\\\|,<>\\/?]+)");
	std::smatch match_string;
	bool a=regex_match(identifier,match_string,regex_string);
	if (a){
		return match_string[2];
	}
	return "";
}
json_value generatelibs::item::genManifest(){
	uuid_generator uuid;
	json_value bp=json_value(json_object<json_value>{});
	json_value rp=json_value(json_object<json_value>{});
	json_value rs=json_value(json_object<json_value>{});
	bp.val_object["format_version"]=2;
	rp.val_object["format_version"]=2;
	bp.val_object["header"]=json_value(json_object<json_value>{});
	rp.val_object["header"]=json_value(json_object<json_value>{});
	bp.val_object["header"].val_object["name"]=std::string("Item behaviorpack");
	rp.val_object["header"].val_object["name"]=std::string("Item resourcepack");
	bp.val_object["header"].val_object["description"]=std::string("Behavior pack required for the item to work, generated not using https://tools.pixelpoly.co/item-generator");
	rp.val_object["header"].val_object["description"]=std::string("Resource pack required for the item to work, generated not using https://tools.pixelpoly.co/item-generator");
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
	bp.val_object["dependencies"]=json_value(std::vector<json_value>{json_object<json_value>{}});
	rp.val_object["dependencies"]=json_value(std::vector<json_value>{json_object<json_value>{}});
	bp.val_object["dependencies"].val_list[0].val_object["uuid"]=rp.val_object["header"].val_object["uuid"];
	rp.val_object["dependencies"].val_list[0].val_object["uuid"]=bp.val_object["header"].val_object["uuid"];
	bp.val_object["dependencies"].val_list[0].val_object["version"]=rp.val_object["header"].val_object["version"];
	rp.val_object["dependencies"].val_list[0].val_object["version"]=rp.val_object["header"].val_object["version"];
	rs.val_object["rp"]=rp;
	rs.val_object["bp"]=bp;
	return rs;
}
std::string generatelibs::item::truncate_slashes(std::string strink){
	std::regex regex_string(".*[\\\\\\/](.+)");
	std::smatch match_string;
	bool a=regex_match(strink,match_string,regex_string);
	if (a){
		return match_string[1];
	}
	return strink;
}
file generatelibs::item::writeItem(
	std::string formatVersion, std::string identifier,
	std::string category, bool isExp, std::string icon,
	std::string displayName, long long maxStackSize, bool useAnimCheckbox,
	std::string useAnim, bool foil, double useDuration, bool stackedData,
	bool handEquipped, bool destroyInC, bool ccategoryCheckbox,
	std::string ccategory, bool foodCheckbox, double foodNutr,
	std::string foodSatu, bool foodCanA, std::string foodConvert, bool isexplod,
	bool allowOffHand, bool wearableCheckbox, std::string wearableSlot,
	long long armorProtection, bool cooldownCheckbox, std::string cooldownCategory,
	double cooldownDuration, long long damage, bool blockPlacerCheckbox,
	std::string blockPlacerBlock, bool blockPlacerDes, bool entityPlacerCheckbox,
	std::string entityPlacerEntity, std::vector<json_value> entityPlacerUseon,
	std::vector<json_value> entityPlacerDison, bool enchantableCheckbox,
	std::string enchantableSlot, long long enchantableValue, bool shooterCheckbox,
	double shooterMaxDraw, bool shooterCharge, bool shooterScale,
	bool shooterAmmoCheckbox, bool shooterAmmoInven, std::string shooterAmmoItem,
	bool shooterAmmoHand, bool shooterAmmoCreative, long long fertilizer,
	bool ignPerms, long long miningSpeed, bool liquidClipped, bool shouldDespawn,
	bool mirroredArt, bool projectileCheckbox, std::string projectileEntity,
	double projectilePower, bool dyeCheckbox, std::string dyePowder,
	bool throwableCheckbox, bool throwableSwing, double throwableDraw,
	bool throwablePower, double knockbackResis, double fuel,
	bool recordCheckbox, std::string recordSound, long long recordDuration,
	long long recordSignal, file itemIcon, bool enable_itematlas
){
	json_value item=json_value(json_object<json_value>{});
	item.val_object["format_version"]=formatVersion;
	item.val_object["minecraft:item"]=json_value(json_object<json_value>{});
	item.val_object["minecraft:item"].val_object["description"]=json_value(json_object<json_value>{});
	if (this->verifyIdentifier(identifier)){
		item.val_object["minecraft:item"].val_object["description"].val_object["identifier"]=to_lower(identifier);
	}
	item.val_object["minecraft:item"].val_object["description"].val_object["category"]=category;
	if (isExp){
		item.val_object["minecraft:item"].val_object["description"].val_object["is_experimental"]=isExp;
	}
	item.val_object["minecraft:item"].val_object["components"]=json_value(json_object<json_value>{});
	if (icon!=""&&verifyIdentifier(identifier)){
		item.val_object["minecraft:item"].val_object["components"].val_object["minecraft:icon"]=json_value(json_object<json_value>{});
		item.val_object["minecraft:item"].val_object["components"].val_object["minecraft:icon"].val_object["texture"]=to_lower(get_iname(identifier)+".texture");
	}
	json_value itemAtlas=json_value(json_object<json_value>{});
	if (enable_itematlas&&icon!=""){
		itemAtlas.val_object["resource_pack_name"]=std::string("vanilla");
		itemAtlas.val_object["texture_name"]=std::string("atlas.items");
		itemAtlas.val_object["texture_data"]=json_value(json_object<json_value>{});
		if (this->verifyIdentifier(identifier)){
			itemAtlas.val_object["texture_data"].val_object[get_iname(identifier)+".texture"]=json_value(json_object<json_value>{});
			itemAtlas.val_object["texture_data"].val_object[get_iname(identifier)+".texture"].val_object["textures"]=icon;
		}
	}
	if (displayName!=""){
		item.val_object["minecraft:item"].val_object["components"].val_object["minecraft:display_name"]=json_value(json_object<json_value>{});
		item.val_object["minecraft:item"].val_object["components"].val_object["minecraft:display_name"].val_object["value"]=displayName;
	}
	if (maxStackSize>=1 && maxStackSize<=64){
		item.val_object["minecraft:item"].val_object["components"].val_object["minecraft:max_stack_size"]=maxStackSize;
	}
	if (foil){
		item.val_object["minecraft:item"].val_object["components"].val_object["minecraft:foil"]=foil;
	}
	if (useAnimCheckbox&&useAnim!="none"){
		item.val_object["minecraft:item"].val_object["components"].val_object["minecraft:use_animation"]=useAnim;
	}
	if (useDuration){
		item.val_object["minecraft:item"].val_object["components"].val_object["minecraft:use_duration"]=useDuration;
	}
	if (stackedData){
		item.val_object["minecraft:item"].val_object["components"].val_object["minecraft:stacked_by_data"]=stackedData;
	}
	if (handEquipped){
		item.val_object["minecraft:item"].val_object["components"].val_object["minecraft:hand_equipped"]=handEquipped;
	}
	if (!destroyInC){
		item.val_object["minecraft:item"].val_object["components"].val_object["minecraft:can_destroy_in_creative"]=destroyInC;
	}
	if (ccategoryCheckbox){
		item.val_object["minecraft:item"].val_object["components"].val_object["minecraft:creative_category"]=json_value(json_object<json_value>{});
		if (ccategory!="none"){
			item.val_object["minecraft:item"].val_object["components"].val_object["minecraft:creative_category"].val_object["parent"]=ccategory;
		}
	}
	if (foodCheckbox){
		item.val_object["minecraft:item"].val_object["components"].val_object["minecraft:food"]=json_value(json_object<json_value>{});
		if (foodNutr>=0)item.val_object["minecraft:item"].val_object["components"].val_object["minecraft:food"].val_object["nutrition"]=foodNutr;
		if (foodSatu!="") item.val_object["minecraft:item"].val_object["components"].val_object["minecraft:food"].val_object["saturation_modifier"]=foodSatu;
		item.val_object["minecraft:item"].val_object["components"].val_object["minecraft:food"].val_object["can_always_eat"]=foodCanA;
		if (foodConvert!="") item.val_object["minecraft:item"].val_object["components"].val_object["minecraft:food"].val_object["using_converts_to"]=foodConvert;
	}
	if (!isexplod){
		item.val_object["minecraft:item"].val_object["components"].val_object["minecraft:explodable"]=isexplod;
	}
	if (!allowOffHand){
		item.val_object["minecraft:item"].val_object["components"].val_object["minecraft:allow_off_hand"]=allowOffHand;
	}
	if (wearableCheckbox){
		item.val_object["minecraft:item"].val_object["components"].val_object["minecraft:wearable"]=json_value(json_object<json_value>{});
		if (wearableSlot!="none")item.val_object["minecraft:item"].val_object["components"].val_object["minecraft:wearable"].val_object["slot"]="slot.armor."+wearableSlot;
	}
	if (armorProtection>=0&&armorProtection<=20){
		item.val_object["minecraft:item"].val_object["components"].val_object["minecraft:armor"]=json_value(json_object<json_value>{});
		item.val_object["minecraft:item"].val_object["components"].val_object["minecraft:armor"].val_object["protection"]=armorProtection;
	}
	if (cooldownCheckbox){
		item.val_object["minecraft:item"].val_object["components"].val_object["minecraft:cooldown"]=json_value(json_object<json_value>{});
		if (cooldownCategory!="") item.val_object["minecraft:item"].val_object["components"].val_object["minecraft:cooldown"].val_object["category"]=cooldownCategory;
		if (cooldownDuration>=0) item.val_object["minecraft:item"].val_object["components"].val_object["minecraft:cooldown"].val_object["duration"]=cooldownDuration;
	}
	if (damage>=0) item.val_object["minecraft:item"].val_object["components"].val_object["minecraft:damage"]=damage;
	if (blockPlacerCheckbox){
		item.val_object["minecraft:item"].val_object["components"].val_object["minecraft:block_placer"]=json_value(json_object<json_value>{});
		if (blockPlacerBlock!="") item.val_object["minecraft:item"].val_object["components"].val_object["minecraft:block_placer"].val_object["block"]=blockPlacerBlock;
		if (blockPlacerDes) item.val_object["minecraft:item"].val_object["components"].val_object["minecraft:block_placer"].val_object["use_block_description"]=blockPlacerDes;
	}
	if (entityPlacerCheckbox){
		item.val_object["minecraft:item"].val_object["components"].val_object["minecraft:entity_placer"]=json_value(json_object<json_value>{});
		if (entityPlacerEntity!="") item.val_object["minecraft:item"].val_object["components"].val_object["minecraft:entity_placer"].val_object["entity"]=entityPlacerEntity;
		if (!entityPlacerUseon.empty()) item.val_object["minecraft:item"].val_object["components"].val_object["minecraft:entity_placer"].val_object["use_on"]=entityPlacerUseon;
		if (!entityPlacerDison.empty()) item.val_object["minecraft:item"].val_object["components"].val_object["minecraft:entity_placer"].val_object["dispense_on"]=entityPlacerDison;
	}
	if (enchantableCheckbox){
		item.val_object["minecraft:item"].val_object["components"].val_object["minecraft:enchantable"]=json_value(json_object<json_value>{});
		if (enchantableSlot!="none") item.val_object["minecraft:item"].val_object["components"].val_object["minecraft:enchantable"].val_object["slot"]=enchantableSlot;
		if (enchantableValue!=-1) item.val_object["minecraft:item"].val_object["components"].val_object["minecraft:enchantable"].val_object["value"]=enchantableValue;
	}
	if (shooterCheckbox){
		item.val_object["minecraft:item"].val_object["components"].val_object["minecraft:shooter"]=json_value(json_object<json_value>{});
		if (shooterMaxDraw>=1) item.val_object["minecraft:item"].val_object["components"].val_object["minecraft:shooter"].val_object["max_draw_duration"]=shooterMaxDraw;
		item.val_object["minecraft:item"].val_object["components"].val_object["minecraft:shooter"].val_object["charge_on_draw"]=shooterCharge;
		item.val_object["minecraft:item"].val_object["components"].val_object["minecraft:shooter"].val_object["scale_power_by_draw_duration"]=shooterScale;
		if (shooterAmmoCheckbox){
			item.val_object["minecraft:item"].val_object["components"].val_object["minecraft:shooter"].val_object["ammunition"]=json_value(std::vector<json_value>{json_object<json_value>{}});
			item.val_object["minecraft:item"].val_object["components"].val_object["minecraft:shooter"].val_object["ammunition"].val_list[0].val_object["search_inventory"]=shooterAmmoInven;
			if (shooterAmmoItem!="") item.val_object["minecraft:item"].val_object["components"].val_object["minecraft:shooter"].val_object["ammunition"].val_list[0].val_object["item"]=shooterAmmoItem;
			item.val_object["minecraft:item"].val_object["components"].val_object["minecraft:shooter"].val_object["ammunition"].val_list[0].val_object["use_offhand"]=shooterAmmoHand;
			item.val_object["minecraft:item"].val_object["components"].val_object["minecraft:shooter"].val_object["ammunition"].val_list[0].val_object["use_in_creative"]=shooterAmmoCreative;
		}
	}
	if (fertilizer>=0){
		item.val_object["minecraft:item"].val_object["components"].val_object["minecraft:fertilizer"]=json_value(json_object<json_value>{});
		item.val_object["minecraft:item"].val_object["components"].val_object["minecraft:fertilizer"].val_object["duration"]=fertilizer;
	}
	if (ignPerms){
		item.val_object["minecraft:item"].val_object["components"].val_object["minecraft:ignores_permission"]=ignPerms;
	}
	if (miningSpeed>=0) item.val_object["minecraft:item"].val_object["components"].val_object["minecraft:mining_speed"]=miningSpeed;
	if (liquidClipped) item.val_object["minecraft:item"].val_object["components"].val_object["minecraft:liquid_clipped"]=liquidClipped;
	if (!shouldDespawn) item.val_object["minecraft:item"].val_object["components"].val_object["minecraft:should_despawn"]=shouldDespawn;
	if (mirroredArt) item.val_object["minecraft:item"].val_object["components"].val_object["minecraft:mirrored_art"]=mirroredArt;
	if (projectileCheckbox){
		item.val_object["minecraft:item"].val_object["components"].val_object["minecraft:projectile"]=json_value(json_object<json_value>{});
		if (projectileEntity!="") item.val_object["minecraft:item"].val_object["components"].val_object["minecraft:projectile"].val_object["projectile_entity"]=projectileEntity;
		if (projectilePower>=0) item.val_object["minecraft:item"].val_object["components"].val_object["minecraft:projectile"].val_object["minimum_critical_power"]=projectilePower;
	}
	if (dyeCheckbox){
		item.val_object["minecraft:item"].val_object["components"].val_object["minecraft:dye_powder"]=json_value(json_object<json_value>{});
		item.val_object["minecraft:item"].val_object["components"].val_object["minecraft:dye_powder"].val_object["color"]=dyePowder;
	}
	if (throwableCheckbox){
		item.val_object["minecraft:item"].val_object["components"].val_object["minecraft:throwable"]=json_value(json_object<json_value>{});
		item.val_object["minecraft:item"].val_object["components"].val_object["minecraft:throwable"].val_object["do_swing_animation"]=throwableSwing;
		if (throwableDraw>=0) item.val_object["minecraft:item"].val_object["components"].val_object["minecraft:throwable"].val_object["max_draw_duration"]=throwableDraw;
		item.val_object["minecraft:item"].val_object["components"].val_object["minecraft:throwable"].val_object["scale_power_by_draw_duration"]=throwablePower;
	}
	if (knockbackResis>=0&&knockbackResis<=1){
		item.val_object["minecraft:item"].val_object["components"].val_object["minecraft:knockback_resistance"]=json_value(json_object<json_value>{});
		item.val_object["minecraft:item"].val_object["components"].val_object["minecraft:knockback_resistance"].val_object["protection"]=knockbackResis;
	}
	if (fuel>=0){
		item.val_object["minecraft:item"].val_object["components"].val_object["minecraft:fuel"]=json_value(json_object<json_value>{});
		item.val_object["minecraft:item"].val_object["components"].val_object["minecraft:fuel"].val_object["duration"]=fuel;
	}
	if (recordCheckbox){
		item.val_object["minecraft:item"].val_object["components"].val_object["minecraft:record"]=json_value(json_object<json_value>{});
		if (recordSound!="") item.val_object["minecraft:item"].val_object["components"].val_object["minecraft:record"].val_object["sound_event"]=recordSound;
		if (recordDuration>=0) item.val_object["minecraft:item"].val_object["components"].val_object["minecraft:record"].val_object["duration"]=recordDuration;
		if (recordSignal>=0&&recordSignal<=15) item.val_object["minecraft:item"].val_object["components"].val_object["minecraft:record"].val_object["comparator_signal"]=recordSignal;
	}
	std::string name;
	if (identifier!="") name=get_iname(identifier)+".mcaddon";
	else name="item.mcaddon";
	json_value p=genManifest();
	if (itemIcon.filename!=""){
		itemIcon.filename=truncate_slashes(to_lower(icon)+".png");
		file zip(name,std::vector<file>{
			file("resource",std::vector<file>{
				file("manifest.json",p.val_object["rp"].get_repr()),
				file("textures",std::vector<file>{
					file("item_texture.json",itemAtlas.get_repr())
				}),
				itemIcon
			}),
			file("behavior",std::vector<file>{
				file("manifest.json",p.val_object["bp"].get_repr()),
				file("items",std::vector<file>{
					file(get_iname(identifier)+".item.json",item.get_repr())
				})
			})
		});
		return zip;
	}
	else{
		file zip(name,std::vector<file>{
			file("resource",std::vector<file>{
				file("manifest.json",p.val_object["rp"].get_repr())
			}),
			file("behavior",std::vector<file>{
				file("manifest.json",p.val_object["bp"].get_repr()),
				file("items",std::vector<file>{
					file(get_iname(identifier)+".item.json",item.get_repr())
				})
			})
		});
		return zip;
	}
}