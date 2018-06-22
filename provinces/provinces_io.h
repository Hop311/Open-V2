#pragma once
#include "common\\common.h"
#include "provinces.h"
#include "simple_serialize\\simple_serialize.hpp"
#include "simple_fs\\simple_fs.h"
#include "Parsers\\parsers.hpp"
#include "text_data\\text_data.h"
#include <ppl.h>
#include <map>

template<>
class serialization::serializer<provinces::province> : public serialization::memcpy_serializer<provinces::province> {};

template<>
class serialization::serializer<provinces::province_manager> {
public:
	static constexpr bool has_static_size = false;
	static constexpr bool has_simple_serialize = false;

	static void rebuild_indexes(provinces::province_manager& obj) {
		for(int32_t i = static_cast<int32_t>(obj.state_names.size()) - 1; i >= 0; --i)
			obj.named_states_index.emplace(obj.state_names[provinces::state_tag(static_cast<provinces::state_tag::value_base_t>(i))], provinces::state_tag(static_cast<provinces::state_tag::value_base_t>(i)));
		for(auto const& i_prov : obj.province_container)
			obj.states_to_province_index.emplace(i_prov.state_id, i_prov.id);
	}

	static void serialize_object(std::byte* &output, provinces::province_manager const& obj) {
		serialize(output, obj.province_container);
		serialize(output, obj.state_names);
		serialize(output, obj.same_type_adjacency);
		serialize(output, obj.coastal_adjacency);
		serialize(output, obj.canals);
		serialize(output, obj.province_map_data);
		serialize(output, obj.province_map_width);
		serialize(output, obj.province_map_height);
	}
	static void deserialize_object(std::byte const* &input, provinces::province_manager& obj) {
		deserialize(input, obj.province_container);
		deserialize(input, obj.state_names);
		deserialize(input, obj.same_type_adjacency);
		deserialize(input, obj.coastal_adjacency);
		deserialize(input, obj.canals);
		deserialize(input, obj.province_map_data);
		deserialize(input, obj.province_map_width);
		deserialize(input, obj.province_map_height);

		rebuild_indexes(obj);
	}
	static void deserialize_object(std::byte const* &input, provinces::province_manager& obj, concurrency::task_group& tg) {
		deserialize(input, obj.province_container);
		deserialize(input, obj.state_names);
		deserialize(input, obj.same_type_adjacency);
		deserialize(input, obj.coastal_adjacency);
		deserialize(input, obj.canals);
		deserialize(input, obj.province_map_data);
		deserialize(input, obj.province_map_width);
		deserialize(input, obj.province_map_height);

		tg.run([&obj]() { rebuild_indexes(obj); });
	}
	static size_t size(provinces::province_manager const& obj) {
		return serialize_size(obj.province_container) +
			serialize_size(obj.state_names) +
			serialize_size(obj.same_type_adjacency) +
			serialize_size(obj.coastal_adjacency) +
			serialize_size(obj.canals) +
			serialize_size(obj.province_map_data) +
			serialize_size(obj.province_map_width) +
			serialize_size(obj.province_map_height);
	}
};

namespace provinces {
	struct parsing_environment;

	class parsing_state {
	public:
		std::unique_ptr<parsing_environment> impl;

		parsing_state(text_data::text_sequences& tl, province_manager& m, modifiers::modifiers_manager& mm);
		parsing_state(parsing_state&&) noexcept;
		~parsing_state();
	};

	void read_default_map_file(
		parsing_state& state,
		const directory& source_directory);

	color_to_terrain_map
		read_terrain(parsing_state& state, const directory& source_directory); // returns color to terrain tag map; adds provincial modifiers
	
	tagged_vector<uint8_t, province_tag> generate_province_terrain(size_t province_count, uint16_t const* province_map_data, uint8_t const* terrain_color_map_data, int32_t height, int32_t width);
	tagged_vector<uint8_t, province_tag> generate_province_terrain_inverse(size_t province_count, uint16_t const* province_map_data, uint8_t const* terrain_color_map_data, int32_t height, int32_t width);
	tagged_vector<uint8_t, province_tag> load_province_map_data(province_manager& m, directory const& root); // returns province to terrain color array
	void assign_terrain_color(province_manager& m, tagged_vector<uint8_t, province_tag> const & terrain_colors, color_to_terrain_map const & terrain_map);
	
	std::map<province_tag, boost::container::flat_set<province_tag>> generate_map_adjacencies(uint16_t const* province_map_data, int32_t height, int32_t width);
	void read_adjacnencies_file(std::map<province_tag, boost::container::flat_set<province_tag>>& adj_map, std::vector<std::pair<province_tag, province_tag>>& canals, directory const& root);
	void make_lakes(std::map<province_tag, boost::container::flat_set<province_tag>>& adj_map, province_manager& m);
	void make_adjacency(std::map<province_tag, boost::container::flat_set<province_tag>>& adj_map, province_manager& m);

	void read_states(
		parsing_state& state,
		const directory& source_directory);
	void read_continents(
		parsing_state& state,
		const directory& source_directory); // adds provincial modifiers
	void read_climates(
		parsing_state& state,
		const directory& source_directory); // adds provincial modifiers
	boost::container::flat_map<uint32_t, province_tag> read_province_definition_file(directory const& source_directory);
}