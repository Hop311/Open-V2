#include "provinces\\provinces.h"
#include "gtest\\gtest.h"
#include "fake_fs\\fake_fs.h"
#include "modifiers\\modifiers.h"

#define RANGE(x) (x), (x) + (sizeof((x))/sizeof((x)[0])) - 1

class preparse_test_files {
public:
	directory_representation f_root = directory_representation(u"F:");
	directory_representation map_dir = directory_representation(u"map", f_root);

	file_representation a = file_representation(u"default.map", map_dir,
		"max_provinces = 5\r\n"
		"sea_starts = { 1 4 }\r\n"
		"definitions = \"definition.csv\"\r\n"
		"terrain_sheet_heights = { 500 }");
	file_representation b = file_representation(u"climate.txt", map_dir,
		"mild_climate = {\r\n"
		"value = 0\r\n"
		"}\r\n"
		"temperate_climate  = {\r\n"
		"value = 1\r\n"
		"}\r\n"
		"mild_climate = { 1 2 3 }\r\n"
		"temperate_climate = { 0 4 }\r\n");
	file_representation c = file_representation(u"continent.txt", map_dir,
		"europe = {\r\n"
		"\tprovinces = {3 4}\r\n"
		"\tvalue = 0.0\r\n"
		"}\r\n"
		"asia = {\r\n"
		"\tprovinces = { 1 2 }\r\n"
		"\tvalue = 6\r\n"
		"}\r\n");
	file_representation d = file_representation(u"region.txt", map_dir,
		"region_a = { 1 3 }\r\n"
		"region_b = { 2 4 }");
	file_representation e = file_representation(u"terrain.txt", map_dir,
		"terrain = 64\r\n"
		"categories = {\r\n"
		"ocean = { \r\n"
		"	movement_cost = 1.0\r\n"
		"	is_water = yes\r\n"
		"color = { 0 0 255 }\r\n"
		"}\r\n"
		"arctic = { \r\n"
		"	movement_cost = 1.0\r\n"
		"	color = { 235 235 235 }\r\n"
		"farm_rgo_size = -0.5\r\n"
		"farm_rgo_eff = -0.2\r\n"
		"mine_rgo_size = -0.0\r\n"
		"mine_rgo_eff = -0.2\r\n"
		"min_build_railroad = 1\r\n"
		"}\r\n"
		"}\r\n"
		"junk = { stuff = x color = { 5 } priority = 	0 }");


	preparse_test_files() {
		set_default_root(f_root);
	}
};

inline auto fake_text_handle_lookup(std::map<std::string, text_data::text_tag>& values) {
	return[j = 0ui16, &values](const char* s, const char* e) mutable {
		const auto i = std::string(s, e);
		if (values.find(i) == values.end()) {
			values[i] = text_data::text_tag(j++);
		}
		return values[i];
	};
}

using namespace provinces;
using namespace modifiers;

TEST(provinces_test, default_map_read) {
	preparse_test_files real_fs;
	file_system f;

	f.set_root(RANGE(u"F:"));

	province_manager m;
	modifiers_manager mm;
	std::map<std::string, text_data::text_tag> values;

	provinces::parsing_state state(fake_text_handle_lookup(values), m, mm);

	read_default_map_file(state, f.get_root());

	EXPECT_EQ(5ui64, m.province_container.size());
	EXPECT_EQ(province_tag(0), m.province_container[province_tag(0)].id);
	EXPECT_EQ(0ui16, m.province_container[province_tag(0)].flags);
	EXPECT_EQ(province::sea, m.province_container[province_tag(1)].flags);
	EXPECT_EQ(province::sea, m.province_container[province_tag(4)].flags);
}

TEST(provinces_test, climate_preparse) {
	preparse_test_files real_fs;
	file_system f;

	f.set_root(RANGE(u"F:"));

	province_manager m;
	modifiers_manager mm;
	std::map<std::string, text_data::text_tag> values;

	provinces::parsing_state state(fake_text_handle_lookup(values), m, mm);

	read_default_map_file(state, f.get_root());
	pre_parse_climates(state, f.get_root());

	EXPECT_EQ(2ui64, mm.provincial_modifiers.size());
	EXPECT_EQ(provincial_modifier_tag(0), mm.provincial_modifiers[provincial_modifier_tag(0)].id);
	EXPECT_EQ(provincial_modifier_tag(1), mm.provincial_modifiers[provincial_modifier_tag(1)].id);
	EXPECT_EQ(provincial_modifier_tag(0), mm.named_provincial_modifiers_index[mm.provincial_modifiers[provincial_modifier_tag(0)].name]);
	EXPECT_EQ(provincial_modifier_tag(1), mm.named_provincial_modifiers_index[values[std::string("temperate_climate")]]);

	EXPECT_EQ(provincial_modifier_tag(1), m.province_container[province_tag(0)].climate);
	EXPECT_EQ(provincial_modifier_tag(1), m.province_container[province_tag(4)].climate);

	EXPECT_EQ(provincial_modifier_tag(0), m.province_container[province_tag(1)].climate);
	EXPECT_EQ(provincial_modifier_tag(0), m.province_container[province_tag(2)].climate);
	EXPECT_EQ(provincial_modifier_tag(0), m.province_container[province_tag(3)].climate);
}

