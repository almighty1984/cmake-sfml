module;
#include <vector>
export module collider;
import console;
import line;
import sprite;
import tile;
import types;
import transform;

export namespace collider {
    /*struct Info {
        sprite::Type type;        
        Vec2f position;
        Vec2f velocity;
    };*/

    enum class Type {
        null,
        aabb
    };

    class Object {
    protected:
        //f32 m_time_to_collide = 5.0f;
        //f32 m_time_left_colliding = 0.0f;

        i32 m_transform_id = -1;
        bool m_is_inactive = false;

    public:
        i32 id = -1;
        Type type = Type::null;
        tile::Object* parent = nullptr;

        void collide_x(sprite::Infoc our, sprite::Infoc other) {
            if (parent) {
                parent->collide_x(our, other);
            }
        }
        void collide_y(sprite::Infoc our, sprite::Infoc other) {
            if (parent) {
                parent->collide_y(our, other);
            }
        }

        virtual bool is_inactive() { return m_is_inactive; }  virtual void is_inactive(bool is) { m_is_inactive = is; }

        virtual std::vector<Vec2f> points() { return {}; }

        virtual void color(Color c) {}
        virtual Color start_color() { return {}; }

        i32c transform_id() { return m_transform_id; }

        transform::Object* transform() { return transform::Set::is_valid(m_transform_id) ? transform::Set::at(m_transform_id) : nullptr; }
    };    
}


export namespace aabb {
    class Object : public collider::Object {
        i32 m_line_up_id = -1,
            m_line_down_id = -1,
            m_line_left_id = -1,
            m_line_right_id = -1;

        //Vec2f m_position = { 0.0f, 0.0f };

        Rectf m_rect = { 0.0f, 0.0f, 0.0f, 0.0f };


        Color m_color = { 255, 255, 255 },
            m_start_color = { 255, 255, 255 };
    public:
        
        bool is_inactive() override { return m_is_inactive; }
        void is_inactive(bool is) override {
            m_is_inactive = is;
            if (!line::Set::at(m_line_up_id) || !line::Set::at(m_line_down_id) || !line::Set::at(m_line_left_id) || !line::Set::at(m_line_right_id)) return;
            line::Set::at(m_line_up_id)->is_hidden = line::Set::at(m_line_down_id)->is_hidden = line::Set::at(m_line_left_id)->is_hidden = line::Set::at(m_line_right_id)->is_hidden = is;
        }
        std::vector<size_t> quad_node_ids;

        Rectfc get_rect() const { return m_rect; }
        void set_rect(i32c transform_id, Rectfc rect) {
            if (!line::Set::at(m_line_up_id) || !line::Set::at(m_line_down_id) || !line::Set::at(m_line_left_id) || !line::Set::at(m_line_right_id)) return;

            m_transform_id = transform_id;
            m_rect = rect;

            line::Set::at(m_line_up_id)->set({ m_rect.x, m_rect.y },              { m_rect.x + m_rect.w, m_rect.y });
            line::Set::at(m_line_down_id)->set({ m_rect.x, m_rect.y + m_rect.h }, { m_rect.x + m_rect.w, m_rect.y + m_rect.h });
            line::Set::at(m_line_left_id)->set({ m_rect.x, m_rect.y },            { m_rect.x,            m_rect.y + m_rect.h });
            line::Set::at(m_line_right_id)->set({ m_rect.x + m_rect.w, m_rect.y },{ m_rect.x + m_rect.w, m_rect.y + m_rect.h });

            line::Set::at(m_line_up_id)->layer = line::Set::at(m_line_down_id)->layer = line::Set::at(m_line_left_id)->layer = line::Set::at(m_line_right_id)->layer = NUM_VISIBLE_LAYERS - 1;
            line::Set::at(m_line_up_id)->transform_id = line::Set::at(m_line_down_id)->transform_id = line::Set::at(m_line_left_id)->transform_id = line::Set::at(m_line_right_id)->transform_id = m_transform_id;
            line::Set::at(m_line_up_id)->is_debug = line::Set::at(m_line_down_id)->is_debug = line::Set::at(m_line_left_id)->is_debug = line::Set::at(m_line_right_id)->is_debug = true;
        }

        Object() = delete;
        Object(i32c transform_id, Rectfc rect) {
            m_transform_id = transform_id;
            m_rect = rect;

            m_line_up_id = line::Set::make({ m_rect.x, m_rect.y },              { m_rect.x + m_rect.w, m_rect.y });
            m_line_down_id = line::Set::make({ m_rect.x, m_rect.y + m_rect.h }, { m_rect.x + m_rect.w, m_rect.y + m_rect.h });
            m_line_left_id = line::Set::make({ m_rect.x, m_rect.y },            { m_rect.x,            m_rect.y + m_rect.h });
            m_line_right_id = line::Set::make({ m_rect.x + m_rect.w, m_rect.y },{ m_rect.x + m_rect.w, m_rect.y + m_rect.h });

            line::Set::at(m_line_up_id)->layer = line::Set::at(m_line_down_id)->layer = line::Set::at(m_line_left_id)->layer = line::Set::at(m_line_right_id)->layer = NUM_VISIBLE_LAYERS - 1;
            line::Set::at(m_line_up_id)->transform_id = line::Set::at(m_line_down_id)->transform_id = line::Set::at(m_line_left_id)->transform_id = line::Set::at(m_line_right_id)->transform_id = m_transform_id;
            line::Set::at(m_line_up_id)->is_debug = line::Set::at(m_line_down_id)->is_debug = line::Set::at(m_line_left_id)->is_debug = line::Set::at(m_line_right_id)->is_debug = true;
        }
        ~Object() {
            line::Set::erase(m_line_up_id);
            line::Set::erase(m_line_down_id);
            line::Set::erase(m_line_left_id);
            line::Set::erase(m_line_right_id);
        }

