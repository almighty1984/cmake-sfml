module;
#include <filesystem>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
export module player;
import anim;
import collider;
import console;
import input;
import sound;
import sprite;
import transform;
import tile;
import types;

//export u8 NUM_ADDED_PLAYERS = 0;
export namespace player {
    enum class State {
        null = 0,
        run, swim
    };
    class Object : public tile::Object {
    private:
        State m_state = State::run;
        i32 m_bump_head_sound_id   = -1,
            m_hover_sound_id       = -1,
            m_jump_sound_id        = -1,
            m_ledge_climb_sound_id = -1,
            m_ledge_grab_sound_id  = -1,
            m_melee_sound_id       = -1,
            m_skid_sound_id        = -1,
            m_slide_wall_sound_id  = -1,
            m_step_grass_sound_id  = -1;

        //std::vector<std::unique_ptr<Sound>> m_sounds;

        i32 m_input_id = -1;

        i32 m_duck_anim_id         = -1,
            m_crawl_anim_id        = -1,
            m_fall_anim_id         = -1,
            m_hover_anim_id        = -1,
            m_idle_anim_id         = -1,
            m_jump_anim_id         = -1,
            m_jump_skid_anim_id    = -1,
            m_jump_spin_anim_id    = -1,
            m_ledge_climb_anim_id  = -1,
            m_ledge_grab_anim_id   = -1,
            m_melee_anim_id        = -1,
            m_rise_anim_id         = -1,
            m_run_anim_id          = -1,
            m_skid_anim_id         = -1,
            m_slide_ground_anim_id = -1,
            m_slide_wall_anim_id   = -1,
            m_swim_anim_id         = -1,
            m_walk_anim_id         = -1;

        Vec2f m_start_velocity_limit = {};

        u8 m_num_jumps = 0,
           m_time_left_rising       = 0, m_time_to_rise       = 10,
           m_time_left_holding_jump = 0, m_time_to_hold_jump  = 10,
           m_time_left_jumping_wall = 0, m_time_to_jump_wall  =  8,
           m_time_left_jump_again   = 0, m_time_to_jump_again =  8,
           m_time_left_melee        = 0, m_time_to_melee      =  8,
           m_time_left_skidding     = 0, m_time_to_skid       = 20;

        bool m_is_locked_melee      = false, m_is_locked_jump = false,
             m_is_wall_to_left      = false,
             m_is_to_wall_jump_left = false;

        bool m_is_ducking        = false,
             m_is_sliding_ground = false,
             m_is_sliding_wall   = false,
             m_is_on_ledge       = false,
             m_is_climbing_ledge = false,
             m_is_hovering       = false;

    public:
        void load_config(const std::filesystem::path& path) override;
        u8 id = 0;
        void collide_x(sprite::Infoc our, sprite::Infoc other);
        void collide_y(sprite::Infoc our, sprite::Infoc other);

        std::vector<i32>& aabb_ids() { return m_aabb_ids; }

        input::Key key_up     = input::Key::up,
                   key_down   = input::Key::down,
                   key_left   = input::Key::left,
                   key_right  = input::Key::right,
                   key_jump   = input::Key::x,
                   key_melee  = input::Key::c,
                   key_sprint = input::Key::z;

        Object();
        ~Object();

        bool is_all_valid() {

            return anim::Set::is_valid(m_crawl_anim_id)         &&
                   anim::Set::is_valid(m_duck_anim_id)          &&
                   anim::Set::is_valid(m_fall_anim_id)          &&
                   anim::Set::is_valid(m_idle_anim_id)          &&
                   anim::Set::is_valid(m_jump_anim_id)          &&                   
                   anim::Set::is_valid(m_ledge_grab_anim_id)    &&
                   anim::Set::is_valid(m_ledge_climb_anim_id)   &&
                   anim::Set::is_valid(m_rise_anim_id)          &&
                   anim::Set::is_valid(m_run_anim_id)           &&
                   anim::Set::is_valid(m_skid_anim_id)          &&
                   anim::Set::is_valid(m_slide_ground_anim_id)  &&
                   anim::Set::is_valid(m_slide_wall_anim_id)    &&
                   anim::Set::is_valid(m_swim_anim_id)          &&
                   anim::Set::is_valid(m_walk_anim_id)          &&
                   input::Set::is_valid(m_input_id)             &&
                   sound::Set::is_valid(m_bump_head_sound_id)   &&
                   sound::Set::is_valid(m_ledge_climb_sound_id) &&
                   sound::Set::is_valid(m_ledge_grab_sound_id)  &&
                   sound::Set::is_valid(m_jump_sound_id)        &&
                   sound::Set::is_valid(m_hover_sound_id)       &&
                   sound::Set::is_valid(m_melee_sound_id)       &&
                   sound::Set::is_valid(m_skid_sound_id)        &&
                   sound::Set::is_valid(m_slide_wall_sound_id)  &&
                   sound::Set::is_valid(m_step_grass_sound_id)  &&
                   transform::Set::is_valid(m_transform_id)     &&
                   sprite::Set::is_valid(m_sprite_id);
        }

        void set_layer(u8c layer) { sprite::Set::at(m_sprite_id)->layer = layer; }

        size_t get_transform_id() const { return m_transform_id; }
        size_t get_sprite_id() const { return m_sprite_id; }

        Rectic source_rect() const { return sprite::Set::at(m_sprite_id) ? sprite::Set::at(m_sprite_id)->source_rect : Recti{ 0, 0, 0, 0 }; }

        bool sprite_texture(const std::filesystem::path& path) { if (sprite::Set::at(m_sprite_id)) { sprite::Set::at(m_sprite_id)->texture(path); return true; } else return false; }

        Vec2fc position() const { return transform::Set::at(m_transform_id) ? transform::Set::at(m_transform_id)->position : Vec2f{ 0.0f,0.0f }; }  void position(Vec2fc p) { if (transform::Set::at(m_transform_id)) transform::Set::at(m_transform_id)->position = p; }
        f32c   x()        const { return transform::Set::at(m_transform_id) ? transform::Set::at(m_transform_id)->position.x : 0.0f; }              void x(f32c in_x) { if (transform::Set::at(m_transform_id)) transform::Set::at(m_transform_id)->position.x = in_x; }
        f32c   y()        const { return transform::Set::at(m_transform_id) ? transform::Set::at(m_transform_id)->position.y : 0.0f; }              void y(f32c in_y) { if (transform::Set::at(m_transform_id)) transform::Set::at(m_transform_id)->position.y = in_y; }
        Vec2fc velocity() const { return transform::Set::at(m_transform_id) ? transform::Set::at(m_transform_id)->velocity : Vec2f{ 0.0f, 0.0f }; } void velocity(Vec2fc v) { if (transform::Set::at(m_transform_id)) transform::Set::at(m_transform_id)->velocity = v; }

        void acceleration(Vec2fc a) { if (transform::Set::at(m_transform_id)) transform::Set::at(m_transform_id)->acceleration = a; }
        void deceleration(Vec2fc d) { if (transform::Set::at(m_transform_id)) transform::Set::at(m_transform_id)->deceleration = d; }

        void velocity_limit(Vec2fc l) { if (transform::Set::at(m_transform_id)) transform::Set::at(m_transform_id)->velocity_limit = l; }

        bool is_pressed(input::Key k) { return input::Set::is_valid(m_input_id) ? input::Set::at(m_input_id)->is_pressed(k) : false; }
        void release(input::Key k) { if (input::Set::is_valid(m_input_id))     input::Set::at(m_input_id)->release(k); }

        void update();
        void update_run();
        void update_swim();
    };
}