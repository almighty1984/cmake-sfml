module;
#include <vector>

export module transform;

import console;
import types;

export namespace transform {
    struct Object {
        i32   id = -1;
        Vec2f position             = { 0.0f, 0.0f },
              start_position       = { 0.0f, 0.0f },
              velocity             = { 0.0f, 0.0f },              
              moved_velocity       = { 0.0f, 0.0f },
              acceleration         = { 0.0f, 0.0f },
              deceleration         = { 0.0f, 0.0f },
              velocity_limit       = { 0.0f, 0.0f },
              start_velocity_limit = { 0.0f, 0.0f };

        Object() : id(-1),
                   position({ 0.0f, 0.0f }), start_position({ 0.0f, 0.0f }),
                   velocity({ 0.0f, 0.0f }), moved_velocity({ 0.0f, 0.0f }),
                   acceleration({ 0.0f, 0.0f }), deceleration({ 0.0f, 0.0f }),
                   velocity_limit({ 0.0f, 0.0f }), start_velocity_limit({ 0.0f, 0.0f }) {
        }
        Object(Vec2f p) : Object() {
            start_position = p;
        }
        ~Object() {
            //Console::log("transform::~Object()\n");
            id = -1;
        }
        void update() {
            //moved_velocity = decelerate(moved_velocity, deceleration);
            moved_velocity = clamp(moved_velocity, velocity_limit);

            velocity = decelerate(velocity, deceleration);
            velocity = clamp(velocity, velocity_limit);

            position += velocity + moved_velocity;            
        }
    private:
        Vec2f clamp(Vec2f v, Vec2f limit) {
            if (limit == Vec2f{ 0.0f, 0.0f }) return v;
            if (v.x < -limit.x) {
                v.x = -limit.x;
            } else if (v.x > limit.x) {
                v.x = limit.x;
            }
            if (v.y < -limit.y) {
                v.y = -limit.y;
            } else if (v.y > limit.y) {
                v.y = limit.y;
            }
            return v;
        }
        Vec2f decelerate(Vec2f v, Vec2f amount) {
            if (v.x != 0.0f && amount.x != 0.0f) {
                if (v.x > 0.0f && v.x < amount.x || v.x < 0.0f && v.x > -amount.x) {
                    v.x = 0.0f;
                } else {
                    if (v.x > 0.0f) {
                        v.x -= amount.x;
                    } else if (v.x < 0.0f) {
                        v.x += amount.x;
                    }
                }
            }
            if (v.y != 0.0f && amount.y != 0.0f) {
                if (v.y > 0.0f && v.y < amount.y || v.y < 0.0f && v.y > -amount.y) {
                    v.y = 0.0f;
                } else {
                    if (v.y > 0.0f) {
                        v.y -= amount.y;
                    } else if (v.y < 0.0f) {
                        v.y += amount.y;
                    }
                }
            }
            return v;
        }
    };

    class Set {
        static inline std::vector<Object*>  m_objects;
        static inline std::vector<i32>      m_unused_ids;

    public:
        static Object* at(size_t i) { return (i >= 0 && i < m_objects.size()) ? m_objects.at(i) : nullptr; }
        static bool    is_valid(size_t i) { return (i >= 0 && i < m_objects.size() && m_objects.at(i)) ? true : false; }
        static size_t  size() { return m_objects.size(); }

        static i32 make() {
            Object* object = new Object;
            if (!m_unused_ids.empty()) {
                object->id = m_unused_ids.back();
                m_unused_ids.pop_back();                
                if (!m_objects.empty() && object->id >= 0 && object->id < m_objects.size() && m_objects.at(object->id)) {
                    delete m_objects.at(object->id);
                    m_objects.at(object->id) = nullptr;
                }
            } else {
                object->id = m_objects.size();
                m_objects.push_back(nullptr);
            }            
            m_objects.at(object->id) = object;
            return object->id;
        }
        static bool erase(size_t i) {
            if (!(i < m_objects.size()) || !m_objects.at(i)) {
                //Console::log("transform::Set::erase ", i, " can't do it!\n");
                return false;
            }
            delete m_objects.at(i);
            m_objects.at(i) = nullptr;
            m_unused_ids.push_back(i);
            return true;
        }
        static void update() {
            for (auto& i : m_objects) {
                if (i) i->update();
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