module;
#include <vector>
#include <memory>
export module collider.aabb;
//import collider;
import console;
import line;
import sprite;
import entity;
import types;
import transform;
import window;

export namespace aabb {
    class Object {
        bool m_is_active = true;

        I32 m_transform_id = -1;

        I32 m_up_line_id    = -1,
            m_down_line_id  = -1,
            m_left_line_id  = -1,
            m_right_line_id = -1;

        //Vec2F m_position = { 0.0F, 0.0F };

        RectF m_rect = { 0.0F, 0.0F, 0.0F, 0.0F };

        Color m_color       = { 255, 255, 255 },
              m_start_color = { 255, 255, 255 };
    public:
        I32 id = -1;

        entity::Object* owner = nullptr;
        U16 time_left_colliding = 0;
        Name name = Name::none;
        
        void collide_x(aabb::cInfo our, aabb::cInfo other) {
            if (owner) {
                owner->collide_x(our, other);
            }
        }
        void collide_y(aabb::cInfo our, aabb::cInfo other) {
            if (owner) {
                owner->collide_y(our, other);
            }
        }
        bool is_active() { return m_is_active; }
        void is_active(bool is) {
            m_is_active = is;
            if (!line::Manager::get(m_up_line_id) || !line::Manager::get(m_down_line_id) || !line::Manager::get(m_left_line_id) || !line::Manager::get(m_right_line_id)) return;
            line::Manager::get(m_up_line_id)->is_hidden = line::Manager::get(m_down_line_id)->is_hidden = line::Manager::get(m_left_line_id)->is_hidden = line::Manager::get(m_right_line_id)->is_hidden = !is;
        }

        cI32 transform_id() { return m_transform_id; }

        transform::Object* transform() { return transform::Manager::is_valid(m_transform_id) ? transform::Manager::get(m_transform_id) : nullptr; }
                
        std::vector<size_t> quad_node_ids;

        cRectF get_rect() const { return m_rect; }
        void set_rect(cRectF rect) {
            if (!line::Manager::get(m_up_line_id) || !line::Manager::get(m_down_line_id) || !line::Manager::get(m_left_line_id) || !line::Manager::get(m_right_line_id)) return;

            line::Manager::get(m_up_line_id   )->set({ rect.x, rect.y },          { rect.x + rect.w, rect.y });
            line::Manager::get(m_down_line_id )->set({ rect.x, rect.y + rect.h }, { rect.x + rect.w, rect.y + rect.h });
            line::Manager::get(m_left_line_id )->set({ rect.x, rect.y },          { rect.x,          rect.y + rect.h });
            line::Manager::get(m_right_line_id)->set({ rect.x + rect.w, rect.y }, { rect.x + rect.w, rect.y + rect.h });
            m_rect = rect;
        }

        Object() = delete;
        Object(cI32 transform_id, cRectF rect) {
            m_up_line_id    = line::Manager::make({ rect.x, rect.y },          { rect.x + rect.w, rect.y });
            m_down_line_id  = line::Manager::make({ rect.x, rect.y + rect.h }, { rect.x + rect.w, rect.y + rect.h });
            m_left_line_id  = line::Manager::make({ rect.x, rect.y },          { rect.x,          rect.y + rect.h });
            m_right_line_id = line::Manager::make({ rect.x + rect.w, rect.y }, { rect.x + rect.w, rect.y + rect.h });

            line::Manager::get(m_up_line_id)->layer    = NUM_VISIBLE_LAYERS - 1;
            line::Manager::get(m_down_line_id)->layer  = NUM_VISIBLE_LAYERS - 1;
            line::Manager::get(m_left_line_id)->layer  = NUM_VISIBLE_LAYERS - 1;
            line::Manager::get(m_right_line_id)->layer = NUM_VISIBLE_LAYERS - 1;
            line::Manager::get(m_up_line_id)->transform_id    = transform_id;
            line::Manager::get(m_down_line_id)->transform_id  = transform_id;
            line::Manager::get(m_left_line_id)->transform_id  = transform_id;
            line::Manager::get(m_right_line_id)->transform_id = transform_id;
            line::Manager::get(m_up_line_id)->is_debug    = true;
            line::Manager::get(m_down_line_id)->is_debug  = true;
            line::Manager::get(m_left_line_id)->is_debug  = true;
            line::Manager::get(m_right_line_id)->is_debug = true;
            m_transform_id = transform_id;
            m_rect = rect;
        }
        ~Object() {
            line::Manager::erase(m_up_line_id);
            line::Manager::erase(m_down_line_id);
            line::Manager::erase(m_left_line_id);
            line::Manager::erase(m_right_line_id);
        }

