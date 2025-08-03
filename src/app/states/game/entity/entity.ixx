module;
#include <filesystem>
#include <string>
#include <vector>
#include <set>
#include <list>

export module entity;
import anim;
import camera;
//import collider.aabb;
import console;
import sound;
import sprite;
import transform;
import types;

namespace entity {
    class Object;
}

export namespace aabb {    
    struct Info {
        entity::Object* owner = nullptr;
        i32     id = -1;
        //Rectf   rect = { 0.0f, 0.0f, 0.0f, 0.0f };
    };
    using Infoc = const Info;
}

export namespace entity {
    enum class State {
        null = 0,
        bounce, hurt, idle, jump, melee, upended, run, swim, swing, tossed, walk
    };
    using Statec = const State;
    const char* state_to_string(Statec s) {
        switch (s) {
        case State::bounce:     return "bounce";
        case State::hurt:       return "hurt";
        case State::idle:       return "idle";
        case State::jump:       return "jump";
        case State::melee:      return "melee";
        case State::run:        return "run";
        case State::swim:       return "swim";
        case State::swing:      return "swing";
        case State::tossed:     return "tossed";
        case State::upended:    return "upended";
        case State::walk:       return "walk";
        default:                return "";       }
    }

    enum class Type {
        null = 0,
        arch_L_1x1, arch_R_1x1, arch_L_2x1_0, arch_L_2x1_1, arch_R_2x1_0, arch_R_2x1_1,
        bug,
        clip, clip_duck, clip_D, clip_D_L, clip_D_R, clip_L, clip_R, clip_ledge, clip_ledge_L, clip_ledge_R, clip_U, 
        conduit_U, conduit_D, conduit_L, conduit_R,
        conduit_LU, conduit_RU,
        conduit_LD, conduit_RD,
        conduit_UL, conduit_UR,
        conduit_DL, conduit_DR,
        frog,
        level_center, level_L_0, level_R_0,
        logic_and_UD, logic_and_LR,
        logic_not_U, logic_not_D, logic_not_L, logic_not_R,
        logic_or_UD, logic_or_LR,        
        slope_L_1x1, slope_R_1x1, slope_L_2x1_0, slope_L_2x1_1, slope_R_2x1_0, slope_R_2x1_1, slope_U,
        spring_U,
        swing,        
        track_U, track_D, track_L, track_R,
        track_UL, track_LD, track_DR, track_RU,
        track_UR, track_RD, track_DL, track_LU,

        track_LU_1x1, track_UL_1x1, track_LD_1x1, track_DL_1x1,
        track_RU_1x1, track_UR_1x1, track_RD_1x1, track_DR_1x1,

        track_LU_2x1_0, track_LU_2x1_1, track_LD_2x1_0, track_LD_2x1_1,
        track_RU_2x1_0, track_RU_2x1_1, track_RD_2x1_0, track_RD_2x1_1,

        track_UL_1x2_0, track_UL_1x2_1, track_UR_1x2_0, track_UR_1x2_1,        
        track_DL_1x2_0, track_DL_1x2_1, track_DR_1x2_0, track_DR_1x2_1,

        train,
        trigger,
        particle_sense,
        player
    };
    using Typec = const Type;   
    /*bool is_type_trigger(Typec type) {
        return type == Type::trigger;
    }*/

