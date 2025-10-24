module;
#include <filesystem>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
export module player;
import anim;
import collider.aabb;
import console;
import input;
import sound;
import sprite;
import transform;
import entity;
import types;

//export U8 NUM_ADDED_PLAYERS = 0;
export namespace player {
    class Object : public entity::Object {
    private:        
        start::Type   m_next_level = start::Type::center;

        start::Info m_next_start = { start::Type::center, 0 };

        bool m_is_to_write_save = false;

        I32 m_input_id = -1;

        I32 m_down_thrust_sound_id  = -1,
            m_hit_ground_sound_id   = -1,
            m_hover_sound_id        = -1,
            m_jump_sound_id         = -1,
            m_ledge_climb_sound_id  = -1,
            m_ledge_grab_sound_id   = -1,
            m_lever_sound_id        = -1,            
            m_pick_up_sound_id      = -1,
            m_skid_sound_id         = -1,
            m_slide_wall_sound_id   = -1,
            m_step_grass_sound_id   = -1,
            m_swing_detach_sound_id = -1,
            m_toss_sound_id         = -1;

        I32 m_down_thrust_anim_id  = -1,
            m_duck_anim_id         = -1,
            m_crawl_anim_id        = -1,
            m_fall_anim_id         = -1,
            m_hit_ground_anim_id   = -1,
            m_hover_anim_id        = -1,
            m_jump_skid_anim_id    = -1,
            m_jump_spin_anim_id    = -1,
            m_ledge_climb_anim_id  = -1,
            m_ledge_grab_anim_id   = -1,
            m_lever_anim_id        = -1,
            m_rise_anim_id         = -1,
            m_run_anim_id          = -1,
            m_skid_anim_id         = -1,
            m_slide_ground_anim_id = -1,
            m_slide_wall_anim_id   = -1,
            m_swim_anim_id         = -1,
            m_swing_anim_id        = -1,
            m_walk_anim_id         = -1;

        U8 m_time_left_until_down_thrust = 0, m_time_until_down_thrust = 16,
           m_time_left_bouncing          = 0, m_time_to_bounce         = 20,
           m_time_left_ducking           = 0, m_time_to_duck           = 10,
           m_time_left_rising            = 0, m_time_to_rise           = 10,
           m_time_left_hitting_ground    = 0, m_time_to_hit_ground     = 20,
           m_time_left_holding_jump      = 0, m_time_to_hold_jump      = 10,           
           m_time_left_jumping_wall      = 0, m_time_to_jump_wall      =  8,
           m_time_left_jump_again        = 0, m_time_to_jump_again     =  8,
           m_time_left_lever             = 0, m_time_to_lever          = 10,
           m_time_left_melee             = 0, m_time_to_melee          =  8,           
           m_time_left_skidding          = 0, m_time_to_skid           = 20,
           m_time_sliding_wall           = 0;

        bool m_is_locked_melee      = false, m_is_locked_jump = false,
             m_is_wall_to_left      = false,
             m_is_to_wall_jump_left = false;

        bool m_is_down_thrusting = false,                
             m_is_sliding_ground = false,
             m_is_sliding_wall   = false,
             m_is_on_ledge       = false,
             m_is_climbing_ledge = false,
             m_is_hovering       = false;


        Vec2F m_ground_velocity_limit = { 2.0F, 4.0F };
        Vec2F m_swing_velocity_limit  = { 4.0F, 5.0F };

        U8 m_time_left_in_state = 0;

        Vec2F m_start_sprite_offset = { 0.0F, 0.0F };
                
    public:
        bool load_config(const std::filesystem::path& path) override;
        U8 id = 0;
                
        void collide_x(aabb::cInfo our, aabb::cInfo other) override;
        void collide_y(aabb::cInfo our, aabb::cInfo other) override;

        input::Key key_up     = input::Key::up,
                   key_down   = input::Key::down,
                   key_left   = input::Key::left,
                   key_right  = input::Key::right,
                   key_jump   = input::Key::x,
                   key_melee  = input::Key::c,
                   key_sprint = input::Key::z;

