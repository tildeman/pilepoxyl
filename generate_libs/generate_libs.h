#include "make_json.h"
#include "file_management.h"

namespace generatelibs{
	// Generates entity files with only basic behaviors
	class entity{
	private:
		// Check whether the given identifier is valid
		bool verifyIdentifier(std::string identifier);
		// Get the entity name with the given identifier
		std::string get_ename(std::string identifier);
		// Strip the ".png" extention of the filename, if present
		std::string strippng(std::string filename);
		// Convert a string to lowercase, lacks support beyond the US-ASCII range
		std::string to_lower(std::string a);
		// Get your geolocation ID and then dox the heck out of you
		// Okay I was joking it's just a model file parser
		std::string getGeo(std::string model_json);
		// Generate the associated manifest files
		json_value genManifest(bool select_both);
		// Get the render controllers, keys are appended by the given ID
		json_value genRenderControllers(std::string idv1);
		// Generate basic behavior data
		json_value genBehBasic(std::string identifier,bool isSpawnable,bool isSummonable,bool isExperimental);
	public:
		// Generate an MCBE entity
		file make(
			std::string identifier, file textureFile, file modelFile,
			bool isSpawnable, bool isSummonable, bool isExperimental,
			std::string spawnEggOverlay, std::string spawnEggBase,
			std::string material, std::string select
		);
	};

	// Messes around your JSON file according to RFC 7159
	// File wrapper needed
	class obfuscator{
	public:
		// Evaluate a multibyte UTF-8 character, returns the character code point
		int decode_utf8(std::string embestring);
		// Escape data in UTF-8 format
		std::string unicodeEscape(std::string input_string);
		// Strip any spaces generated through the JSON library
		std::string minify(std::string input_string);
		// Obfuscate a JSON file in accordance to RFC 8259
		std::string obfuscateMain(std::string input_string, bool do_minify);
	};

	// The most tiresome generator to use
	class item{
	private:
		// Check whether the given identifier is valid
		bool verifyIdentifier(std::string identifier);
		// Convert a string to lowercase, lacks support beyond the US-ASCII range
		std::string to_lower(std::string a);
		// Generate the associated manifest files
		json_value genManifest();
		// Truncate anything before and including the last forward slash
		std::string truncate_slashes(std::string strink);
	public:
		// Generate an MCBE item
		file writeItem(
			std::string formatVersion="1.16.100", std::string identifier="custom:item",
			std::string category="Items", bool isExp=0, std::string icon="textures/items/my_item",
			std::string displayName="", long long maxStackSize=0, bool useAnimCheckbox=0,
			std::string useAnim="none", bool foil=0, double useDuration=32, bool stackedData=0,
			bool handEquipped=0, bool destroyInC=1, bool ccategoryCheckbox=0,
			std::string ccategory="none", bool foodCheckbox=0, double foodNutr=-1,
			std::string foodSatu="low", bool foodCanA=0, std::string foodConvert="", bool isexplod=1,
			bool allowOffHand=1, bool wearableCheckbox=0, std::string wearableSlot="none",
			long long armorProtection=-1, bool cooldownCheckbox=0, std::string cooldownCategory="",
			double cooldownDuration=-1, long long damage=-1, bool blockPlacerCheckbox=0,
			std::string blockPlacerBlock="", bool blockPlacerDes=0, bool entityPlacerCheckbox=0,
			std::string entityPlacerEntity="", std::vector<json_value> entityPlacerUseon={},
			std::vector<json_value> entityPlacerDison={}, bool enchantableCheckbox=0,
			std::string enchantableSlot="none", long long enchantableValue=-1, bool shooterCheckbox=0,
			double shooterMaxDraw=0, bool shooterCharge=0, bool shooterScale=0,
			bool shooterAmmoCheckbox=0, bool shooterAmmoInven=0, std::string shooterAmmoItem="",
			bool shooterAmmoHand=0, bool shooterAmmoCreative=0, long long fertilizer=-1,
			bool ignPerms=0, long long miningSpeed=-1, bool liquidClipped=0, bool shouldDespawn=1,
			bool mirroredArt=0, bool projectileCheckbox=0, std::string projectileEntity="",
			double projectilePower=-1, bool dyeCheckbox=0, std::string dyePowder="silver",
			bool throwableCheckbox=0, bool throwableSwing=0, double throwableDraw=-1,
			bool throwablePower=0, double knockbackResis=-1, double fuel=-1,
			bool recordCheckbox=0, std::string recordSound="", long long recordDuration=-1,
			long long recordSignal=-1, file itemIcon=file("",""), bool enable_itematlas=0
		);
		std::string get_iname(std::string identifier);
	};

	// Metadata generator, split into two separate functions/commands
	// File wrapper needed
	class manifest{
	public:
		// Generate an independent manifest for a script, behavior, resource or skin pack
		std::string generate_nondependent(
			std::string pack_type="data", std::string pack_name="", std::string pack_description="",
			std::vector<json_value> pack_version={1,17,0}, bool pack_use_metadata=0,
			std::vector<json_value> pack_authors={std::string("")}, std::string pack_author_url="",
			bool pack_use_gametest=0, std::string pack_gametest_entry_file="modules/Main.js"
		);
		// Generate a combined behavior and resource manifest pack with extra options
		std::vector<std::string> generate_dependent(
			std::string bp_name="", std::string bp_description="",
			std::vector<json_value> bp_version={1,17,0}, bool bp_use_scripts=0,
			bool bp_use_metadata=0, std::vector<json_value> bp_authors={std::string("")},
			std::string bp_author_url="", std::string rp_name="", std::string rp_description="",
			std::vector<json_value> rp_version={1,17,0}, bool rp_use_metadata=0,
			std::vector<json_value> rp_authors={std::string("")}, std::string rp_author_url="",
			bool rp_use_ui=0, bool bp_use_gametest=0,
			std::string bp_gametest_entry_file="modules/Main.js"
		);
	};

	// Split a string delimited by 'delim' into a std::vector
	std::vector<json_value> split_authors(std::string striq,char delim=',');
	// Split a period-delimited string into a std::vector, then turn the elements to integers
	std::vector<json_value> split_version(std::string striq);
}