    bool is_arch(Typec t) { return t == Type::arch_L_1x1 || t == Type::arch_L_2x1_0 || t == Type::arch_L_2x1_1 ||
                                   t == Type::arch_R_1x1 || t == Type::arch_R_2x1_0 || t == Type::arch_R_2x1_1; }
    bool is_clip(Typec t) { return t == Type::clip       || t == Type::clip_duck    || t == Type::clip_U   || t == Type::clip_D   ||
                                   t == Type::clip_L     || t == Type::clip_R       || t == Type::clip_D_L || t == Type::clip_D_R ||
                                   t == Type::clip_ledge || t == Type::clip_ledge_L || t == Type::clip_ledge_R; }
    bool is_conduit(Typec t) { return t == Type::conduit_U  || t == Type::conduit_D  || t == Type::conduit_L  || t == Type::conduit_R  ||
                                      t == Type::conduit_LU || t == Type::conduit_LD || t == Type::conduit_RU || t == Type::conduit_RD ||
                                      t == Type::conduit_UL || t == Type::conduit_UR || t == Type::conduit_DL || t == Type::conduit_DR; }
    bool is_logic_and(Typec t) { return t == Type::logic_and_UD || t == Type::logic_and_LR; }
    bool is_logic_not(Typec t) { return t == Type::logic_not_U  || t == Type::logic_not_D || t == Type::logic_not_L || t == Type::logic_not_R; }
    bool is_logic_or(Typec t)  { return t == Type::logic_or_UD  || t == Type::logic_or_LR; }
    bool is_logic(Typec t) { return is_logic_and(t) || is_logic_not(t) || is_logic_or(t); }
    bool is_track(Typec t) { return t == Type::track_U        || t == Type::track_L        || t == Type::track_D        || t == Type::track_R        ||
                                    t == Type::track_UR       || t == Type::track_RD       || t == Type::track_DL       || t == Type::track_LU       ||
                                    t == Type::track_LD       || t == Type::track_DR       || t == Type::track_RU       || t == Type::track_UL       ||
                                    t == Type::track_LU_1x1   || t == Type::track_UL_1x1   || t == Type::track_LD_1x1   || t == Type::track_DL_1x1   ||
                                    t == Type::track_RU_1x1   || t == Type::track_UR_1x1   || t == Type::track_RD_1x1   || t == Type::track_DR_1x1   ||
                                    t == Type::track_LU_2x1_0 || t == Type::track_LU_2x1_1 || t == Type::track_LD_2x1_0 || t == Type::track_LD_2x1_1 ||
                                    t == Type::track_RU_2x1_0 || t == Type::track_RU_2x1_1 || t == Type::track_RD_2x1_0 || t == Type::track_RD_2x1_1 ||
                                    t == Type::track_UL_1x2_0 || t == Type::track_UL_1x2_1 || t == Type::track_UR_1x2_0 || t == Type::track_UR_1x2_1 ||               
                                    t == Type::track_DL_1x2_0 || t == Type::track_DL_1x2_1 || t == Type::track_DR_1x2_0 || t == Type::track_DR_1x2_1;
    }

