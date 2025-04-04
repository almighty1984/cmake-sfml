module;
#include <filesystem>

#include <sstream>
export module state.game;

import state;
import bitmap_text;
import console;
import inputs;
import lines;
import sprites;
import transforms;
import types;

import player;
import quad_node;

export namespace state {
    class Game : public State {
    private:        
        i32 m_transform_id = -1, m_level_transform_id = -1;        
        i32 m_input_id = -1;
        i32 m_line_0   = -1;

        u32 m_timer = 0;
        Player m_player;
        Player m_player2;

        QuadNode m_quad_node;

        BitmapText m_fps_text;

        std::filesystem::path m_level_path = "res/levels/test.bin";

    public:
        Game();
        ~Game();
        void update() override;

        bool is_all_valid() {
            return Transforms::is_valid(m_transform_id) &&
                   Transforms::is_valid(m_level_transform_id) &&
                   Inputs::is_valid(m_input_id) &&
                   Lines::is_valid(m_line_0);
        }

        Vec2f level_velocity() { return Transforms::at(m_level_transform_id) ? Transforms::at(m_level_transform_id)->velocity : Vec2f{}; }
        void level_velocity(Vec2fc v) {
            if (Transforms::at(m_level_transform_id)) Transforms::at(m_level_transform_id)->velocity = v;
        }
        f32c level_x() { return Transforms::at(m_level_transform_id) ? Transforms::at(m_level_transform_id)->position.x : 0.0f; }
        void level_x(f32c x) { if (Transforms::at(m_level_transform_id)) Transforms::at(m_level_transform_id)->position.x = x; }

        f32c level_y() { return Transforms::at(m_level_transform_id) ? Transforms::at(m_level_transform_id)->position.y : 0.0f; }
        void level_y(f32c y) { if (Transforms::at(m_level_transform_id)) Transforms::at(m_level_transform_id)->position.y = y; }

        Vec2f level_position() { return Transforms::at(m_level_transform_id) ? Transforms::at(m_level_transform_id)->position : Vec2f{}; }
        void level_position(Vec2fc p) {
            if (Transforms::at(m_level_transform_id)) Transforms::at(m_level_transform_id)->position = p;
        }

        void load_level_sprites_from(const std::filesystem::path& path);
    };
}