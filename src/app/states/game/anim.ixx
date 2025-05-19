module;
#include <vector>

export module anim;
import console;
import types;

export namespace anim {
    enum class Type {
        idle,
        run
    };
    struct Object {
        i32 id = -1;
        f32 timer = 0.0f;

        Recti source;
        f32 speed = 1.0f;
        u16 loop = 0;

        u16 loops = 0;

        Vec2u texture_size = { 16, 16 };

        u16 first_frame = 0;

        void update() {
            timer += speed;
            if (timer < 1.0f) {
                return;
            }
            timer = 0.0f;
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

    class Set {
        static inline std::vector<Object*>  m_objects;
        static inline std::vector<i32>      m_unused_ids;

    public:
        static Object* at(size_t i)       { return (i >= 0 && i < m_objects.size()) ? m_objects.at(i) : nullptr;       }
        static bool    is_valid(size_t i) { return (i >= 0 && i < m_objects.size() && m_objects.at(i)) ? true : false; }
        static size_t  size()             { return m_objects.size(); }

        static i32 make() {
            Object* object = new Object;
            if (!m_unused_ids.empty()) {
                object->id = m_unused_ids.back();
                m_unused_ids.pop_back();
            } else {
                object->id = m_objects.size();
                m_objects.push_back(nullptr);
            }
            if (object->id >= 0 && object->id < m_objects.size() && m_objects.at(object->id)) {
                delete m_objects.at(object->id);
            }
            m_objects.at(object->id) = object;
            return object->id;
        }
        static bool erase(size_t i) {
            if (!(i < m_objects.size()) || !m_objects.at(i)) {
                Console::log("transform::Set::erase ", i, "can't do it\n");
                return false;
            }
            delete m_objects.at(i);
            m_objects.at(i) = nullptr;
            m_unused_ids.push_back(i);
            return true;
        }
        static void update() {
            for (auto& i : m_objects) {
                if (!i) continue;
                i->update();
            }
        }
        static void clear() {
            for (size_t i = 0; i < m_objects.size(); ++i) {
                erase(i);
            }
            m_objects.clear();
            m_unused_ids.clear();
        }
    };


}