        cVec2F point_0() { return transform::Manager::get(m_transform_id) ? transform::Manager::get(m_transform_id)->position + Vec2F{ m_rect.x,  m_rect.y } : Vec2F{ m_rect.x, m_rect.y }; }
        cVec2F point_1() { return transform::Manager::get(m_transform_id) ? transform::Manager::get(m_transform_id)->position + Vec2F{ m_rect.x + m_rect.w, m_rect.y } : Vec2F{ m_rect.x + m_rect.w, m_rect.y }; }
        cVec2F point_2() { return transform::Manager::get(m_transform_id) ? transform::Manager::get(m_transform_id)->position + Vec2F{ m_rect.x,  m_rect.y + m_rect.h } : Vec2F{ m_rect.x, m_rect.y + m_rect.h }; }
        cVec2F point_3() { return transform::Manager::get(m_transform_id) ? transform::Manager::get(m_transform_id)->position + Vec2F{ m_rect.x + m_rect.w, m_rect.y + m_rect.h } : Vec2F{ m_rect.x + m_rect.w, m_rect.y + m_rect.h }; }

        cF32 width()  const { return m_rect.w; }
        cF32 height() const { return m_rect.h; }
        
        /*cVec2F point_0() { return Vec2F{ m_rect.x, m_rect.y }; }
        cVec2F point_1() { return Vec2F{ m_rect.x + m_rect.w, m_rect.y }; }
        cVec2F point_2() { return Vec2F{ m_rect.x, m_rect.y + m_rect.h }; }
        cVec2F point_3() { return Vec2F{ m_rect.x + m_rect.w, m_rect.y + m_rect.h }; }*/

        const std::vector<Vec2F> points() {
            return { point_0(), point_1(), point_2(), point_3() };
        }

        void color(Color c) {
            m_color = c;
            if (line::Manager::get(m_up_line_id))    line::Manager::get(m_up_line_id)->color = c;
            if (line::Manager::get(m_down_line_id))  line::Manager::get(m_down_line_id)->color = c;
            if (line::Manager::get(m_left_line_id))  line::Manager::get(m_left_line_id)->color = c;
            if (line::Manager::get(m_right_line_id)) line::Manager::get(m_right_line_id)->color = c;
        }
        Color color() { return m_color; }
        Color start_color() {
            return m_start_color;
        }
        void start_color(Color c) {
            m_start_color = c;
        }

        void update() {
            /*if (time_left_colliding > 0) {
                --time_left_colliding;
                color({ 255,0,0 });
            } else {
                color(start_color());
            }*/

            /*if (overlap != 0.0F) {
                color = { 255,0,0 };
                Console::log("overlap: ", overlap.x, " ", overlap.y, "\n");
                overlap = 0.0F;
            }
            else {
                color = start_color;
            }*/
            line::Manager::get(m_up_line_id)->color = m_color;
            line::Manager::get(m_down_line_id)->color = m_color;
            line::Manager::get(m_left_line_id)->color = m_color;
            line::Manager::get(m_right_line_id)->color = m_color;

            //Console::log("color: ", (int)color.r, "\n");

            /*for (auto& i : quad_node_ids) {
                Console::log("id: ", i, "\n");
            }*/
        }

        void draw(std::unique_ptr<Window>& window) {
            window->draw(line::Manager::get(m_up_line_id));
            window->draw(line::Manager::get(m_down_line_id));
            window->draw(line::Manager::get(m_left_line_id));
            window->draw(line::Manager::get(m_right_line_id));
        }
    };

    class Manager {
        static inline std::vector<Object*> s_objects;
        static inline std::vector<I32>     s_unused_ids;
    public:
        static std::vector<Object*>& get_objects() { return s_objects; }

        static bool    is_valid(size_t i) { return (i < s_objects.size() && s_objects.at(i)) ? true : false; }
        static size_t  size() { return s_objects.size(); }
        static Object* get(size_t i) { return (i < s_objects.size()) ? s_objects.at(i) : nullptr; }

        static cI32 make(cI32 transform_id, cRectF rect) {
            Object* object = new Object(transform_id, rect);
            if (!s_unused_ids.empty()) {
                object->id = s_unused_ids.back();
                s_unused_ids.pop_back();
            } else {
                object->id = s_objects.size();
                s_objects.emplace_back(nullptr);
            }
            if (object->id >= 0 && object->id < s_objects.size() && s_objects.at(object->id)) {
                delete s_objects.at(object->id);
            }
            s_objects.at(object->id) = object;
            return object->id;
        }
        static bool erase(cI32 i) {
            if (i < 0 || s_objects.empty() || i > s_objects.size() - 1 || !s_objects.at(i)) {
                //Console::log("aabb::Manager::erase ", i, " can't do it\n");
                return false;
            }
            //Console::log("aabb::Manager::erase ", i, "\n");
            delete s_objects.at(i);
            s_objects.at(i) = nullptr;
            s_unused_ids.emplace_back(i);
            return true;
        }
        static void update() {
            for (auto& i : s_objects) {
                if (i) i->update();                
            }
        }
        static void clear() {
            for (size_t i = 0; i < s_objects.size(); ++i) {
                //Console::log("aabb::Manager::clear erasing ", i, "\n");
                if (s_objects.at(i)) {
                    erase(i);
                }
            }
            s_objects.clear();
            s_unused_ids.clear();
        }
    };
}
