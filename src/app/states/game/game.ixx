module;
#include <filesystem>
#include <sstream>
#include <vector>
#include <map>

export module state.game;

import state;
import bitmap_text;
import camera;
import collider;
import console;
import input;
import line;
import sprite;
import tile;
import transform;
import types;

import player;
import quad_node;

import music;
import plane;

export namespace state {
    class Game : public State {
    private:
        Camera m_camera;
        u16 m_window_w = 0,
            m_window_h = 0;

        i32 m_transform_id       = -1,
            m_level_transform_id = -1,
            m_input_id           = -1;


        u32 m_timer = 0;
        player::Object m_player;
        //player::Object m_player2;
        //player::Object m_player3;        

        Vec2f m_start_position;
        //sprite::Type m_level_start = sprite::Type::level_start;        

        std::vector<std::unique_ptr<tile::Object>> m_tile_objects;

        std::vector<std::pair<Vec2i, std::unique_ptr<QuadNode>>> m_level_quad_nodes;        

        BitmapText m_fps_text;

        std::filesystem::path m_level_path = "res/levels/test.bin";

        

        //Sound m_sound = Sound("res/sounds/whip.wav");

        plane::Object m_background_plane;

    public:
        Game(u16c window_w, u16c window_h, const std::filesystem::path level_path, start::Info start_info);
        ~Game() {
            clear();
        }
        void clear() {
            Console::log("state::Game::clear()\n");
            input::Set::erase(m_input_id);
            transform::Set::erase(m_transform_id);
            transform::Set::erase(m_level_transform_id);

            input::Set::clear();
            sprite::Set::clear();
            transform::Set::clear();

            aabb::Set::clear();

        }
        void update() override;

        void restart() {
            start_info(m_player.next_start());
            //m_start_position = m_player.next_start_position() - transform::Set::at(m_level_transform_id)->position;

            current(state::Type::null);
            next(state::Type::game);
        }

        bool is_all_valid() const {
            return transform::Set::is_valid(m_transform_id)       &&
                   transform::Set::is_valid(m_level_transform_id) &&                   
                   input::Set::is_valid(m_input_id);
        }
                
        std::filesystem::path level_path_to_save() override {
            return m_level_path;
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


        /*const std::filesystem::path next_level_path() override {
            switch (m_next_level) {
            case start::Type::L_0: return m_level_path_L_0; break;
            case start::Type::R_0: return m_level_path_R_0; break;
            default:               return m_level_path;     break; }
        }*/

        void load_level_config(const std::filesystem::path& path);

        void load_level_sprites(const std::filesystem::path& path);
    };
}