        Vec2fc point_0() { return transform::Set::at(m_transform_id) ? transform::Set::at(m_transform_id)->position + Vec2f{ m_rect.x,  m_rect.y } : Vec2f{ m_rect.x, m_rect.y }; }
        Vec2fc point_1() { return transform::Set::at(m_transform_id) ? transform::Set::at(m_transform_id)->position + Vec2f{ m_rect.x + m_rect.w, m_rect.y } : Vec2f{ m_rect.x + m_rect.w, m_rect.y }; }
        Vec2fc point_2() { return transform::Set::at(m_transform_id) ? transform::Set::at(m_transform_id)->position + Vec2f{ m_rect.x,  m_rect.y + m_rect.h } : Vec2f{ m_rect.x, m_rect.y + m_rect.h }; }
        Vec2fc point_3() { return transform::Set::at(m_transform_id) ? transform::Set::at(m_transform_id)->position + Vec2f{ m_rect.x + m_rect.w, m_rect.y + m_rect.h } : Vec2f{ m_rect.x + m_rect.w, m_rect.y + m_rect.h }; }


        /* Vec2fc point_0() { return Vec2f{ m_rect.x, m_rect.y }; }
         Vec2fc point_1() { return Vec2f{ m_rect.x + m_rect.w, m_rect.y }; }
         Vec2fc point_2() { return Vec2f{ m_rect.x, m_rect.y + m_rect.h }; }
         Vec2fc point_3() { return Vec2f{ m_rect.x + m_rect.w, m_rect.y + m_rect.h }; }*/

        std::vector<Vec2f> points() override {
            return { point_0(), point_1(), point_2(), point_3() };
        }

        void color(Color c) override {
            m_color = c;
            line::Set::at(m_line_up_id)->color = c;
            line::Set::at(m_line_down_id)->color = c;
            line::Set::at(m_line_left_id)->color = c;
            line::Set::at(m_line_right_id)->color = c;
        }
        Color color() { return m_color; }
        Color start_color() override {
            return m_start_color;
        }
        void start_color(Color c) {
            m_start_color = c;
        }

        void update() {
            /*if (m_time_left_colliding > 0.0f) {
                m_time_left_colliding -= 1.0f;
                color({ 255,0,0 });
            } else {
                color(start_color());
            }*/

            /*if (overlap != 0.0f) {
                color = { 255,0,0 };
                Console::log("overlap: ", overlap.x, " ", overlap.y, "\n");
                overlap = 0.0f;
            }
            else {
                color = start_color;
            }*/
            /*line::Set::at(m_line_up_id)->color = m_color;
            line::Set::at(m_line_down_id)->color = m_color;
            line::Set::at(m_line_left_id)->color = m_color;
            line::Set::at(m_line_right_id)->color = m_color;*/

            //Console::log("color: ", (int)color.r, "\n");

            /*for (auto& i : quad_node_ids) {
                Console::log("id: ", i, "\n");
            }*/
        }
    };

    class Set {
        static inline std::vector<Object*> s_objects;
        static inline std::vector<i32>     s_unused_ids;
    public:
        static std::vector<Object*>& get_objects() { return s_objects; }

        static bool    is_valid(size_t i) { return (i < s_objects.size() && s_objects.at(i)) ? true : false; }
        static size_t  size() { return s_objects.size(); }
        static Object* at(size_t i) { return (i < s_objects.size()) ? s_objects.at(i) : nullptr; }

        static i32 make(i32c transform_id, Rectfc rect) {
            Object* object = new Object(transform_id, rect);
            if (!s_unused_ids.empty()) {
                object->id = s_unused_ids.back();
                s_unused_ids.pop_back();
            } else {
                object->id = s_objects.size();
                s_objects.push_back(nullptr);
            }
            if (object->id >= 0 && object->id < s_objects.size() && s_objects.at(object->id)) {
                delete s_objects.at(object->id);
            }
            s_objects.at(object->id) = object;
            return object->id;
        }
        static bool erase(size_t i) {
            if (i < s_objects.size() && s_objects.at(i)) {
                Console::log("aabb::Set::erase ", i, "\n");
                delete s_objects.at(i);
                s_objects.at(i) = nullptr;
                s_unused_ids.push_back(i);
                return true;
            }
            return false;
        }
        static void update() {
            for (auto& i : s_objects) {
                if (i) {
                    i->update();
                }
            }
        }
        static void clear() {
            for (size_t i = 0; i < s_objects.size(); ++i) {
                //Console::log("aabb::Set::clear erasing ", i, "\n");
                erase(i);
            }
            s_objects.clear();
            s_unused_ids.clear();
        }
    };
}