    Type type_from_string(const std::string& s) {
        //Console::log("sprite::type_from_string: ", s, "\n");
        if      (s == "arch_L_1x1")     return Type::arch_L_1x1;
        else if (s == "arch_R_1x1")     return Type::arch_R_1x1;
        else if (s == "arch_L_2x1_0")   return Type::arch_L_2x1_0;
        else if (s == "arch_L_2x1_1")   return Type::arch_L_2x1_1;
        else if (s == "arch_R_2x1_0")   return Type::arch_R_2x1_0;
        else if (s == "arch_R_2x1_1")   return Type::arch_R_2x1_1;
        else if (s == "bug")            return Type::bug;
        else if (s == "clip")           return Type::clip;
        else if (s == "clip_duck")      return Type::clip_duck;
        else if (s == "clip_L")         return Type::clip_L;
        else if (s == "clip_R")         return Type::clip_R;
        else if (s == "clip_D")         return Type::clip_D;
        else if (s == "clip_D_L")       return Type::clip_D_L;
        else if (s == "clip_D_R")       return Type::clip_D_R;
        else if (s == "clip_ledge")     return Type::clip_ledge;
        else if (s == "clip_ledge_L")   return Type::clip_ledge_L;
        else if (s == "clip_ledge_R")   return Type::clip_ledge_R;
        else if (s == "clip_U")         return Type::clip_U;
        else if (s == "conduit_U")      return Type::conduit_U;
        else if (s == "conduit_D")      return Type::conduit_D;
        else if (s == "conduit_L")      return Type::conduit_L;
        else if (s == "conduit_R")      return Type::conduit_R;
        else if (s == "conduit_LU")     return Type::conduit_LU;
        else if (s == "conduit_RU")     return Type::conduit_RU;
        else if (s == "conduit_LD")     return Type::conduit_LD;
        else if (s == "conduit_RD")     return Type::conduit_RD;
        else if (s == "conduit_UL")     return Type::conduit_UL;
        else if (s == "conduit_UR")     return Type::conduit_UR;
        else if (s == "conduit_DL")     return Type::conduit_DL;
        else if (s == "conduit_DR")     return Type::conduit_DR;
        else if (s == "frog")           return Type::frog;
        else if (s == "level_center")   return Type::level_center;
        else if (s == "level_L_0")      return Type::level_L_0;
        else if (s == "level_R_0")      return Type::level_R_0;
        else if (s == "logic_and_UD")   return Type::logic_and_UD;
        else if (s == "logic_and_LR")   return Type::logic_and_LR;
        else if (s == "logic_not_U")    return Type::logic_not_U;
        else if (s == "logic_not_D")    return Type::logic_not_D;
        else if (s == "logic_not_L")    return Type::logic_not_L;
        else if (s == "logic_not_R")    return Type::logic_not_R;
        else if (s == "logic_or_UD")    return Type::logic_or_UD;
        else if (s == "logic_or_LR")    return Type::logic_or_LR;
        else if (s == "particle_sense") return Type::particle_sense;
        else if (s == "player")         return Type::player;
        else if (s == "slope_L_1x1")    return Type::slope_L_1x1;
        else if (s == "slope_L_2x1_0")  return Type::slope_L_2x1_0;
        else if (s == "slope_L_2x1_1")  return Type::slope_L_2x1_1;
        else if (s == "slope_R_1x1")    return Type::slope_R_1x1;
        else if (s == "slope_R_2x1_0")  return Type::slope_R_2x1_0;
        else if (s == "slope_R_2x1_1")  return Type::slope_R_2x1_1;
        else if (s == "spring_U")       return Type::spring_U;
        else if (s == "slope_U")        return Type::slope_U;
        else if (s == "swing")          return Type::swing;
        else if (s == "train")          return Type::train;

        else if (s == "track_U")       return Type::track_U;
        else if (s == "track_D")       return Type::track_D;
        else if (s == "track_L")       return Type::track_L;
        else if (s == "track_R")       return Type::track_R;

        else if (s == "track_UR")       return Type::track_UR;
        else if (s == "track_RD")       return Type::track_RD;
        else if (s == "track_DL")       return Type::track_DL;
        else if (s == "track_LU")       return Type::track_LU;
        else if (s == "track_LD")       return Type::track_LD;
        else if (s == "track_DR")       return Type::track_DR;
        else if (s == "track_RU")       return Type::track_RU;
        else if (s == "track_UL")       return Type::track_UL;
        
        else if (s == "track_LU_1x1")    return Type::track_LU_1x1;
        else if (s == "track_UL_1x1")    return Type::track_UL_1x1;
        else if (s == "track_LD_1x1")    return Type::track_LD_1x1;
        else if (s == "track_DL_1x1")    return Type::track_DL_1x1;
        else if (s == "track_RU_1x1")    return Type::track_RU_1x1;
        else if (s == "track_UR_1x1")    return Type::track_UR_1x1;
        else if (s == "track_RD_1x1")    return Type::track_RD_1x1;
        else if (s == "track_DR_1x1")    return Type::track_DR_1x1;

        else if (s == "track_LU_2x1_0")    return Type::track_LU_2x1_0;
        else if (s == "track_LU_2x1_1")    return Type::track_LU_2x1_1;
        else if (s == "track_LD_2x1_0")    return Type::track_LD_2x1_0;
        else if (s == "track_LD_2x1_1")    return Type::track_LD_2x1_1;

        else if (s == "track_RU_2x1_0")    return Type::track_RU_2x1_0;
        else if (s == "track_RU_2x1_1")    return Type::track_RU_2x1_1;
        else if (s == "track_RD_2x1_0")    return Type::track_RD_2x1_0;
        else if (s == "track_RD_2x1_1")    return Type::track_RD_2x1_1;

        else if (s == "track_UL_1x2_0")    return Type::track_UL_1x2_0;
        else if (s == "track_UL_1x2_1")    return Type::track_UL_1x2_1;
        else if (s == "track_UR_1x2_0")    return Type::track_UR_1x2_0;
        else if (s == "track_UR_1x2_1")    return Type::track_UR_1x2_1;
        
        else if (s == "track_DL_1x2_0")    return Type::track_DL_1x2_0;
        else if (s == "track_DL_1x2_1")    return Type::track_DL_1x2_1;
        else if (s == "track_DR_1x2_0")    return Type::track_DR_1x2_0;
        else if (s == "track_DR_1x2_1")    return Type::track_DR_1x2_1;   

        else if (s == "trigger")        return Type::trigger;
        return Type::null;
    }
    std::string type_to_string(const Type type) {
        switch (type) {
        case Type::arch_L_1x1:     return "arch_L_1x1";
        case Type::arch_R_1x1:     return "arch_R_1x1";
        case Type::arch_L_2x1_0:   return "arch_L_2x1_0";
        case Type::arch_L_2x1_1:   return "arch_L_2x1_1";
        case Type::arch_R_2x1_0:   return "arch_R_2x1_0";
        case Type::arch_R_2x1_1:   return "arch_R_2x1_1";
        case Type::bug:            return "bug";
        case Type::clip:           return "clip";
        case Type::clip_duck:      return "clip_duck";
        case Type::clip_L:         return "clip_L";
        case Type::clip_R:         return "clip_R";
        case Type::clip_D:         return "clip_D";
        case Type::clip_D_L:       return "clip_D_L";
        case Type::clip_D_R:       return "clip_D_R";
        case Type::clip_ledge:     return "clip_ledge";
        case Type::clip_ledge_L:   return "clip_ledge_L";
        case Type::clip_ledge_R:   return "clip_ledge_R";
        case Type::clip_U:         return "clip_U";        
        case Type::conduit_U:      return "conduit_U";
        case Type::conduit_D:      return "conduit_D";
        case Type::conduit_L:      return "conduit_L";
        case Type::conduit_R:      return "conduit_R";
        case Type::conduit_LU:     return "conduit_LU";
        case Type::conduit_RU:     return "conduit_RU";
        case Type::conduit_LD:     return "conduit_LD";
        case Type::conduit_RD:     return "conduit_RD";
        case Type::conduit_UL:     return "conduit_UL";
        case Type::conduit_UR:     return "conduit_UR";
        case Type::conduit_DL:     return "conduit_DL";
        case Type::conduit_DR:     return "conduit_DR";
        case Type::frog:           return "frog";
        case Type::level_center:   return "level_center";
        case Type::level_L_0:      return "level_L_0";
        case Type::level_R_0:      return "level_R_0";
        case Type::logic_and_UD:   return "logic_and_UD";
        case Type::logic_and_LR:   return "logic_and_LR";
        case Type::logic_not_U:    return "logic_not_U";
        case Type::logic_not_D:    return "logic_not_D";
        case Type::logic_not_L:    return "logic_not_L";
        case Type::logic_not_R:    return "logic_not_R";
        case Type::logic_or_UD:    return "logic_or_UD";
        case Type::logic_or_LR:    return "logic_or_LR";
        case Type::particle_sense: return "particle_sense";
        case Type::player:         return "player";
        case Type::slope_L_1x1:    return "slope_L_1x1";
        case Type::slope_R_1x1:    return "slope_R_1x1";
        case Type::slope_L_2x1_0:  return "slope_L_2x1_0";
        case Type::slope_L_2x1_1:  return "slope_L_2x1_1";
        case Type::slope_R_2x1_0:  return "slope_R_2x1_0";
        case Type::slope_R_2x1_1:  return "slope_R_2x1_1";
        case Type::slope_U:        return "slope_U";
        case Type::spring_U:       return "spring_U";
        case Type::swing:          return "swing";

        case Type::track_U:       return "track_U";
        case Type::track_D:       return "track_D";
        case Type::track_L:       return "track_L";
        case Type::track_R:       return "track_R";

        case Type::track_UR:      return "track_UR";
        case Type::track_RD:      return "track_RD";
        case Type::track_DL:      return "track_DL";
        case Type::track_LU:      return "track_LU";
        case Type::track_LD:      return "track_LD";
        case Type::track_DR:      return "track_DR";
        case Type::track_RU:      return "track_RU";
        case Type::track_UL:      return "track_UL";
        
        case Type::track_LU_1x1:    return "track_LU_1x1";
        case Type::track_UL_1x1:    return "track_UL_1x1";
        case Type::track_LD_1x1:    return "track_LD_1x1";
        case Type::track_DL_1x1:    return "track_DL_1x1";

        case Type::track_RU_1x1:    return "track_RU_1x1";
        case Type::track_UR_1x1:    return "track_UR_1x1";
        case Type::track_RD_1x1:    return "track_RD_1x1";
        case Type::track_DR_1x1:    return "track_DR_1x1";

        case Type::track_LU_2x1_0:    return "track_LU_2x1_0";
        case Type::track_LU_2x1_1:    return "track_LU_2x1_1";
        case Type::track_LD_2x1_0:    return "track_LD_2x1_0";
        case Type::track_LD_2x1_1:    return "track_LD_2x1_1";

        case Type::track_RU_2x1_0:    return "track_RU_2x1_0";
        case Type::track_RU_2x1_1:    return "track_RU_2x1_1";
        case Type::track_RD_2x1_0:    return "track_RD_2x1_0";
        case Type::track_RD_2x1_1:    return "track_RD_2x1_1";

        case Type::track_UL_1x2_0:    return "track_UL_1x2_0";
        case Type::track_UL_1x2_1:    return "track_UL_1x2_1";
        case Type::track_UR_1x2_0:    return "track_UR_1x2_0";
        case Type::track_UR_1x2_1:    return "track_UR_1x2_1";
        
        case Type::track_DL_1x2_0:    return "track_DL_1x2_0";
        case Type::track_DL_1x2_1:    return "track_DL_1x2_1";        
        case Type::track_DR_1x2_0:    return "track_DR_1x2_0";
        case Type::track_DR_1x2_1:    return "track_DR_1x2_1";        
        
        case Type::train:          return "train";
        case Type::trigger:        return "trigger";        
        default:                   return "";
        }
    }
    enum class ParticleType {
        null = 0,
        hit,
        sense_frog
    };
    struct ParticleToSpawn {
        ParticleType type     = ParticleType::null;
        Vec2f        position = { 0.0f, 0.0f },
                     velocity = { 0.0f, 0.0f };
    };