TEST(provinces_test, continent_preparse) {
	preparse_test_files real_fs;
	file_system f;

	f.set_root(RANGE(u"F:"));

	province_manager m;
	modifiers_manager mm;
	std::map<std::string, text_data::text_tag> values;

	provinces::parsing_state state(fake_text_handle_lookup(values), m, mm);

	read_default_map_file(state, f.get_root());
	pre_parse_continents(state, f.get_root());

	EXPECT_EQ(2ui64, mm.provincial_modifiers.size());
	EXPECT_EQ(provincial_modifier_tag(0), mm.provincial_modifiers[provincial_modifier_tag(0)].id);
	EXPECT_EQ(provincial_modifier_tag(1), mm.provincial_modifiers[provincial_modifier_tag(1)].id);
	EXPECT_EQ(provincial_modifier_tag(0), mm.named_provincial_modifiers_index[values[std::string("europe")]]);
	EXPECT_EQ(provincial_modifier_tag(1), mm.named_provincial_modifiers_index[mm.provincial_modifiers[provincial_modifier_tag(1)].name]);

	EXPECT_EQ(provincial_modifier_tag(1), m.province_container[province_tag(1)].continent);
	EXPECT_EQ(provincial_modifier_tag(1), m.province_container[province_tag(2)].continent);

	EXPECT_EQ(provincial_modifier_tag(0), m.province_container[province_tag(3)].continent);
	EXPECT_EQ(provincial_modifier_tag(0), m.province_container[province_tag(4)].continent);
}

TEST(provinces_test, terrain_preparse) {
	preparse_test_files real_fs;
	file_system f;

	f.set_root(RANGE(u"F:"));

	province_manager m;
	modifiers_manager mm;
	std::map<std::string, text_data::text_tag> values;

	provinces::parsing_state state(fake_text_handle_lookup(values), m, mm);

	read_default_map_file(state, f.get_root());
	auto t_map = pre_parse_terrain(state, f.get_root());

	EXPECT_EQ(2ui64, mm.provincial_modifiers.size());
	EXPECT_EQ(provincial_modifier_tag(0), mm.provincial_modifiers[provincial_modifier_tag(0)].id);
	EXPECT_EQ(provincial_modifier_tag(1), mm.provincial_modifiers[provincial_modifier_tag(1)].id);
	EXPECT_EQ(provincial_modifier_tag(0), mm.named_provincial_modifiers_index[values[std::string("ocean")]]);
	EXPECT_EQ(provincial_modifier_tag(1), mm.named_provincial_modifiers_index[mm.provincial_modifiers[provincial_modifier_tag(1)].name]);

	const auto va = t_map[graphics::rgb_to_int(graphics::color_rgb{ 0ui8, 0ui8, 255ui8 })];
	EXPECT_EQ(provincial_modifier_tag(0), va);
	const auto vb = t_map[graphics::rgb_to_int(graphics::color_rgb{ 235ui8, 235ui8, 235ui8 })];
	EXPECT_EQ(provincial_modifier_tag(1), vb);
}

TEST(provinces_test, region_read) {
	preparse_test_files real_fs;
	file_system f;

	f.set_root(RANGE(u"F:"));

	province_manager m;
	modifiers_manager mm;
	std::map<std::string, text_data::text_tag> values;

	provinces::parsing_state state(fake_text_handle_lookup(values), m, mm);

	read_default_map_file(state, f.get_root());
	read_states(state, f.get_root());

	EXPECT_EQ(2ui64, m.state_names.size());
	EXPECT_EQ(2ui64, m.named_states_index.size());

	EXPECT_EQ(state_tag(), m.province_container[province_tag(0)].state_id);
	EXPECT_EQ(state_tag(0), m.province_container[province_tag(1)].state_id);
	EXPECT_EQ(state_tag(1), m.province_container[province_tag(2)].state_id);
	EXPECT_EQ(state_tag(0), m.province_container[province_tag(3)].state_id);
	EXPECT_EQ(state_tag(1), m.province_container[province_tag(4)].state_id);

	bool tagged_provs[5] = { false };
	for (auto s1_range = m.states_to_province_index.equal_range(state_tag(0)); s1_range.first != s1_range.second; ++s1_range.first) {
		tagged_provs[to_index(s1_range.first->second)] = true;
	}
	EXPECT_EQ(false, tagged_provs[0]);
	EXPECT_EQ(true, tagged_provs[1]);
	EXPECT_EQ(false, tagged_provs[2]);
	EXPECT_EQ(true, tagged_provs[3]);
	EXPECT_EQ(false, tagged_provs[4]);

	bool tagged_provs_b[5] = { false };
	for (auto s1_range = m.states_to_province_index.equal_range(state_tag(1)); s1_range.first != s1_range.second; ++s1_range.first) {
		tagged_provs_b[to_index(s1_range.first->second)] = true;
	}
	EXPECT_EQ(false, tagged_provs_b[0]);
	EXPECT_EQ(false, tagged_provs_b[1]);
	EXPECT_EQ(true, tagged_provs_b[2]);
	EXPECT_EQ(false, tagged_provs_b[3]);
	EXPECT_EQ(true, tagged_provs_b[4]);
	
	EXPECT_EQ(values[std::string("region_a")], m.state_names[state_tag(0)]);
	EXPECT_EQ(values[std::string("region_b")], m.state_names[state_tag(1)]);
}