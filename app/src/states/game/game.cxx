module;
#include <sstream>

module state.game;

import console;
import inputs;
import lines;
import sprites;
import transforms;
import types;

namespace state {
    Game::Game() {
        set_current(state::Type::Game);

        m_transform_id = Transforms::make();

        m_level_transform_id = Transforms::make();
        Transforms::at(m_level_transform_id)->deceleration.x = 0.1f;

        m_quad_node.transform_id = m_level_transform_id;

        Console::log("quad_node transform_id: ", m_quad_node.transform_id, "\n");

        load_level_sprites_from(m_level_path);

        for (u8 i = 0; i < NUM_VISIBLE_LAYERS; ++i) {
            add_visible_layer(i);
        }
        m_player.set_layer(NUM_VISIBLE_LAYERS - 1);
        m_player.position({ 160.0f, 120.0f });
        m_player.acceleration({ 0.3f, 0.3f });
        m_player.deceleration({ 0.1f, 0.1f });
        m_player.velocity_limit({ 2.5f, 2.5f });

        m_player.sprite_texture("res/textures/tile_blue.png");

        m_player2.set_layer(NUM_VISIBLE_LAYERS - 1);
        m_player2.position({ 16.0f, 120.0f });
        m_player2.acceleration({ 0.3f, 0.3f });
        m_player2.deceleration({ 0.1f, 0.1f });
        m_player2.velocity_limit({ 2.5f, 2.5f });

        m_player2.sprite_texture("res/textures/tile_yellow.png");

        m_player2.key_up = input::Key::up;
        m_player2.key_down = input::Key::down;
        m_player2.key_left = input::Key::left;
        m_player2.key_right = input::Key::right;

        Transforms::at(m_transform_id)->velocity = { 2.0f, 1.0f };
        Transforms::at(m_transform_id)->deceleration = { 0.001f, 0.001f };
        Console::log("Game transform_id: ", m_transform_id, "\n");

        m_input_id = Inputs::make();

        m_line_0 = Lines::make({ 32, 32 }, { 64, 128 });
        Lines::at(m_line_0)->size = 8;
        Lines::at(m_line_0)->color = { 255, 127, 0 };

        //Console::log("Sprites size: ", Sprites::size(), "\n");

        m_fps_text.transform_id = m_transform_id;
        m_fps_text.layer = NUM_VISIBLE_LAYERS - 1;
    }
    Game::~Game() {
        Console::log("~Game()\n");
        Inputs::erase(m_input_id);
        Transforms::erase(m_transform_id);
        Transforms::erase(m_level_transform_id);
        Lines::erase(m_line_0);

        Inputs::clear();
        Sprites::clear();
        Transforms::clear();
    }    
    void Game::load_level_sprites_from(const std::filesystem::path& path) {
        Console::log("Game::load_level_sprites_from\n");

        auto sprite_data = Sprites::open(path);

        for (auto& i : sprite_data) {
            size_t sprite_id = Sprites::make(tile_set_texture_path(i.tile_set));
            Sprites::at(sprite_id)->id = sprite_id;
            Sprites::at(sprite_id)->tile_set = i.tile_set;
            Sprites::at(sprite_id)->layer = i.layer;
            Sprites::at(sprite_id)->offset = Vec2f{ (f32)i.offset_x, (f32)i.offset_y };
            Sprites::at(sprite_id)->source_rect = Rect<i32>{ i.source_x, i.source_y, 16, 16 };
            Sprites::at(sprite_id)->transform_id = m_level_transform_id;
        }        
    }
}