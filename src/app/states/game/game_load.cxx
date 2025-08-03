module;
#include <filesystem>
#include <fstream>
#include <string>
#include <map>

module state.game;
import camera;
import console;
import entity.bug;
import entity.frog;
import entity.conduit;
import entity.flag;
import entity.lever;
import entity.logic_and;
import entity.logic_not;
import entity.logic_or;
import entity.spring;
import entity.train;
import entity.track;
import level_config;

namespace state {
    std::map<u16, std::string> Game::load_types_from_bin() {
        std::map<u16, std::string> types_map;

        const std::filesystem::path path = std::filesystem::current_path() / "res" / "types.bin";

        std::ifstream in_file(path, std::ios::in | std::ios::binary);
        if (!in_file.is_open()) return {};

        u16 type_count = 0;
        in_file.read((i8*)&type_count, sizeof(u16));

        Console::log("state::Game::load_types_from_bin count: ", type_count, "\n");

        for (u16 i = 0; i < type_count; ++i) {
            u16 number = 0;
            char type[32] = {};
            in_file.read((i8*)&number, sizeof(u16));
            in_file.read((i8*)type, sizeof(char) * 32);

            //Console::log("type: ", number, " ", type, "\n");

            types_map.emplace(number, type);

        }
        in_file.close();
        return types_map;
    }