        Object();
        ~Object();

        bool is_all_valid() override {
            return anim::Manager::is_valid(m_crawl_anim_id)        &&
                   anim::Manager::is_valid(m_duck_anim_id)         &&
                   anim::Manager::is_valid(m_fall_anim_id)         &&
                   anim::Manager::is_valid(m_idle_anim_id)         &&
                   anim::Manager::is_valid(m_jump_anim_id)         &&                   
                   anim::Manager::is_valid(m_ledge_grab_anim_id)   &&
                   anim::Manager::is_valid(m_ledge_climb_anim_id)  &&
                   anim::Manager::is_valid(m_lever_anim_id)        &&
                   anim::Manager::is_valid(m_melee_anim_id)        &&
                   anim::Manager::is_valid(m_rise_anim_id)         &&
                   anim::Manager::is_valid(m_run_anim_id)          &&
                   anim::Manager::is_valid(m_skid_anim_id)         &&
                   anim::Manager::is_valid(m_slide_ground_anim_id) &&
                   anim::Manager::is_valid(m_slide_wall_anim_id)   &&
                   anim::Manager::is_valid(m_swim_anim_id)         &&
                   anim::Manager::is_valid(m_swing_anim_id)        &&
                   anim::Manager::is_valid(m_walk_anim_id)         &&
                   health::Manager::is_valid(m_health_id)          &&
                   input::Manager::is_valid(m_input_id)            &&                   
                   transform::Manager::is_valid(m_transform_id)    &&
                   sprite::Manager::is_valid(m_sprite_id);
        }

        start::cType next_level() const { return m_next_level; }
        start::cInfo next_start() const { return m_next_start; }

        bool is_to_write_save() const { return m_is_to_write_save; }    void is_to_write_save(bool is) { m_is_to_write_save = is; }

        void set_layer(cU8 layer) { if (sprite::Manager::get(m_sprite_id)) sprite::Manager::get(m_sprite_id)->layer = layer; }

        cI32 get_transform_id() const { return m_transform_id; }
        cI32 get_sprite_id()    const { return m_sprite_id;    }

        cRectI source_rect() const { return sprite::Manager::get(m_sprite_id) ? sprite::Manager::get(m_sprite_id)->source_rect : RectI{ 0, 0, 0, 0 }; }

        bool sprite_texture(const std::filesystem::path& path) { if (sprite::Manager::get(m_sprite_id)) { sprite::Manager::get(m_sprite_id)->texture(path); return true; } else return false; }

        void acceleration(cVec2F a) { if (transform::Manager::get(m_transform_id)) transform::Manager::get(m_transform_id)->acceleration = a; }
        void deceleration(cVec2F d) { if (transform::Manager::get(m_transform_id)) transform::Manager::get(m_transform_id)->deceleration = d; }

        void max_velocity(cVec2F l) { if (transform::Manager::get(m_transform_id)) transform::Manager::get(m_transform_id)->max_velocity = l; }

        bool is_pressed(input::Key k) { return input::Manager::is_valid(m_input_id) ? input::Manager::get(m_input_id)->is_pressed(k) : false; }
        void release(input::Key k)    { if (input::Manager::is_valid(m_input_id)) input::Manager::get(m_input_id)->release(k); }
        void press(input::Key k)      { if (input::Manager::is_valid(m_input_id)) input::Manager::get(m_input_id)->press(k);   }

        void update() override;
        void update_dead();
        void update_run();
        void update_swim();
        void update_swing();

        void hurt(entity::Object* culprit) override;
        void interact(entity::Object* object) override;

        void play_sound(cI32 sound_id) {
            if (!sound::Manager::get(sound_id)) return;
            if (sound::Manager::get(sound_id)->is_playing()) {
                sound::Manager::get(sound_id)->stop();
            }
            sound::Manager::get(sound_id)->position({ position().x / 160.0F, position().y / 90.0F });
            sound::Manager::get(sound_id)->play();
        }

        void spawn_down_thrust(cVec2F position);
    };
}