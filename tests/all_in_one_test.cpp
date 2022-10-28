#include "../generate_libs/generate_libs.h"

using namespace std;
using namespace generatelibs;

int main(){
	// Entity generator test
	entity entity_a;
	file entity_tf,entity_mf;
	entity_tf.read_file("resources/Ę.png",1);
	entity_mf.read_file("resources/model.json",0);
	file entity_b=entity_a.make("ss:aa",entity_tf,entity_mf,1,1,0,"#7b3d3d","#ffa8a8","entity_alphatest","both");
	string entity_c=entity_b.get_repr(0);
	std::cout << entity_c << endl;
	// File management test
	file file_a("think",{
		file("ssss",vector<file>{
			file("a","moi"),
			file("fldr",vector<file>{
				file("sh","iet"),
				file("muzyka","aasasas")
			}),
			file("b","móc")
		}),
		file("must","fffff")
	});
	cout << "Zip filename for the Entity Generator test: " << entity_b.make_zip() << endl;
	cout << file_a.get_repr(0) << endl;

	// Item generator test
	file tf;
	tf.read_file("/mnt/ltinh/młośpąc.png",1);
	item item_a;
	cout << item_a.writeItem(
		"1.17.0", "test:test1", "Equipment", false, "textures/items/test1",
		"Test #1", 0, true, "eat", false, 3.0, false, true, true, true,
		"itemGroup.name.crop", false, -1.0, "low", false, "", false, true,
		true, "slot.armor.head", 12, false, "", -1.0, 3, false, "", "",
		true, "d", vector<json_value>{string("a"),string("b"),string("c")},
		vector<json_value>{string("d"),string("e"),string("f")}, false,
		"none", -1, true, 2.0, true, false, true, false, "sss", false, true,
		-1, true, -1, true, true, true, false, "", -1.0, true, "magenta", false,
		false, -1.0, false, 0.5, -1.0, true, "słuchać", 1, 4, file("",""), false
	).get_repr(0) << endl;
	file item_b=item_a.writeItem(
		"1.16.100", "test:test2", "Items", true, "textures/items/test2",
		"Test #2", 16, false, "none", true, 0.0, true, false, false, false,
		"none", true, 2.43, "normal", false, "eater", true, false, false, "", -1,
		true, "sab", 3.3, -1, true, "arb", true, false, "", vector<json_value>{},
		vector<json_value>{}, true, "armor_legs", -2, false, 0.0, false, false,
		false, false, "", false, false, 2, false, 3, false, false, false, true,
		"dd", 5.3, false, "", true, true, 2.0, true, -1.0, 3.4, false, "", -1,
		-1, tf, true
	);
	cout << "Representation for the Item Generator test:" << endl << item_b.get_repr() << endl;
	cout << "Zip filename for the Item Generator test: " << item_b.make_zip(".mcaddon") << endl;

	// JSON generation test
	json_value float_representation_n=json_value(json_object<json_value>{});
	float_representation_n.val_object["test"]=json_value(11.44);
	float_representation_n.val_object["test2"]=json_value(string("car"));
	cout << float_representation_n.get_repr() << endl;
	json_object<json_value> make_json_o;
	make_json_o.object_pair={{"a",json_value(1)},{"c",json_value(2)}};
	make_json_o["c"]=3;
	make_json_o["b"]=999999999999999999ll;
	json_value make_json_v=json_value(vector<json_value>{json_value(make_json_o),json_value(false),json_value(85.47)});
	cout << make_json_v.get_repr() << endl;

	// Manifest generator test
	manifest manifest_n;
	cout << "Non-dependent:\n" << manifest_n.generate_nondependent(
		"data",
		"abv",
		"sontdfdfdf",
		{1,17,0},
		true,
		{string("aa"),string("bb"),string("cc")},
		"http://localhost",
		1
	) << endl;
	vector<string> manifest_r=manifest_n.generate_dependent(
		"abv",
		"d🤩ixneuf",
		vector<json_value> {1,17,0},
		true,
		true,
		vector<json_value> {string("aiw"),string("aed"),string("ddd")},
		"http://localhost",
		"rpbv",
		"dikneuph",
		vector<json_value> {1,17,0},
		true,
		vector<json_value> {string("rpw"),string("ddd"),string("wwddd")},
		"http://192.168.32.1",
		true,
		true);
	cout << "Dependent: \n" << manifest_r[0] << endl << manifest_r[1] << endl;
	// JSON obfuscator test
	obfuscator obfuscator_a;
	file obfuscator_json_file;
	obfuscator_json_file.read_file("resources/obfuscator_test.json",0);
	string obfuscator_s=obfuscator_json_file.text_contents;
	cout << "Result:\n" << obfuscator_a.obfuscateMain(obfuscator_s,0) << "\nResult (minified): " << obfuscator_a.obfuscateMain(obfuscator_s,1) << "\n";
	// UUID generator test
	uuid_generator uuid_generator_a;
	cout << uuid_generator_a.generate_uuid() << endl;
	return 0;
}