    class Object {
    protected:
        Object* m_parent = nullptr;

        Type m_type = Type::null;

        State m_state = State::null,
              m_next_state = State::null,
              m_prev_state = State::null;

        i32 m_transform_id = -1,
            m_sprite_id    = -1;

        i32 m_current_anim_id = -1,
            m_bounce_anim_id  = -1,
            m_dead_anim_id    = -1,
            m_hurt_anim_id    = -1,
            m_idle_anim_id    = -1,
            m_jump_anim_id    = -1,
            m_melee_anim_id   = -1,
            m_upended_anim_id = -1;

        i32 m_bounce_sound_id = -1,
            m_dead_sound_id   = -1,
            m_hurt_sound_id   = -1,
            m_melee_sound_id  = -1;

        std::vector<i32> m_aabb_ids;

        std::vector<ParticleToSpawn> m_particles_to_spawn;

        std::list<Object*> m_sensed_objects;

        u16 m_time_left_alive          =  U16_MAX, m_time_to_be_alive    = U16_MAX,
            m_time_left_dead           =  0,       m_time_to_be_dead     = 0,
            m_time_left_hurt           =  0,       m_time_to_hurt        = 10,
            m_time_left_interacting    =  0,       m_time_to_interact    = 10,
            m_time_left_to_spawn_sense =  0,       m_time_to_spawn_sense = 10,
            m_time_left_in_state       =  0;

