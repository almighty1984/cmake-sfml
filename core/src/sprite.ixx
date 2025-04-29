module;
#include "SFML/Graphics.hpp"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string_view>
#include <vector>
#include <set>

export module sprite;

import config;
import console;
import texture;
import transform;
import types;

export namespace sprite {    
    struct Data {
        u8 tile_set = 0;
        u8 layer = 0;
        u16 source_y = 0, source_x = 0;
        u16 y = 0, x = 0;
    };
    enum class Type {
        null,
        clip, clip_U, clip_D, clip_D_L, clip_D_R, clip_L, clip_R, clip_ledge, clip_ledge_L, clip_ledge_R,
        level_start,
        slope_L_1x1, slope_R_1x1, slope_L_2x1_0, slope_L_2x1_1, slope_R_2x1_0, slope_R_2x1_1,
        player
    };
    using Typec = const Type;
    Type type_from_string(const std::string_view& s) {
        //Console::log("sprite::type_from_string: ", s, "\n");
        if      (s == "clip")          return Type::clip;
        else if (s == "clip_U")        return Type::clip_U;        
        else if (s == "clip_L")        return Type::clip_L;
        else if (s == "clip_R")        return Type::clip_R;
        else if (s == "clip_D")        return Type::clip_D;
        else if (s == "clip_D_L")      return Type::clip_D_L;
        else if (s == "clip_D_R")      return Type::clip_D_R;
        else if (s == "clip_ledge")    return Type::clip_ledge;
        else if (s == "clip_ledge_L")  return Type::clip_ledge_L;
        else if (s == "clip_ledge_R")  return Type::clip_ledge_R;
        else if (s == "level_start")   return Type::level_start;
        else if (s == "slope_L_1x1")   return Type::slope_L_1x1;
        else if (s == "slope_L_2x1_0") return Type::slope_L_2x1_0;
        else if (s == "slope_L_2x1_1") return Type::slope_L_2x1_1;
        else if (s == "slope_R_1x1")   return Type::slope_R_1x1;
        else if (s == "slope_R_2x1_0") return Type::slope_R_2x1_0;
        else if (s == "slope_R_2x1_1") return Type::slope_R_2x1_1;
        else if (s == "player")        return Type::player;
        return Type::null;
    }
    std::string_view type_to_string(const Type type) {
        switch (type) {
        case Type::clip:          return "clip";
        case Type::clip_U:        return "clip_U";        
        case Type::clip_L:        return "clip_L";        
        case Type::clip_R:        return "clip_R";
        case Type::clip_D:        return "clip_D";
        case Type::clip_D_L:      return "clip_D_L";
        case Type::clip_D_R:      return "clip_D_R";
        case Type::clip_ledge:    return "clip_ledge";
        case Type::clip_ledge_L:  return "clip_ledge_L";
        case Type::clip_ledge_R:  return "clip_ledge_R";
        case Type::level_start:   return "level_start";
        case Type::slope_L_1x1:   return "slope_L_1x1";
        case Type::slope_R_1x1:   return "slope_R_1x1";
        case Type::slope_L_2x1_0: return "slope_L_2x1_0";
        case Type::slope_L_2x1_1: return "slope_L_2x1_1";
        case Type::slope_R_2x1_0: return "slope_R_2x1_0";
        case Type::slope_R_2x1_1: return "slope_R_2x1_1";
        case Type::player:        return "player";
        default:                  return "";
        }
    }
    struct Info {
        Type type;
        Rectf rect;
        Vec2f velocity;
    };
    using Infoc = const Info;

    struct Object {
        Type  type         = Type::null;
        i32   id           = -1,
              transform_id = -1;

        u8    layer        = 0,
              tile_set     = 0;
        bool  is_hidden    = false,
              is_leftward  = false,
              is_upended   = false;
        Vec2f offset,
              prev_position;
        Vec2f start,
              level,
              origin,
              center;

        Color color, start_color;
        f32   rotation, start_rotation;

