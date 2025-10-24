module;
#include <memory>
export module health;
import console;
import line;
import types;
import window;

export namespace health {
    class Object {
        friend class Manager;

        I32 m_id = -1;

        I32 m_bar_line_id     = -1,
            m_bar_hit_line_id = -1,
            m_bar_bg_line_id  = -1;
    public:

        U8 layer = 0;

        F32 amount = 16.0F,
            max    = 16.0F,
            regen  =  0.1F;

        Vec2F offset = { 0.0F, 0.0F };

        Object() = delete;
        Object(cI32 transform_id) {
            m_bar_bg_line_id = line::Manager::make({ 0.0F, 0.0F }, { max, 0.0F });
            line::Manager::get(m_bar_bg_line_id)->size = 2;
            line::Manager::get(m_bar_bg_line_id)->layer = layer;
            line::Manager::get(m_bar_bg_line_id)->transform_id = transform_id;

            m_bar_hit_line_id = line::Manager::make({ 0.0F, 0.0F }, { max, 0.0F });
            line::Manager::get(m_bar_hit_line_id)->transform_id = transform_id;
            line::Manager::get(m_bar_hit_line_id)->size = 2;
            line::Manager::get(m_bar_hit_line_id)->layer = layer;
            line::Manager::get(m_bar_hit_line_id)->color = { 191, 191, 191 };

            m_bar_line_id = line::Manager::make({ 0.0F, 0.0F }, { max, 0.0F });
            line::Manager::get(m_bar_line_id)->transform_id = transform_id;
            line::Manager::get(m_bar_line_id)->size = 2;
            line::Manager::get(m_bar_line_id)->layer = layer;
            line::Manager::get(m_bar_line_id)->color = { 255, 0, 0 };
        }
        ~Object() {
            line::Manager::erase(m_bar_bg_line_id);
            line::Manager::erase(m_bar_hit_line_id);
            line::Manager::erase(m_bar_line_id);
        }       
        void reset() {
            amount = max;
        }
        void update() {
            if (!line::Manager::get(m_bar_line_id) || !line::Manager::get(m_bar_hit_line_id)) return;
            
            if      (amount < 0.0f) amount = 0.0f;
            else if (amount > max)  amount = max;

            line::Manager::get(m_bar_bg_line_id)->end.x = max;
            line::Manager::get(m_bar_line_id)->end.x    = amount;
            if (line::Manager::get(m_bar_line_id)->end.x < 0.0F) {
                line::Manager::get(m_bar_line_id)->end.x = 0.0F;
            }
            if (line::Manager::get(m_bar_hit_line_id)->end.x < 0.0F) {
                line::Manager::get(m_bar_hit_line_id)->end.x = 0.0F;
            }
                        
            if (line::Manager::get(m_bar_hit_line_id)->end.x < line::Manager::get(m_bar_line_id)->end.x) {
                line::Manager::get(m_bar_hit_line_id)->end.x = line::Manager::get(m_bar_line_id)->end.x;
                //line::Manager::get(m_bar_hit_line_id)->is_hidden = true;
            } else if (line::Manager::get(m_bar_hit_line_id)->end.x > line::Manager::get(m_bar_line_id)->end.x) {
                cF32 diff_x = line::Manager::get(m_bar_hit_line_id)->end.x - line::Manager::get(m_bar_line_id)->end.x;
                line::Manager::get(m_bar_hit_line_id)->end.x -= (diff_x / 5.0f);
                //line::Manager::get(m_bar_hit_line_id)->is_hidden = false;
            }            

            line::Manager::get(m_bar_bg_line_id)->offset  = offset;
            line::Manager::get(m_bar_hit_line_id)->offset = offset;
            line::Manager::get(m_bar_line_id)->offset     = offset;

            line::Manager::get(m_bar_bg_line_id)->layer  = layer;
            line::Manager::get(m_bar_hit_line_id)->layer = layer;
            line::Manager::get(m_bar_line_id)->layer     = layer;

            if (amount > 0.0f && amount < max) {
                amount += regen;
            }
        }
        void draw(std::unique_ptr<Window>& window) {
            if (amount <= 0.0F || amount >= max) return;
            window->draw(line::Manager::get(m_bar_bg_line_id));
            window->draw(line::Manager::get(m_bar_hit_line_id));
            window->draw(line::Manager::get(m_bar_line_id));
        }
    };

    class Manager {
        static inline std::vector<Object*>  s_objects;
        static inline std::vector<I32>      s_unused_ids;

    public:
        static Object* get(cI32 i) { return (i >= 0 && i < s_objects.size()) ? s_objects.at(i) : nullptr; }
        static bool    is_valid(cI32 i) { return (i >= 0 && i < s_objects.size() && s_objects.at(i)) ? true : false; }
        static size_t  size() { return s_objects.size(); }

        static I32 make(cI32 transform_id) {
            Object* object = new Object(transform_id);
            if (!s_unused_ids.empty()) {
                object->m_id = s_unused_ids.back();
                s_unused_ids.pop_back();
            } else {
                object->m_id = s_objects.size();
                s_objects.emplace_back(nullptr);
            }
            if (object->m_id >= 0 && object->m_id < s_objects.size() && s_objects.at(object->m_id)) {
                delete s_objects.at(object->m_id);
            }
            s_objects.at(object->m_id) = object;
            return object->m_id;
        }
        static bool erase(cI32 i) {            
            if (i < 0 || s_objects.empty() || i > s_objects.size() - 1 || !s_objects.at(i)) {
                //Console::log("transform::Manager::erase ", i, " can't do it\n");
                return false;
            }
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
        static void draw(std::unique_ptr<Window>& window, cU8 layer) {
            for (auto& i : s_objects) {
                if (i && i->layer == layer) {
                    i->draw(window);
                }
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