        Vec2f m_start_offset      = { 0.0f, 0.0f };

        u8    m_start_layer = 0;

        Recti m_start_source_rect = { 0, 0, 16, 16 };

        Vec2f m_direction;

        bool m_is_first_update = true,
             m_is_first_state_update = true;

        bool m_is_carrying  = false,
             m_is_ducking   = false, 
             m_is_on_ground = false,
             m_is_on_slope  = false,
             m_is_powered   = false;

        u16 m_input_limit = U16_MAX;
        u16c num_inputs() const { return input_objects.size(); }
        
    public:
        Object* parent() const { return m_parent; } void parent(Object* p) { m_parent = p; }
        //Object* sensed() const { return m_sensed; } void sensed(Object* s) { m_sensed = s; }

        i32 level_transform_id = -1;

        bool add_input_object(std::shared_ptr<Object> object) {
            if (input_objects.size() >= m_input_limit) {
                return false;
            }
            if (std::find(input_objects.begin(), input_objects.end(), object) != input_objects.end()) {
                return false;
            }
            if (object->is_powered()) {
                m_is_powered = true;
            }
            input_objects.push_back(object);
            return true;
        }
        std::vector<std::shared_ptr<Object>> input_objects;

        bool add_sensed_object(Object* object) {            
            if (std::find(m_sensed_objects.begin(), m_sensed_objects.end(), object) != m_sensed_objects.end()) {
                return false;
            }
            m_sensed_objects.push_back(object);
            return true;
        }