        std::filesystem::path texture_path;
        sf::Sprite sf_sprite;
        sf::Texture* sf_texture;
        Recti source_rect;
        Object() = delete;
        Object(std::filesystem::path path) : //m_transformed_position(),
            id(-1), transform_id(-1),
            layer(0), tile_set(0),
            is_hidden(false), is_leftward(false), is_upended(false),
            offset(), //prev_position(-1.0f, -1.0f),
            start(), level(), origin(), center(),
            color(), start_color(),
            rotation(0.0f), start_rotation(0.0f),
            texture_path(path),
            sf_sprite(*texture::Set::load(path)),
            sf_texture(texture::Set::load(path)),
            source_rect{ 0, 0, 16, 16 } {
        }
        ~Object() {
            //Console::log("~Object layer: ", layer, " id: ", id, "\n");
            id = -1;
            transform_id = -1;
        }
        Object& operator=(const Object& other) {
            transform_id = other.transform_id;
            layer = other.layer;
            tile_set = other.tile_set;
            is_hidden = other.is_hidden;
            is_leftward = other.is_leftward;
            is_upended = other.is_upended;
            offset = other.offset;
            start = other.start;
            level = other.level;
            origin = other.origin;
            center = other.center;
            color = other.color;
            start_color = other.start_color;
            rotation = other.rotation;
            start_rotation = other.start_rotation;
            texture_path = other.texture_path;
            source_rect = other.source_rect;
            return *this;
        }
        //void attach_transform(size_t id) { transform_id = id; }  void detach_transform() { transform_id = -1; }
        transform::Object* transform() { return transform::Set::is_valid(transform_id) ? transform::Set::at(transform_id) : nullptr; }

        Vec2uc texture_size() {
            return { sf_sprite.getTexture().getSize().x, sf_sprite.getTexture().getSize().y };
        }

        void set_rotation(f32 r) {
            if (r > 360.0f) r -= 360.0f;
            rotation = r + start_rotation;
            if (rotation > 360.0f) rotation -= 360.0f;
            sf_sprite.setRotation(sf::degrees(rotation));
        }
        void rotate(f32c r) {
            rotation += r;
            if (rotation > 360.0f) rotation -= 360.0f;
            sf_sprite.setRotation(sf_sprite.getRotation() + sf::degrees(r));
        }
        bool texture(const std::filesystem::path path) {
            texture_path = path;
            sf_texture = texture::Set::load(path);
            if (!sf_texture) return false;            
            sf_sprite.setTexture(*sf_texture, true);
            sf_sprite.setTextureRect(sf::IntRect({ (i32)source_rect.x, (i32)source_rect.y }, { (i32)source_rect.w, (i32)source_rect.h }));
            return true;
        }
        void texture_rect(Rectic rect) {
            source_rect = rect;
            sf_sprite.setTextureRect(sf::IntRect({ (i32)rect.x, (i32)rect.y }, { (i32)rect.w, (i32)rect.h }));
        }
        void update() {
            if (!transform::Set::is_valid(transform_id)) return;

            //position = transform::Set::at(transform_id)->position;

            texture_rect(source_rect);
            sf_sprite.setOrigin({ origin.x, origin.y });


            f32 flip_x = 0.0f;
            if (is_leftward) {
                flip_x = source_rect.w * 1.0f;
            }
            
            sf_sprite.setScale({ (is_leftward ? -1.0f : 1.0f) * Config::scale(), (f32)Config::scale() });

            //if (position != prev_position) {
                //m_transformed_position = position + transform()->position;
                //prev_position = position;
                //center = { position.x + source_rect.w / 2.0f, position.y + source_rect.h / 2.0f };
                //Console::log("sprite::Object::update position: ", prev_position.x, " ", transform()->position.x, "  ", prev_position.y, " ", transform()->position.y, "\n");        
            sf_sprite.setPosition({ (transform::Set::at(transform_id)->position.x + offset.x + flip_x) * Config::scale(),
                                    (transform::Set::at(transform_id)->position.y + offset.y) * Config::scale() });
            //}
        }
        /* Vec2f position() {
             return transform::Set::at(transform_id)->position + offset;
         }*/
    };


    class Set {
        static inline std::vector<Object*> m_objects;
        static inline std::vector<i32>     m_unused_ids;

    public:
        static inline u8 num_layers = 10;

        static std::vector<Object*>& get() { return m_objects; }

        static inline std::vector<Object*> get_objects(u8c layer) {
            std::vector<Object*> same_layer_objects;
            std::for_each(m_objects.cbegin(), m_objects.cend(),
                [&](Object* i) { if (i && i->id != -1 && i->layer == layer) same_layer_objects.push_back(i); }
            );
            return same_layer_objects;
        }

