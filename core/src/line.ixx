module;
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <vector>

export module line;
import config;
import console;
import transform;
import types;

export namespace line {
    struct Object {
        i32 id = -1;
        i32 transform_id = -1;

        sf::Vertex sf_vertices[4] = {};
        u8 size = 1, layer = 0;
        Color color{ 127 };
        Color start_color{ 127 };
        Color prev_color{ 0 };
        Vec2f offset;
        Vec2f start, end;
        Vec2f delta;
        Vec2f velocity;

        f32 length = 0.0f, max_length = 0.0f;
        f32 slope = 0.0f;

        bool is_debug = false;

        Object(Vec2fc in_start, Vec2fc in_end) {
            set(in_start, in_end);
        }
        Vec2fc transformed_start() const {
            return start + (transform::Set::at(transform_id) ? transform::Set::at(transform_id)->position : Vec2f{ 0.0f, 0.0f });
        }
        Vec2fc transformed_end() const {
            return end + (transform::Set::at(transform_id) ? transform::Set::at(transform_id)->position : Vec2f{ 0.0f, 0.0f });
        }
        void set(Vec2fc in_start, Vec2fc in_end) {
            Vec2fc prev_start = start;
            Vec2fc prev_end = end;
            start = in_start;
            end = in_end;

            /*if (transform::Set::at(transform_id)) {
                start += transform::Set::at(transform_id)->position;
                end += transform::Set::at(transform_id)->position;
            }*/

            //Vec2fc size_offset = { (delta.x / length) * size, -(delta.y / length) * size };
            //Vec2fc extra_length = { (delta.x / length) * size * 0.25f, (delta.y / length) * size * 0.25f };
            Vec2fc delta = end - start;

            if (prev_start != start && prev_end != end) {
                //std::cout << "shit\n";
                length = std::sqrtf(delta.x * delta.x + delta.y * delta.y);
            }
            if (max_length > 0.0f && length > max_length) {
                f32c over_max_length = length - max_length;
                end.x -= (over_max_length * delta.x / length);
                end.y -= (over_max_length * delta.y / length);
            }
            Vec2f perpendicular = { -delta.y / length * size / 2.0f, delta.x / length * size / 2.0f };

            sf_vertices[0].position.x = (transformed_start().x + perpendicular.x) * Config::scale();
            sf_vertices[0].position.y = (transformed_start().y + perpendicular.y) * Config::scale();

            sf_vertices[1].position.x = (transformed_end().x + perpendicular.x) * Config::scale();
            sf_vertices[1].position.y = (transformed_end().y + perpendicular.y) * Config::scale();

            sf_vertices[2].position.x = (transformed_end().x - perpendicular.x) * Config::scale();
            sf_vertices[2].position.y = (transformed_end().y - perpendicular.y) * Config::scale();

            sf_vertices[3].position.x = (transformed_start().x - perpendicular.x) * Config::scale();
            sf_vertices[3].position.y = (transformed_start().y - perpendicular.y) * Config::scale();
        }
        void update() {
            Vec2f prev_delta = delta;
            delta = end - start;               
            
            Vec2f perpendicular = { -delta.y / length * size / 2.0f, delta.x / length * size / 2.0f };

            sf_vertices[0].position.x = (transformed_start().x + perpendicular.x) * Config::scale();
            sf_vertices[0].position.y = (transformed_start().y + perpendicular.y) * Config::scale();

            sf_vertices[1].position.x = (transformed_end().x + perpendicular.x) * Config::scale();
            sf_vertices[1].position.y = (transformed_end().y + perpendicular.y) * Config::scale();

            sf_vertices[2].position.x = (transformed_end().x - perpendicular.x) * Config::scale();
            sf_vertices[2].position.y = (transformed_end().y - perpendicular.y) * Config::scale();

            sf_vertices[3].position.x = (transformed_start().x - perpendicular.x) * Config::scale();
            sf_vertices[3].position.y = (transformed_start().y - perpendicular.y) * Config::scale();

            if (delta != prev_delta) {
                length = std::sqrtf(delta.x * delta.x + delta.y * delta.y);
                slope = delta.y / delta.x;
                //set(start, end);                
            }

            /*if (color == Color{255, 0, 0}) {
                Console::log("hello\n");
            }*/

            //if (color != prev_color) {
                //prev_color = color;
                for (u8 i = 0; i < 4; ++i) {
                    sf_vertices[i].color = { color.r, color.g, color.b };
                }
            //}
        }
    };


    export class Set {
        static inline std::vector<Object*>  m_objects;
        static inline std::vector<i32>      m_unused_ids;
    public:        
        static inline std::vector<Object*> get_objects(u8c layer) {
            std::vector<Object*> same_layer_objects;
            std::for_each(m_objects.cbegin(), m_objects.cend(),
                [&](Object* i) { if (i && i->id != -1 && i->layer == layer) same_layer_objects.push_back(i); }
            );
            return same_layer_objects;
        }

        static bool    is_valid(size_t i) { return (i < m_objects.size() && m_objects.at(i)) ? true : false; }
        static size_t  size() { return m_objects.size(); }
        static Object* at(size_t i) { return (i < m_objects.size()) ? m_objects.at(i) : nullptr; }

        static size_t make(Vec2fc start, Vec2fc end) {
            Object* line = new Object(start, end);
            if (!m_unused_ids.empty()) {
                line->id = m_unused_ids.back();
                m_unused_ids.pop_back();
            } else {
                line->id = m_objects.size();
                m_objects.push_back(nullptr);
            }
            if (line->id >= 0 && line->id < m_objects.size() && m_objects.at(line->id)) {
                delete m_objects.at(line->id);
            }
            m_objects.at(line->id) = line;
            return line->id;
        }
        static bool erase(i32c i) {
            if (i == -1 || !(i < m_objects.size()) || !m_objects.at(i)) {
                //Console::log("line::Set::erase ", i, " can't do it\n");
                return false;
            }
            //Console::log("line::Set::erase ", i, "\n");
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
                //Console::log("line::Set::clear erasing ", i, "\n");
                erase(i);
            }
            m_unused_ids.clear();
        }
    };
}