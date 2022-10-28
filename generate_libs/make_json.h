/**
 * The clunkiest and least efficient JSON generator in the world
 * Features:
 *   - Little to no overflow checks
 *   - Inefficient objects
 *   - Clunky data only for polymorphism
 * Written by: tildeman123
*/

#ifndef mkjson
#define mkjson

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <regex>
#include <cmath>

template <typename jso> class json_object{
public:
	std::vector<std::pair<std::string,jso>> object_pair;
	json_object();
	json_object(std::vector<std::pair<std::string,jso>> a);
	jso& operator[](std::string s);
};

// ********************************
// * Lookup table for value types *
// ********************************
// +---------+------------+
// | Type ID |  Used for  |
// +---------+------------+
// |    0    |   Integer  |
// +---------+------------+
// |    1    |   String   |
// +---------+------------+
// |    2    |    List    |
// +---------+------------+
// |    3    |   Object   |
// +---------+------------+
// |    4    | Enumerated |
// +---------+------------+
// |    5    |   Double   |
// +---------+------------+
// |    6    | 64-bit int |
// +---------+------------+

// Represents a value in JSON
class json_value{
private:
	std::string indent_json(std::string s);
	static std::string strip(std::string s);
	static int hex_to_int(std::string s);
	//Kept me up for a whole night
	static std::string unescapify(std::string s);
public:
	int val_type;
	int val_int;
	long long val_llint;
	double val_float;
	std::string val_string;
	std::vector<json_value> val_list;
	json_object<json_value> val_object;
	// An integer with value 0
	json_value();
	// An integer with value "n"
	json_value(int n);
	// A std::string with value "s"
	json_value(std::string s);
	// A C string with value "s"
	json_value(const char* s);
	// A list "l" of values
	json_value(std::vector<json_value> l);
	// An object "o" of values
	json_value(json_object<json_value> o);
	// A boolean with value "n". Represents "null" when val_int is modified to -1
	json_value(bool n);
	// A float with value "d"
	json_value(double d);
	// A 64-bit integer with value "n"
	json_value(long long n);
	/*
	A JSON output mini-function made by neither JustAsh nor Zarkmend.
	Refer to RFC 8259 if you want to know more about what this is.
	I mean it runs terribly slow on large files but it gets the job done.
	(as long as Discord doesn't chuck in some impossibly long request)
	*/
	std::string get_repr();
	/*
	A JSON parsing mini-function made by neither JustAsh nor Zarkmend.
	Refer to RFC 8259 if you want to know more about what this is.
	Discord can still chuck in 8765432101112131, so a case for 64-bit
	integers is included (sneakily as to not break older API).
	*/
	static json_value parse(std::string s);
	/**
	 * Retrieves a number from a json_value object of long long type.
	*/
	long long lick();
	/**
	 * Retrieves a number from a json_value object of double type.
	*/
	double ding();
};

#endif