        static bool    is_valid(size_t i) { return (i >= 0 && i < m_objects.size() && m_objects.at(i)) ? true : false; }
        static size_t  size() { return m_objects.size(); }
        static Object* at(size_t i) { return (i >= 0 && i < m_objects.size()) ? m_objects.at(i) : nullptr; }
        static i32c    make(const std::filesystem::path& path) {
            Object* sprite = new Object(path);
            if (!m_unused_ids.empty()) {
                sprite->id = m_unused_ids.back();
                m_unused_ids.pop_back();
            } else {
                sprite->id = m_objects.size();
                m_objects.push_back(nullptr);
            }
            if (!m_objects.empty() && sprite->id >= 0 && sprite->id < m_objects.size() && m_objects.at(sprite->id)) {
                delete m_objects.at(sprite->id);
            }
            m_objects.at(sprite->id) = sprite;
            return sprite->id;
        }
        static bool erase(size_t i) {
            if (!(i < m_objects.size()) || !m_objects.at(i)) {
                Console::log("sprite::Set::erase ", i, " can't do it\n");
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
        static void save(const std::filesystem::path& path, std::vector<i32>& grid_sprite_ids) {
            Console::log("sprite::Set::save\n");
            std::vector<Object*> sprites_to_save;
            for (auto& i : grid_sprite_ids) {
                u16 rows = m_objects.at(i)->source_rect.h / 16;
                u16 columns = m_objects.at(i)->source_rect.w / 16;
                Vec2fc offset = m_objects.at(i)->offset;

                for (u8 layer = 0; layer < num_layers; ++layer) {
                    for (u16 y = 0; y < rows; ++y) {
                        for (u16 x = 0; x < columns; ++x) {                                                        
                            /*std::for_each(m_objects.cbegin(), m_objects.cend(),
                            [&](Object* i) { if (i && i->offset == offset + Vec2f{ x * 16.0f, y * 16.0f }) sprites_to_save.push_back(i); }
                            );*/
                            for (auto& i : m_objects) {
                                if (!i) continue;
                                if (i->layer == layer && i->offset == offset + Vec2f{ x * 16.0f, y * 16.0f }) {
                                    sprites_to_save.push_back(i);
                                }
                            }
                        }
                    }
                }
            }
            Console::log("sprites_to_save.size: ", sprites_to_save.size(), "\n");

            u16c size = (u16)sprites_to_save.size();

            std::ofstream out_file(path, std::ios::out | std::ios::binary);
            out_file.write((i8*)&size, sizeof(u16));

            for (auto& i : sprites_to_save) {
                if (!i) continue;
                Console::log((int)i->layer, " ", i->offset.y, " ", i->offset.x, " ");

                u8c  tile_set = i->tile_set;
                u8c  layer = i->layer;
                u16c source_y = (u16)i->source_rect.y,
                     source_x = (u16)i->source_rect.x;
                u16c y = (u16)i->offset.y,
                     x = (u16)i->offset.x;

                out_file.write((i8*)&tile_set, sizeof(u8));
                out_file.write((i8*)&layer, sizeof(u8));
                out_file.write((i8*)&source_y, sizeof(u16));
                out_file.write((i8*)&source_x, sizeof(u16));
                out_file.write((i8*)&y, sizeof(u16));
                out_file.write((i8*)&x, sizeof(u16));
            }
            out_file.close();
        }
        static std::vector<Data> open(const std::filesystem::path& path) {
            std::ifstream in_file(path, std::ios::in | std::ios::binary);
            if (!in_file.is_open()) return {};

            u16 size = 0;
            in_file.read((i8*)&size, sizeof(u16));

            Console::log("sprite::Set::open size: ", size, "\n");

            std::vector<Data> sprite_data_vec;
            for (u16 i = 0; i < size; ++i) {
                Data sprite_data;
                in_file.read((i8*)&sprite_data.tile_set, sizeof(u8));
                in_file.read((i8*)&sprite_data.layer, sizeof(u8));
                in_file.read((i8*)&sprite_data.source_y, sizeof(u16));
                in_file.read((i8*)&sprite_data.source_x, sizeof(u16));
                in_file.read((i8*)&sprite_data.y, sizeof(u16));
                in_file.read((i8*)&sprite_data.x, sizeof(u16));
                sprite_data_vec.push_back(sprite_data);
            }

            in_file.close();

            return sprite_data_vec;
        }
        static void erase_null(size_t transform_id) {
            if (m_objects.empty()
                || m_objects.size() < m_unused_ids.size()
                ) {
                return;
            }
            Console::log("sprite::Set::erase_null m_objects.size before : ", m_objects.size(), "\n");
            if (m_unused_ids.empty()) return;

            std::ranges::sort(m_unused_ids, std::ranges::greater());

            for (auto& i : m_unused_ids) {
                Console::log("unused id: ", i, "\n");
            }

            Console::log("sprite::Set::erase_null m_objects.size before : ", m_objects.size(), "\n");

            std::set<i32> to_be_moved;
            for (i32 i = m_objects.size() - 1; i > 0; --i) {
                if (to_be_moved.size() >= m_unused_ids.size()) {
                    Console::log("found enough to move\n");
                    break;
                }
                if (m_objects.at(i) && m_objects.at(i)->transform_id == transform_id && i > m_unused_ids.back()) {
                    to_be_moved.insert(i);
                }
            }
            /*if (to_be_moved.size() < m_unused_ids.size()) {
                Console::log("not enough to move\n");
                return;
            }*/

            std::vector<i32> to_be_erased;

            for (auto& i : to_be_moved) {
                Console::log("moving ", i, " to ", m_unused_ids.back(), "\n");
                m_objects.at(m_unused_ids.back()) = m_objects.at(i);
                m_objects.at(m_unused_ids.back())->id = m_unused_ids.back();
                m_objects.at(i) = nullptr;
                m_unused_ids.pop_back();
                //m_objects.erase(m_objects.begin() + i);
                to_be_erased.push_back(i);
                if (m_unused_ids.empty())
                    break;
            }

            std::ranges::sort(to_be_erased, std::ranges::less());

            for (i32 i = to_be_erased.size() - 1; i > 0; ++i) {
                Console::log("to be erased: ", i, "\n");
                m_objects.erase(m_objects.begin() + to_be_erased.at(i));
            }
            for (i32 i = 0; i < m_objects.size(); ++i) {
                if (m_objects.at(i))
                    m_objects.at(i)->id = i;
            }
            m_unused_ids.clear();

            m_unused_ids = to_be_erased;
            Console::log("sprites.size: ", m_objects.size(), "\n");

            /*for (size_t i = m_objects.size() - 1; i > 0; --i) {
                if (!m_objects.at(i)) {
                    Console::log("id: ", i, " was null\n");
                    m_objects.erase(m_objects.begin() + i);
                }
            }*/

            /*for (size_t i = 0; i < m_objects.size(); ++i) {
                if (m_objects.at(i))
                    m_objects.at(i)->id = i;
            }*/

            //Console::log("sprite::Set::erase_null size after erase: ", m_objects.size(), "\n");


            return;

            Console::log("sprite::Set::erase_null size before erase: ", m_objects.size(), "\n");
            for (size_t i = m_objects.size() - 1; i > 0; --i) {
                if (!m_objects.at(i)) {
                    Console::log("sprite::Set::erase_null: ", i, " was null\n");
                    m_objects.erase(m_objects.begin() + i);
                }
            }

            Console::log("sprite::Set::erase_null size after erase: ", m_objects.size(), "\n");
            for (size_t i = m_objects.size() - 1; i > 0; --i) {
                if (m_objects.at(i) && m_objects.at(i)->transform_id == transform_id) {
                    //Console::log("sprite::Set::erase_null after id: ", i, "\n");
                    m_objects.at(i)->id = i;
                }
            }
            return;

            for (size_t i = 0; i < m_objects.size(); ++i) {
                if (!m_objects.at(i)) {
                    //Console::log("first null: ", i, " erase to ", m_objects.size() - 1, "\n");
                    //m_objects.resize(m_objects.size() - (m_objects.size() - i));
                    Console::log("sprite::Set::erase_null erase: ", i, "\n");
                    m_objects.erase(m_objects.begin() + i);

                    if (i > m_objects.size() - 1) break;

                    //--i;

                    /*--i;
                    for (size_t j = i; j < m_objects.size(); ++j) {
                        if (m_objects.at(j))
                            m_objects.at(j)->id = j;
                    }    */
                    //return;
                } else {
                    Console::log("something was here: ", i, " id: ", m_objects.at(i)->id, "\n");
                }
            }
            for (size_t i = 0; i < m_objects.size(); ++i) {
                if (m_objects.at(i)) {
                    Console::log("sprite::Set::erase_null after id: ", i, "\n");
                    m_objects.at(i)->id = i;
                }
            }
            Console::log("sprite::Set::erase_null m_objects.size after: ", m_objects.size(), "\n");
        }
    };
}