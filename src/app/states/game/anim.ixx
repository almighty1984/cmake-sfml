module;
#include <vector>
#include <sstream>
#include <string>
#include <map>

export module anim;
import console;
import types;

export namespace anim {
    enum class Type {
        none = 0,
        bounce,
        crawl,
        dead, down_thrust, duck,
        fall,
        hurt, hit_ground, hover,
        idle,
        jump, jump_spin, jump_skid,
        ledge_grab, ledge_climb, lever,
        melee,
        rise, run,
        skid, slide_ground, slide_wall, stunned, swim, swing,
        walk,
        upended
    };
    using cType = const Type;
    const std::string to_string(cType t) {
        switch (t) {        
        case Type::bounce:       return "bounce";
        case Type::crawl:        return "crawl";
        case Type::dead:         return "dead";
        case Type::down_thrust:  return "down_thrust";
        case Type::duck:         return "duck";
        case Type::fall:         return "fall";
        case Type::hurt:         return "hurt";
        case Type::hit_ground:   return "hit_ground";
        case Type::hover:        return "hover";
        case Type::idle:         return "idle";
        case Type::jump:         return "jump";
        case Type::jump_spin:    return "jump_spin";
        case Type::jump_skid:    return "jump_skid";
        case Type::ledge_grab:   return "ledge_grab";
        case Type::ledge_climb:  return "ledge_climb";
        case Type::lever:        return "lever";
        case Type::melee:        return "melee";
        case Type::rise:         return "rise";
        case Type::run:          return "run";
        case Type::skid:         return "skid";
        case Type::slide_ground: return "slide_ground";
        case Type::slide_wall:   return "slide_wall";
        case Type::stunned:      return "stunned";
        case Type::swim:         return "swim";
        case Type::swing:        return "swing";
        case Type::walk:         return "walk";
        case Type::upended:      return "upended";
        default:                 return "";            }
    }
    const std::map<std::string, Type> string_to_anim_type_map{
        { "bounce"        , Type::bounce       },
        { "crawl"         , Type::crawl        },
        { "dead"          , Type::dead         },
        { "down_thrust"   , Type::down_thrust  },
        { "duck"          , Type::duck         },
        { "fall"          , Type::fall         },
        { "hurt"          , Type::hurt         },
        { "hit_ground"    , Type::hit_ground   },
        { "hover"         , Type::hover        },
        { "idle"          , Type::idle         },
        { "jump"          , Type::jump         },
        { "jump_spin"     , Type::jump_spin    },
        { "jump_skid"     , Type::jump_skid    },
        { "ledge_grab"    , Type::ledge_grab   },
        { "ledge_climb"   , Type::ledge_climb  },
        { "lever"         , Type::lever        },
        { "melee"         , Type::melee        },
        { "rise"          , Type::rise         },
        { "run"           , Type::run          },
        { "skid"          , Type::skid         },
        { "slide_ground"  , Type::slide_ground },
        { "slide_wall"    , Type::slide_wall   },
        { "stunned"       , Type::stunned      },
        { "swim"          , Type::swim         },
        { "swing"         , Type::swing        },
        { "walk"          , Type::walk         },
        { "upended"       , Type::upended      }
    };
    cType string_to_type(const std::string& s) {
        if (string_to_anim_type_map.find(s) != string_to_anim_type_map.end()) {
            return string_to_anim_type_map.at(s);
        }
        return Type::none;        
    }
    struct Object {
    private:
        F32 m_timer;
    public:
        I32 id;
        Type type;
        F32 speed;
        U16 loop, loops;
        U16 first_frame;
        RectI source;
        Vec2U texture_size;
        Object() : m_timer(0.0F),
                   id(-1),
                   type(Type::none),
                   speed(1.0F),
                   loop(0), loops(0),
                   first_frame(0),
                   source(0, 0, 16, 16),
                   texture_size(16, 16) {
        }

        cU16 num_frames()    const { return texture_size.x / source.w; }
        cU16 current_frame() const { return source.x / source.w;       }

        bool is_last_frame() const { return current_frame() == num_frames() - 1; }

        void update() {
            m_timer += speed;
            if (m_timer < 1.0F) {
                return;
            }
            m_timer = 0.0F;
            source.x += source.w;
            
            if (source.x >= texture_size.x) {
                if (loops == 0) {
                    source.x = first_frame * source.w;
                } else {
                    loop += 1;
                    if (loop >= loops) {
                        source.x = texture_size.x - source.w;
                        loop = loops;
                    } else {                        
                        source.x = 0;
                    }
                }
            }
            //Console::log("source: ", source.x, "\n");
        }
    };

    class Manager {
        static inline std::vector<Object*>  s_objects;
        static inline std::vector<I32>      s_unused_ids;

    public:
        static Object* get(cI32 i)      { return (i >= 0 && i < s_objects.size()) ? s_objects.at(i) : nullptr;       }
        static bool    is_valid(cI32 i) { return (i >= 0 && i < s_objects.size() && s_objects.at(i)) ? true : false; }
        static size_t  size()           { return s_objects.size(); }

        static I32 make() {
            Object* object = new Object;
            if (!s_unused_ids.empty()) {
                object->id = s_unused_ids.back();
                s_unused_ids.pop_back();                
                if (!s_objects.empty() && object->id >= 0 && object->id < s_objects.size() && s_objects.at(object->id)) {
                    delete s_objects.at(object->id);
                    //s_objects.at(object->id) = nullptr;
                }
                s_objects.at(object->id) = object;
            } else {
                object->id = s_objects.size();                
                s_objects.emplace_back(object);
            }
            return object->id;
        }
        static bool erase(cI32 i) {
            if (i < 0) return false;
            if (s_objects.empty() || i > s_objects.size() -1 || !s_objects.at(i)) {
                Console::log("anim::Manager::erase ", i, " can't do it!\n");
                return false;
            }
            delete s_objects.at(i);
            s_objects.at(i) = nullptr;
            s_unused_ids.emplace_back(i);
            return true;
        }
        static void update() {
            for (const auto& i : s_objects) {
                if (i) i->update();
            }
        }
        static void clear() {
            for (size_t i = 0; i < s_objects.size(); ++i) {
                erase(i);
            }
            s_objects.clear();
            s_unused_ids.clear();
        }
    };


}