        virtual void hurt() {
            if (m_time_left_hurt == m_time_to_hurt) return;
            reset_anim(m_hurt_anim_id);
            next_state(entity::State::hurt);
            m_time_left_hurt = m_time_to_hurt;
        }
        bool is_carrying()    const { return m_is_carrying; }   void is_carrying(bool is) { m_is_carrying = is; }
        bool is_dead()        const { return m_time_left_dead != 0; }
        bool is_ducking()     const { return m_is_ducking; }
        bool is_hurting()     const { return m_time_left_hurt > 0; }

        std::vector<ParticleToSpawn>& particles_to_spawn() {
            return m_particles_to_spawn;
        }

        bool is_interacting() const { return m_time_left_interacting > 0; }
        bool is_powered()     const { return m_is_powered; }    void is_powered(bool is) { m_is_powered = is; }

        Typec  type()  const { return m_type; }  void type(Typec t) { m_type = t; }
        Statec state() const { return m_state; }  void state(Statec s) { m_state = s; }

        Vec2fc position() const { return transform::Set::at(m_transform_id) ? transform::Set::at(m_transform_id)->position : Vec2f{ 0.0f,0.0f }; }  void position(Vec2fc p) { if (transform::Set::at(m_transform_id)) transform::Set::at(m_transform_id)->position = p; }
        f32c   x()        const { return transform::Set::at(m_transform_id) ? transform::Set::at(m_transform_id)->position.x : 0.0f; }              void x(f32c in_x) { if (transform::Set::at(m_transform_id)) transform::Set::at(m_transform_id)->position.x = in_x; }
        f32c   y()        const { return transform::Set::at(m_transform_id) ? transform::Set::at(m_transform_id)->position.y : 0.0f; }              void y(f32c in_y) { if (transform::Set::at(m_transform_id)) transform::Set::at(m_transform_id)->position.y = in_y; }
        Vec2fc velocity() const { return transform::Set::at(m_transform_id) ? transform::Set::at(m_transform_id)->velocity : Vec2f{ 0.0f, 0.0f }; } void velocity(Vec2fc v) { if (transform::Set::at(m_transform_id)) transform::Set::at(m_transform_id)->velocity = v; }

        Vec2fc direction() const { return m_direction; }    void direction(Vec2fc d) { m_direction = d; }

