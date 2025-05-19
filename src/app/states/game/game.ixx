module;
#include <filesystem>
#include <sstream>
#include <vector>
#include <map>

export module state.game;

import state;
import bitmap_text;
import camera;
import console;
import input;
import line;
import sprite;
import tile;
import transform;
import types;

import player;
import quad_node;

//import sound;

export namespace state {
    class Game : public State {
    private:
        Camera m_camera;
        u16 m_window_w = 0, m_window_h = 0;

        i32 m_transform_id = -1,
            m_level_transform_id = -1;
        i32 m_input_id = -1;

        i32 m_line_0   = -1;

        u32 m_timer = 0;
        player::Object m_player;
        player::Object m_player2;
        player::Object m_player3;

        Vec2f m_level_start;

        std::vector<std::unique_ptr<tile::Object>> m_tile_objects;

        std::vector<std::pair<Vec2i, std::unique_ptr<QuadNode>>> m_level_quad_nodes;        

        BitmapText m_fps_text;

        std::filesystem::path m_level_path = "res/levels/test.bin";

        //Sound m_sound = Sound("res/sounds/whip.wav");

    public:
        Game(u16c window_w, u16c window_h);
        ~Game();
        void update() override;

        bool is_all_valid() {
            return transform::Set::is_valid(m_transform_id) &&
                   transform::Set::is_valid(m_level_transform_id) &&
                   input::Set::is_valid(m_input_id) &&
                   line::Set::is_valid(m_line_0);
        }

        std::map<u16, std::string> load_types_from_bin();

        Vec2f level_velocity() { return transform::Set::at(m_level_transform_id) ? transform::Set::at(m_level_transform_id)->velocity : Vec2f{}; }
        void level_velocity(Vec2fc v) {
            if (transform::Set::at(m_level_transform_id)) transform::Set::at(m_level_transform_id)->velocity = v;
        }
        f32c level_x() { return transform::Set::at(m_level_transform_id) ? transform::Set::at(m_level_transform_id)->position.x : 0.0f; }
        void level_x(f32c x) { if (transform::Set::at(m_level_transform_id)) transform::Set::at(m_level_transform_id)->position.x = x; }

        f32c level_y() { return transform::Set::at(m_level_transform_id) ? transform::Set::at(m_level_transform_id)->position.y : 0.0f; }
        void level_y(f32c y) { if (transform::Set::at(m_level_transform_id)) transform::Set::at(m_level_transform_id)->position.y = y; }

        Vec2f level_position() { return transform::Set::at(m_level_transform_id) ? transform::Set::at(m_level_transform_id)->position : Vec2f{}; }
        void level_position(Vec2fc p) {
            if (transform::Set::at(m_level_transform_id)) transform::Set::at(m_level_transform_id)->position = p;
        }

        void load_level_sprites(const std::filesystem::path& path);
    };
}