    void Game::load_level(const std::filesystem::path& path) {
        Console::log("Game::load_level\n");

        std::map<u16, std::string> types_map = load_types_from_bin();

        std::vector<sprite::Data> sprite_data = sprite::Set::load_level(path);

        Vec2i highest_quad_node = { 0, 0 };

        std::vector<Vec2i> quad_nodes;

        for (auto& i : sprite_data) {
            Vec2fc tile_offset = { (f32)i.x, (f32)i.y };
            if (i.tile_set != 255) {
                size_t sprite_id = sprite::Set::make(tile_set_texture_path(i.tile_set));
                sprite::Set::at(sprite_id)->id = sprite_id;
                sprite::Set::at(sprite_id)->tile_set = i.tile_set;
                sprite::Set::at(sprite_id)->layer = i.layer;
                sprite::Set::at(sprite_id)->offset = tile_offset;
                sprite::Set::at(sprite_id)->source_rect = Rect<i32>{ i.source_x, i.source_y, 16, 16 };
                sprite::Set::at(sprite_id)->transform_id = m_level_transform_id;
            } else if (i.tile_set == 255) {
                u16c tile_x = i.source_x / 16;
                u16c tile_y = i.source_y / 16;
                u16c tile_number = tile_x + tile_y * 32;
                if (types_map.count(tile_number)) {
                    entity::Type entity_type = entity::type_from_string(types_map.at(tile_number));

                    //Console::log("entity_type: ", types_map.at(tile_number), "\n");

                    if (start_info().type == start::Type::center && entity_type == entity::Type::level_center) {
                        //Console::log("state::Game::load_level start: ", sprite::to_string(sprite::Set::at(sprite_id)->type), " position: ", m_start_position.x, " ", m_start_position.y, "\n");
                        m_start_position = tile_offset;
                        m_player.sprite()->layer = i.layer + 1;
                    } else if (start_info().type == start::Type::L_0 && entity_type == entity::Type::level_L_0) {
                        if (tile_offset.y > m_start_position.y) {
                            m_start_position = tile_offset;
                        }
                        m_player.sprite()->layer = i.layer + 1;
                        m_player.sprite()->is_leftward = false;
                    } else if (start_info().type == start::Type::R_0 && entity_type == entity::Type::level_R_0) {
                        if (tile_offset.y > m_start_position.y) {
                            m_start_position = tile_offset;
                        }
                        m_player.sprite()->layer = i.layer + 1;
                        m_player.sprite()->is_leftward = true;
                    }
                    if (entity_type != entity::Type::null) {
                        std::filesystem::path config_path = "res/entity/" + types_map.at(tile_number) + ".cfg";
                        //Console::log("entity config path: ", config_path, "\n");
                        if (entity_type == entity::Type::bug) {
                            m_entity_objects.push_back(std::make_unique<entity::Bug>());
                        } else if (entity_type == entity::Type::frog) {
                            m_entity_objects.push_back(std::make_unique<entity::Frog>());                            
                        } else if (entity::is_conduit(entity_type)) {
                            m_entity_objects.push_back(std::make_unique<entity::Conduit>());
                            m_entity_objects.back()->transform_id(m_level_transform_id);                    
                            m_unlocked_entity_objects.push_back(m_entity_objects.back());
                            //config_path = "res/entity/conduit.cfg";
                        } else if (entity::is_track(entity_type)) {
                            m_entity_objects.push_back(std::make_unique<entity::Track>());
                            m_entity_objects.back()->transform_id(m_level_transform_id);
                            m_unlocked_entity_objects.push_back(m_entity_objects.back());
                        } else if (entity_type == entity::Type::level_center) {
                            m_entity_objects.push_back(std::make_unique<entity::Flag>());
                            m_entity_objects.back()->transform_id(m_level_transform_id);
                        } else if (entity::is_logic_and(entity_type)) {
                            m_entity_objects.push_back(std::make_unique<entity::LogicAnd>());
                            m_entity_objects.back()->transform_id(m_level_transform_id);
                            m_unlocked_entity_objects.push_back(m_entity_objects.back());
                        } else if (entity::is_logic_not(entity_type)) {
                            m_entity_objects.push_back(std::make_unique<entity::LogicNot>());
                            m_entity_objects.back()->transform_id(m_level_transform_id);
                            m_unlocked_entity_objects.push_back(m_entity_objects.back());
                        } else if (entity::is_logic_or(entity_type)) {
                            m_entity_objects.push_back(std::make_unique<entity::LogicOr>());
                            m_entity_objects.back()->transform_id(m_level_transform_id);
                            m_unlocked_entity_objects.push_back(m_entity_objects.back());
                        } else if (entity_type == entity::Type::spring_U) {
                            m_entity_objects.push_back(std::make_unique<entity::Spring>());
                            m_entity_objects.back()->transform_id(m_level_transform_id);
                        } else if (entity_type == entity::Type::train) {
                            m_entity_objects.push_back(std::make_unique<entity::Train>());
                            //m_entity_objects.back()->transform()->position = tile_offset;
                        } else if (entity_type == entity::Type::trigger) {
                            //u8c entity_number = entity::number_in_type(entity_type);
                            //Console::log("trigger number: ", (int)entity_number, "\n");
                            //Console::log("trigger_", (int)entity_number, " = ", LevelConfig::trigger_path(entity_number), "\n");

                            m_entity_objects.push_back(std::make_unique<entity::Lever>());
                            m_entity_objects.back()->transform_id(m_level_transform_id);
                                                 
                            config_path = "res/entity/trigger.cfg";
                            //entity_type = entity::Type::lever;

                            m_trigger_entity_objects.push_back(m_entity_objects.back());
                        } else {
                            m_entity_objects.push_back(std::make_unique<entity::Object>());
                            m_entity_objects.back()->transform_id(m_level_transform_id);                            
                        }

                        m_entity_objects.back()->type(entity_type);
                        m_entity_objects.back()->start_layer(i.layer);

                        if (m_entity_objects.back()->transform_id() == m_level_transform_id) {
                            m_entity_objects.back()->start_offset(tile_offset);
                        }

                        m_entity_objects.back()->load_config(config_path);


                        if (m_entity_objects.back()->transform() && m_entity_objects.back()->transform()->id != m_level_transform_id) {
                            m_entity_objects.back()->transform()->position = tile_offset;
                            //Camera::add_transform_id(m_entity_objects.back()->transform()->id);
                        }

                    }
                }
            }

            Vec2i quad_node = { (i.x - i.x % 256 + 256) / 256, (i.y - i.y % 256 + 256) / 256 };
            if (std::find(quad_nodes.begin(), quad_nodes.end(), quad_node) == quad_nodes.end()) {
                quad_nodes.push_back(quad_node);
                m_level_quad_nodes.push_back(std::make_pair(quad_node, std::make_unique<QuadNode>(m_level_transform_id)));
            }
            if (quad_node.x > highest_quad_node.x)  highest_quad_node.x = quad_node.x;
            if (quad_node.y > highest_quad_node.y)  highest_quad_node.y = quad_node.y;
        }
        Camera::scroll_limit = highest_quad_node * 256 - Vec2i{ m_window_w, m_window_h };

        for (auto& entity : m_entity_objects) {
            check_entities_to_add_input_from(entity);
        }
    }

}