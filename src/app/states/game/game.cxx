module;
#include <filesystem>
#include <fstream>
#include <vector>
#include <sstream>
module state.game;
import collider;
import console;
import input;
import line;
import quad_node;
import sprite;
import transform;
import types;

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



    void Game::load_level_sprites(const std::filesystem::path& path) {
        Console::log("Game::load_level_sprites\n");

        auto types_map = load_types_from_bin();

        auto sprite_data = sprite::Set::open(path);
        Vec2i highest_quad_node = { 0, 0 };

        std::vector<Vec2i> quad_nodes;

        for (auto& i : sprite_data) {
            size_t sprite_id = sprite::Set::make(tile_set_texture_path(i.tile_set));
            sprite::Set::at(sprite_id)->id = sprite_id;
            sprite::Set::at(sprite_id)->tile_set = i.tile_set;
            sprite::Set::at(sprite_id)->layer = i.layer;
            sprite::Set::at(sprite_id)->offset = { (f32)i.x, (f32)i.y };
            sprite::Set::at(sprite_id)->source_rect = Rect<i32>{ i.source_x, i.source_y, 16, 16 };
            sprite::Set::at(sprite_id)->transform_id = m_level_transform_id;

            if (i.tile_set == 255) {
                u16c tile_x = i.source_x / 16;
                u16c tile_y = i.source_y / 16;
                u16c number = tile_x + tile_y * 32;
                if (types_map.count(number)) {
                    sprite::Set::at(sprite_id)->type = sprite::from_string(types_map.at(number));
                    if (start_info().type == start::Type::center && sprite::Set::at(sprite_id)->type == sprite::Type::level_center) {
                        //Console::log("state::Game::load_level_sprites start: ", sprite::to_string(sprite::Set::at(sprite_id)->type), " position: ", m_start_position.x, " ", m_start_position.y, "\n");
                        m_start_position = sprite::Set::at(sprite_id)->offset;
                    } else if (start_info().type == start::Type::L_0 && sprite::Set::at(sprite_id)->type == sprite::Type::level_L_0) {
                        m_start_position = sprite::Set::at(sprite_id)->offset;
                        m_player.sprite()->is_leftward = false;
                    } else if (start_info().type == start::Type::R_0 && sprite::Set::at(sprite_id)->type == sprite::Type::level_R_0) {
                        m_start_position = sprite::Set::at(sprite_id)->offset;
                        m_player.sprite()->is_leftward = true;
                    }
                    if (sprite::Set::at(sprite_id)->type != sprite::Type::null) {
                        m_tile_objects.push_back(std::make_unique<tile::Object>());
                        m_tile_objects.back()->sprite_id(sprite_id);
                        m_tile_objects.back()->transform_id(m_level_transform_id);
                        m_tile_objects.back()->load_config("res/tiles/" + types_map.at(number) + ".cfg");
                    }                    
                }
            } else {
                
            }

            Vec2i quad_node = { (i.x - i.x % 256 + 256) / 256, (i.y - i.y % 256 + 256) / 256 };
            if (std::find(quad_nodes.begin(), quad_nodes.end(), quad_node) == quad_nodes.end()) {
                quad_nodes.push_back(quad_node);
                m_level_quad_nodes.push_back(std::make_pair(quad_node, std::make_unique<QuadNode>(m_level_transform_id)));
            }
            if (quad_node.x > highest_quad_node.x)  highest_quad_node.x = quad_node.x;
            if (quad_node.y > highest_quad_node.y)  highest_quad_node.y = quad_node.y;
        }
        m_camera.scroll_limit = highest_quad_node * 256 - Vec2i{ m_window_w, m_window_h };
    }
    Game::Game(u16c window_w, u16c window_h, std::filesystem::path level_path, start::Info start) {
        //Console::log("Game::Game level: ", level_path, " start: ", start_position.x, " ", start_position.y, "\n");
        current(state::Type::game);
        m_window_w = window_w, m_window_h = window_h;
        m_level_path = level_path;


        start_info(start);

        m_transform_id = transform::Set::make();
        m_fps_text.transform_id = m_transform_id;
        m_fps_text.layer = NUM_VISIBLE_LAYERS - 1;

        Console::log("state::Game fps_text transform id: ", m_transform_id, "\n");

        m_background_plane.create(3, 1);



        m_level_transform_id = transform::Set::make();

        m_player.level_transform_id = m_level_transform_id;


        load_level_sprites(m_level_path);

        /*for (u16 i = 0; i < m_num_quad_nodes.x * m_num_quad_nodes.y; ++i) {
            m_quad_nodes.push_back(std::make_unique<QuadNode>());
            m_quad_nodes.back().get()->transform_id = m_level_transform_id;
        }*/

        for (u8 i = 0; i < NUM_VISIBLE_LAYERS; ++i) {
            add_visible_layer(i);
        }
        m_player.set_layer(NUM_VISIBLE_LAYERS - 1);

        //m_player.sprite_texture("res/textures/tile_blue.png");

        m_player.position(m_start_position);

        //m_player.id = 0;

        //m_player2.set_layer(NUM_VISIBLE_LAYERS - 1);
        //m_player2.position(m_start_position - Vec2f{ 16.0f, 0.0f });


        //m_player2.id = 1;

        /*m_player2.key_up = input::Key::w;
        m_player2.key_down = input::Key::s;
        m_player2.key_left = input::Key::a;
        m_player2.key_right = input::Key::d;
        m_player2.key_jump = input::Key::g;
        m_player2.key_sprint = input::Key::h;
        m_player2.key_melee = input::Key::f;*/

        /*m_player3.set_layer(NUM_VISIBLE_LAYERS - 1);
        m_player3.position(m_start_position + Vec2f{ 16.0f, 0.0f });

        m_player3.key_up = input::Key::i;
        m_player3.key_down = input::Key::k;
        m_player3.key_left = input::Key::j;
        m_player3.key_right = input::Key::l;
        m_player3.key_jump = input::Key::r;
        m_player3.key_sprint = input::Key::e;
        m_player3.key_melee = input::Key::t;*/

        //m_player3.id = 2;

        //transform::Set::at(m_transform_id)->velocity = { 2.0f, 1.0f };
        //transform::Set::at(m_transform_id)->deceleration = { 0.001f, 0.001f };
        //Console::log("Game transform_id: ", m_transform_id, "\n");

        m_input_id = input::Set::make();

        //Console::log("sprite::Objects size: ", sprite::Set::size(), "\n");


       /* for (auto& i : m_background_plane.get_transform_ids()) {
            m_camera.add_transform_id(i);
        }*/

        m_camera.add_transform_id(m_level_transform_id);

        m_camera.focus_transform = m_player.get_transform_id();
        m_camera.add_transform_id(m_player.get_transform_id());
        //m_camera.add_transform_id(m_player2.get_transform_id());
        //m_camera.add_transform_id(m_player3.get_transform_id());

        //Vec2f camera_position = m_player.position() - Vec2f{ window_w / 2.0f, window_h / 2.0f };
        Vec2f camera_position = m_start_position - Vec2f{ window_w / 2.0f, window_h / 2.0f + 8.0f };


        Console::log("state::Game::Game camera position: ", camera_position.x, " ", camera_position.y, "\n");

        m_camera.set_position(camera_position);

        //m_camera.add_transform_id(m_transform_id);

    }
}