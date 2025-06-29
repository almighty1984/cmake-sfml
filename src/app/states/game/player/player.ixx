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
        run, swim, swing
    };
    const char* state_as_string(State s) {
        switch (s) {
        case State::run:   return "run";   break;
        case State::swim:  return "swim";  break;
        case State::swing: return "swing"; break;
        default:           return "";      break; }
    }
    class Object : public tile::Object {
    private:
        player::State m_state      = player::State::run,
                      m_next_state = player::State::run,
                      m_prev_state = player::State::null;
        start::Type   m_next_level = start::Type::center;

        start::Info m_next_start;
        bool m_is_to_write_save;

        std::vector<i32> m_run_collider_ids;
        std::vector<i32> m_duck_collider_ids;
        std::vector<i32> m_swim_collider_ids;

        i32 m_melee_left_collider_id, m_melee_right_collider_id;        

        i32 m_bump_head_sound_id    = -1,
            m_down_thrust_sound_id  = -1,
            m_hit_ground_sound_id   = -1,
            m_hover_sound_id        = -1,
            m_jump_sound_id         = -1,
            m_ledge_climb_sound_id  = -1,
            m_ledge_grab_sound_id   = -1,
            m_melee_sound_id        = -1,
            m_skid_sound_id         = -1,
            m_slide_wall_sound_id   = -1,
            m_step_grass_sound_id   = -1,
            m_swing_attach_sound_id = -1,
            m_swing_detach_sound_id = -1;

        i32 m_input_id = -1;

        i32 m_down_thrust_anim_id  = -1,
            m_duck_anim_id         = -1,
            m_crawl_anim_id        = -1,
            m_fall_anim_id         = -1,
            m_hit_ground_anim_id   = -1,
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
            m_swing_anim_id        = -1,
            m_walk_anim_id         = -1;

        u8 m_num_jumps = 0,
           m_time_left_until_down_thrust = 0, m_time_until_down_thrust = 16,
           m_time_left_ducking           = 0, m_time_to_duck           = 10,
           m_time_left_rising            = 0, m_time_to_rise           = 10,
           m_time_left_hitting_ground    = 0, m_time_to_hit_ground     = 20,
           m_time_left_holding_jump      = 0, m_time_to_hold_jump      = 10,
           m_time_left_jumping_wall      = 0, m_time_to_jump_wall      =  8,
           m_time_left_jump_again        = 0, m_time_to_jump_again     =  8,
           m_time_left_melee             = 0, m_time_to_melee          =  8,
           m_time_left_skidding          = 0, m_time_to_skid           = 20;

        bool m_is_locked_melee      = false, m_is_locked_jump = false,
             m_is_wall_to_left      = false,
             m_is_to_wall_jump_left = false;

        bool m_is_down_thrusting = false,
             m_is_ducking        = false,             
             m_is_sliding_ground = false,
             m_is_sliding_wall   = false,
             m_is_on_ledge       = false,
             m_is_climbing_ledge = false,
             m_is_hovering       = false;

        Vec2f m_swing_level_offset;

        f32 m_rotation_speed = 1.0f;

        Vec2f m_ground_velocity_limit = { 2.0f, 4.0f };
        Vec2f m_swing_velocity_limit  = { 4.0f, 5.0f };

        //f32 m_velocity_at_jump;

        u8 m_time_left_in_state = 0;
                
    public:
        void load_config(const std::filesystem::path& path) override;
        u8 id = 0;
        i32 level_transform_id = -1;

        Vec2fc position_difference(i32c transform_id, i32c transform_id2) {
            return transform::Set::at(transform_id) && transform::Set::at(transform_id2) ?
                transform::Set::at(transform_id)->position - transform::Set::at(transform_id2)->position : Vec2f{0.0f, 0.0f};
        }
        
        void collide_x(sprite::Infoc our, sprite::Infoc other) override;
        void collide_y(sprite::Infoc our, sprite::Infoc other) override;

        void collide_melee_x(sprite::Infoc our, sprite::Infoc other);
        void collide_melee_y(sprite::Infoc our, sprite::Infoc other);

        
        input::Key key_up     = input::Key::up,
                   key_down   = input::Key::down,
                   key_left   = input::Key::left,
                   key_right  = input::Key::right,
                   key_jump   = input::Key::x,
                   key_melee  = input::Key::c,
                   key_sprint = input::Key::z;

        Object();
        ~Object();

        anim::Object* anim() { return anim::Set::at(m_current_anim_id); }

        void next_state(State state) {
            m_prev_state = m_state;
            m_state = state;
        }

        bool is_all_valid() {
            bool is_colliders_valid = true;            
            for (auto& i : m_run_collider_ids) {
                is_colliders_valid = anim::Set::is_valid(i);
            }
            for (auto& i : m_duck_collider_ids) {
                is_colliders_valid = anim::Set::is_valid(i);
            }
            for (auto& i : m_swim_collider_ids) {
                is_colliders_valid = anim::Set::is_valid(i);
            }

            return is_colliders_valid &&
                   anim::Set::is_valid(m_crawl_anim_id)          &&
                   anim::Set::is_valid(m_duck_anim_id)           &&
                   anim::Set::is_valid(m_fall_anim_id)           &&
                   anim::Set::is_valid(m_idle_anim_id)           &&
                   anim::Set::is_valid(m_jump_anim_id)           &&                   
                   anim::Set::is_valid(m_ledge_grab_anim_id)     &&
                   anim::Set::is_valid(m_ledge_climb_anim_id)    &&
                   anim::Set::is_valid(m_rise_anim_id)           &&
                   anim::Set::is_valid(m_run_anim_id)            &&
                   anim::Set::is_valid(m_skid_anim_id)           &&
                   anim::Set::is_valid(m_slide_ground_anim_id)   &&
                   anim::Set::is_valid(m_slide_wall_anim_id)     &&
                   anim::Set::is_valid(m_swim_anim_id)           &&
                   anim::Set::is_valid(m_swing_anim_id)          &&
                   anim::Set::is_valid(m_walk_anim_id)           &&
                   input::Set::is_valid(m_input_id)              &&                   
                   transform::Set::is_valid(m_transform_id)      &&
                   sprite::Set::is_valid(m_sprite_id);
        }

        start::Type next_level() const { return m_next_level; }
        start::Info next_start() const { return m_next_start; }

        bool is_to_write_save() const { return m_is_to_write_save; }    void is_to_write_save(bool is) { m_is_to_write_save = is; }

        void set_layer(u8c layer) { sprite::Set::at(m_sprite_id)->layer = layer; }

        i32c get_transform_id() const { return m_transform_id; }
        i32c get_sprite_id()    const { return m_sprite_id;    }

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
        void release(input::Key k)    { if (input::Set::is_valid(m_input_id)) input::Set::at(m_input_id)->release(k); }
        void press(input::Key k)      { if (input::Set::is_valid(m_input_id)) input::Set::at(m_input_id)->press(k);   }

        void update() override;
        void update_run();
        void update_swim();
        void update_swing();
    };
}