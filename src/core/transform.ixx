module;
#include <vector>

export module transform;

import console;
import types;

export namespace transform {
    struct Object {
        I32   id;
        Vec2F position,      start_position,
              velocity,      moved_velocity,
              acceleration,  deceleration,
              max_velocity,  start_max_velocity;

        Object() : id(-1),
                   position({ 0.0F, 0.0F }), start_position({ 0.0F, 0.0F }),
                   velocity({ 0.0F, 0.0F }), moved_velocity({ 0.0F, 0.0F }),
                   acceleration({ 0.1F, 0.1F }), deceleration({ 0.0F, 0.0F }),
                   max_velocity({ 10.0F, 10.0F }), start_max_velocity({ 10.0F, 10.0F }) {
        }
        /*Object(cVec2F p) : Object() {
            start_position = p;
        }*/
        void update() {
            //moved_velocity = decelerate(moved_velocity, deceleration);
            if (moved_velocity != Vec2F(0.0F, 0.0F)) {
                moved_velocity = clamp(moved_velocity, max_velocity);
            }
            if (velocity != Vec2F(0.0F, 0.0F)) {
                velocity = decelerate(velocity, deceleration);
                velocity = clamp(velocity, max_velocity);
            }           
            
            position += velocity + moved_velocity;
            
        }
    private:
        Vec2F clamp(Vec2F v, Vec2F limit) {
            if (limit == Vec2F{ 0.0F, 0.0F }) return v;
            if (v.x < -limit.x) {
                v.x = -limit.x;
                //v.x += acceleration.x;                
                /*if (v.x > -limit.x) {
                    v.x = -limit.x;
                }*/
            } else if (v.x > limit.x) {
                v.x = limit.x;
                //v.x -= acceleration.x;                
                /*if (v.x < limit.x) {
                    v.x = limit.x;
                }*/
            }
            if (v.y < -limit.y) {
                v.y = -limit.y;
                //v.y += acceleration.y;                
                /*if (v.y > -limit.y) {
                    v.y = -limit.y;
                }*/
            } else if (v.y > limit.y) {
                v.y = limit.y;
                //v.y -= acceleration.y;                
                /*if (v.y < limit.y) {
                    v.y = limit.y;
                }*/
            }
            return v;
        }
        Vec2F decelerate(Vec2F v, Vec2F amount) {
            if (v.x != 0.0F && amount.x != 0.0F) {
                if (v.x > 0.0F && v.x < amount.x || v.x < 0.0F && v.x > -amount.x) {
                    v.x = 0.0F;
                } else {
                    if      (v.x > 0.0F) v.x -= amount.x;
                    else if (v.x < 0.0F) v.x += amount.x;
                }
            }
            if (v.y != 0.0F && amount.y != 0.0F) {
                if (v.y > 0.0F && v.y < amount.y || v.y < 0.0F && v.y > -amount.y) {
                    v.y = 0.0F;
                } else {
                    if      (v.y > 0.0F) v.y -= amount.y;
                    else if (v.y < 0.0F) v.y += amount.y;                    
                }
            }
            return v;
        }
    };

    class Manager {
        static inline std::vector<Object*>  s_objects;
        static inline std::vector<size_t>   s_unused_ids;

    public:
        static Object* get(size_t i)      { return (i >= 0 && i < s_objects.size()) ? s_objects.at(i) : nullptr; }
        static bool    is_valid(size_t i) { return (i >= 0 && i < s_objects.size() && s_objects.at(i)) ? true : false; }
        static size_t  size()             { return s_objects.size();    }
        static size_t  unused_size()      { return s_unused_ids.size(); }

        static I32 make() {
            Object* object = new Object;
            if (!s_unused_ids.empty()) {
                object->id = s_unused_ids.back();
                s_unused_ids.pop_back();
                //Console::log("transform::Manager::make unused: ", object->id, "\n");
                if (!s_objects.empty() && object->id >= 0 && object->id < s_objects.size() && s_objects.at(object->id)) {
                    delete s_objects.at(object->id);
                    //s_objects.at(object->id) = nullptr;
                }
                s_objects.at(object->id) = object;
            } else {
                object->id = s_objects.size();
                //Console::log("transform::Manager::make id: ", object->id, "\n");
                s_objects.emplace_back(object);
            }
            return object->id;
        }
        static bool erase(const size_t i) {
            if (i >= 0 && !s_objects.empty() && i < s_objects.size() && s_objects.at(i)) {
                //Console::log("transform::Manager::erase ", i, "\n");
                delete s_objects.at(i);
                s_objects.at(i) = nullptr;
                s_unused_ids.emplace_back(i);
                return true;
            }
            //Console::log("transform::Manager::erase ", i, " can't do it! size: ", s_objects.size(), "\n");
            return false;
        }
        static void update() {
            for (auto& i : s_objects) {
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