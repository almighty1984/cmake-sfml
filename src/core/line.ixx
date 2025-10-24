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
    cF32 length(cVec2F v) {
        return std::sqrtf(v.x * v.x + v.y * v.y);
    }
    struct Object {
        I32 id           = -1,
            transform_id = -1;

        sf::Vertex sf_vertices[4] = {};
        U8 size  = 1,
           layer = 0;
        Color color       = { 127 },
              start_color = { 127 },
              prev_color  = { 0 };
        Vec2F offset   = { 0.0F, 0.0F},
              start    = { 0.0F, 0.0F},
              end      = { 0.0F, 0.0F},
              delta    = { 0.0F, 0.0F},
              velocity = { 0.0F, 0.0F};

        F32 length     = 0.0F,
            max_length = 0.0F,
            slope      = 0.0F;

        bool is_debug  = false,
             is_hidden = false;

        /*Object() : id(-1), transform_id(-1),
            size(1), layer(0),
            color({ 127, 127, 127 }), start_color({ 127, 127, 127 }), prev_color({ 0, 0, 0 }),
            offset({ 0.0F, 0.0F }), start({ 0.0F, 0.0F }), end({ 0.0F, 0.0F }), delta({ 0.0F, 0.0F }), velocity({ 0.0F, 0.0F }),
            length(0.0F), max_length(0.0F), slope(0.0F),
            is_debug(false), is_hidden(false) {
        }*/
        Object(cVec2F in_start, cVec2F in_end) {
            set(in_start, in_end);
        }
        cVec2F transformed_start() const {
            return start + (transform::Manager::get(transform_id) ? transform::Manager::get(transform_id)->position : Vec2F{ 0.0F, 0.0F });
        }
        cVec2F transformed_end() const {
            return end + (transform::Manager::get(transform_id) ? transform::Manager::get(transform_id)->position : Vec2F{ 0.0F, 0.0F });
        }
        void set(cVec2F in_start, cVec2F in_end) {
            cVec2F prev_start = start;
            cVec2F prev_end = end;
            start = in_start;
            end = in_end;

            /*if (transform::Manager::get(transform_id)) {
                start += transform::Manager::get(transform_id)->position;
                end += transform::Manager::get(transform_id)->position;
            }*/

            //cVec2F size_offset = { (delta.x / length) * size, -(delta.y / length) * size };
            //cVec2F extra_length = { (delta.x / length) * size * 0.25f, (delta.y / length) * size * 0.25f };
            cVec2F delta = end - start;

            if (prev_start != start && prev_end != end) {
                length = line::length(delta);
            }
            if (max_length > 0.0F && length > max_length) {
                cF32 over_max_length = length - max_length;
                end.x -= (over_max_length * delta.x / length);
                end.y -= (over_max_length * delta.y / length);
            }
            cVec2F perpendicular = { -delta.y / length * size / 2.0F, delta.x / length * size / 2.0F };

            sf_vertices[0].position.x = (transformed_start().x + perpendicular.x + offset.x) * Config::scale();
            sf_vertices[0].position.y = (transformed_start().y + perpendicular.y + offset.y) * Config::scale();

            sf_vertices[1].position.x = (transformed_end().x + perpendicular.x + offset.x) * Config::scale();
            sf_vertices[1].position.y = (transformed_end().y + perpendicular.y + offset.y) * Config::scale();

            sf_vertices[2].position.x = (transformed_end().x - perpendicular.x + offset.x) * Config::scale();
            sf_vertices[2].position.y = (transformed_end().y - perpendicular.y + offset.y) * Config::scale();

            sf_vertices[3].position.x = (transformed_start().x - perpendicular.x + offset.x) * Config::scale();
            sf_vertices[3].position.y = (transformed_start().y - perpendicular.y + offset.y) * Config::scale();
        }
        void update() {
            Vec2F prev_delta = delta;
            delta = end - start;

            if (delta != prev_delta) {
                length = line::length(delta);
                slope = delta.y / delta.x;
                //set(start, end);
                //Console::log("perpendicular: ", perpendicular.x, " ", perpendicular.y, "\n");
            }
            cVec2F perpendicular = { -delta.y / length * size / 2.0F, delta.x / length * size / 2.0F };

            sf_vertices[0].position.x = (transformed_start().x + perpendicular.x + offset.x) * Config::scale();
            sf_vertices[0].position.y = (transformed_start().y + perpendicular.y + offset.y) * Config::scale();

            sf_vertices[1].position.x = (transformed_end().x + perpendicular.x + offset.x) * Config::scale();
            sf_vertices[1].position.y = (transformed_end().y + perpendicular.y + offset.y) * Config::scale();

            sf_vertices[2].position.x = (transformed_end().x - perpendicular.x + offset.x) * Config::scale();
            sf_vertices[2].position.y = (transformed_end().y - perpendicular.y + offset.y) * Config::scale();

            sf_vertices[3].position.x = (transformed_start().x - perpendicular.x + offset.x) * Config::scale();
            sf_vertices[3].position.y = (transformed_start().y - perpendicular.y + offset.y) * Config::scale();


            /*if (color == Color{255, 0, 0}) {
                Console::log("hello\n");
            }*/

            //if (color != prev_color) {
                //prev_color = color;
            for (U8 i = 0; i < 4; ++i) {
                sf_vertices[i].color = { color.r, color.g, color.b };
            }
            //}
        }
    };


    export class Manager {
        static inline std::vector<Object*>  s_objects;
        static inline std::vector<I32>      s_unused_ids;
    public:
        static inline std::vector<Object*> objects_in_layer(cU8 layer) {
            std::vector<Object*> same_layer_objects;
            std::for_each(s_objects.cbegin(), s_objects.cend(), [&](Object* i) {
                if (i && i->id != -1 && i->layer == layer) {
                    same_layer_objects.emplace_back(i);
                }
                }
            );
            return same_layer_objects;
        }
        static inline std::vector<I32> object_ids_in_layer(cU8 layer) {
            std::vector<I32> same_layer_object_ids;
            std::for_each(s_objects.cbegin(), s_objects.cend(), [&](Object* i) {
                if (i && i->id != -1 && i->layer == layer) {
                    same_layer_object_ids.emplace_back(i->id);
                }
                }
            );
            return same_layer_object_ids;
        }

        static bool    is_valid(size_t i) { return (i < s_objects.size() && s_objects.at(i)) ? true : false; }
        static size_t  size() { return s_objects.size(); }
        static Object* get(cI32 i) { return (i >= 0 && !s_objects.empty() && i < s_objects.size()) ? s_objects.at(i) : nullptr; }

        static size_t make(cVec2F start, cVec2F end) {
            Object* object = new Object(start, end);
            if (!s_unused_ids.empty()) {
                object->id = s_unused_ids.back();
                s_unused_ids.pop_back();
                if (!s_objects.empty() && object->id >= 0 && object->id < s_objects.size() && s_objects.at(object->id)) {
                    delete s_objects.at(object->id);
                }
            } else {
                object->id = s_objects.size();
                s_objects.emplace_back(nullptr);
            }
            s_objects.at(object->id) = object;
            return object->id;
        }
        static bool erase(cI32 i) {
            if (i < 0 || s_objects.empty() || i > s_objects.size() - 1 || !s_objects.at(i)) {
                //Console::log("line::Manager::erase ", i, " can't do it\n");
                return false;
            }
            //Console::log("line::Manager::erase ", i, "\n");
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
                //Console::log("line::Manager::clear erasing ", i, "\n");
                erase(i);
            }
            s_objects.clear();
            s_unused_ids.clear();
        }
    };
}