        Vec2fc start_offset() const { return m_start_offset; }  void start_offset(Vec2fc offset) { m_start_offset = offset; }
        u8c    start_layer()  const { return m_start_layer; }  void start_layer(u8c layer) { m_start_layer = layer; }

        Rectic start_source_rect() const { return m_start_source_rect; }  void start_source_rect(Rectic r) { m_start_source_rect = r; }

        u16c time_left_dead()  const { return m_time_left_dead;  }  void time_left_dead(u16c t) { m_time_left_dead = t;   }
        u16c time_left_alive() const { return m_time_left_alive; }  void time_left_alive(u16c t) { m_time_left_alive = t; }

        u16c time_left_interacting() const { return m_time_left_interacting; } void time_left_interacting(u16c t) { m_time_left_interacting = t; }
        u16c time_left_in_State()    const { return m_time_left_in_state;    } void time_left_in_state(u16c t)    { m_time_left_in_state = t;    }

        anim::Object* anim() { return anim::Set::at(m_current_anim_id); }

        void next_state(State state) {
            m_prev_state = m_state;
            m_state = state;
            m_is_first_state_update = true;
        }

        Object() {}
        virtual ~Object();
        virtual bool is_all_valid() {
            return m_sprite_id != -1 && sprite() && m_current_anim_id != -1 && anim::Set::at(m_current_anim_id) && m_transform_id != -1 && transform::Set::at(m_transform_id);
        }
        virtual bool load_config(const std::filesystem::path& path);
        virtual void collide_x(aabb::Infoc our, aabb::Infoc other) {}
        virtual void collide_y(aabb::Infoc our, aabb::Infoc other) {}
        virtual void update() {
            if (!is_all_valid()) return;
            //anim::Set::at(m_current_anim_id)->update();
            sprite()->source_rect.x = anim::Set::at(m_current_anim_id)->source.x;
            sprite()->source_rect.y = anim::Set::at(m_current_anim_id)->source.y;

            if (m_time_left_dead > 0) {
                //Console::log("m_time_left_dead: ", (int)m_time_left_dead, "\n");
                if (m_time_left_dead != -1) { // u16 max, remain dead
                    --m_time_left_dead;
                }
                m_current_anim_id = m_dead_anim_id;
            } else {
                m_current_anim_id = m_idle_anim_id;
            }
        }
        /*std::vector<Vec2f> points() {
            std::vector<Vec2f> p;
            for (auto& i : m_aabb_ids) {
                if (aabb::Set::at(i)) {
                    for (auto& point : aabb::Set::at(i)->points()) {
                        p.push_back(point);
                    }
                }
            }
            return p;
        }*/

        void set_anim(i32c anim_id) {
            if (m_current_anim_id == anim_id || anim_id == -1) return;
            m_current_anim_id = anim_id;
        }
        void reset_anim(i32c anim_id) {
            if (anim_id == -1) return;
            //if (m_current_anim_id == anim_id) return;
            m_current_anim_id = anim_id;
            if (anim::Set::at(anim_id)) {
                anim::Set::at(anim_id)->loop = 0;
                anim::Set::at(anim_id)->source.x = 0;
            }
        }
        i32c sprite_id()    const { return m_sprite_id;    }    void sprite_id(i32c id)    { m_sprite_id = id;    }
        i32c transform_id() const { return m_transform_id; }    void transform_id(i32c id) { m_transform_id = id; }
        //i32c current_anim_id() const { return m_current_anim_id; }

        transform::Object* transform() { return transform::Set::is_valid(m_transform_id) ? transform::Set::at(m_transform_id) : nullptr; }
        sprite::Object* sprite() { return sprite::Set::is_valid(m_sprite_id) ? sprite::Set::at(m_sprite_id) : nullptr; }

        std::vector<i32>& aabb_ids() { return m_aabb_ids; }

        void set_layer(u8c layer) { sprite::Set::at(m_sprite_id)->layer = layer; }

        Rectic source_rect() const { return sprite::Set::at(m_sprite_id) ? sprite::Set::at(m_sprite_id)->source_rect : Recti{ 0, 0, 0, 0 }; }
    };
}