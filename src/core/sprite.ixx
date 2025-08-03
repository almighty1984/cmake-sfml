module;
#include <SFML/Graphics.hpp>
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
        u8 tile_set  = 0,
           layer     = 0;
        u16 source_y = 0, source_x = 0,
            y        = 0, x        = 0;
    };    
    struct Object {
        i32   id           = -1,
              transform_id = -1;

        u8    layer        = 0,
              tile_set     = 0;
        bool  is_debug     = false,
              is_hidden    = false,
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
            is_debug(false), is_hidden(false), is_leftward(false), is_upended(false),
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
            is_debug = other.is_debug;
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

        void set_origin(Vec2fc o) {
            origin = o;
            sf_sprite.setOrigin(sf::Vector2f(o.x, o.y));
        }
        void set_rotation(f32 degrees) {
            if (degrees > 360.0f) degrees -= 360.0f;
            rotation = degrees + start_rotation;
            if (rotation > 360.0f) rotation -= 360.0f;
            
            sf_sprite.setRotation(sf::degrees(rotation));
        }
        f32c radians() const {
            return sf::degrees(rotation).asRadians();            
        }
        void rotate(f32c degrees) {
            rotation += degrees;
            if (rotation > 360.0f) rotation -= 360.0f;
            else if (rotation < -360.0f) rotation += 360.0f;
            sf_sprite.setRotation(sf_sprite.getRotation() + sf::degrees(degrees));
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
            //sf_sprite.setOrigin({ origin.x * Config::scale(), origin.y * Config::scale() });
            sf_sprite.setOrigin({ origin.x, origin.y });


            //f32c flip_x = is_leftward ? source_rect.w * 1.0f : 0.0f;
            //f32c flip_y = is_upended  ? source_rect.h * 1.0f : 0.0f;

            
            sf_sprite.setScale({ (is_leftward ? -1.0f : 1.0f) * Config::scale(), (is_upended ? -1.0f : 1.0f) * Config::scale() });


            //if (position != prev_position) {
                //m_transformed_position = position + transform()->position;
                //prev_position = position;
                //center = { position.x + source_rect.w / 2.0f, position.y + source_rect.h / 2.0f };
                //Console::log("sprite::Object::update position: ", prev_position.x, " ", transform()->position.x, "  ", prev_position.y, " ", transform()->position.y, "\n");        
            sf_sprite.setPosition({ (transform::Set::at(transform_id)->position.x + offset.x + origin.x) * Config::scale(),
                                    (transform::Set::at(transform_id)->position.y + offset.y + origin.y) * Config::scale() });
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
        static std::vector<Object*>& objects() { return m_objects; }

        static inline std::vector<Object*> objects_in_layer(u8c layer) {
            std::vector<Object*> same_layer_objects;
            std::for_each(m_objects.cbegin(), m_objects.cend(),
                [&](Object* i) { if (i && i->id != -1 && i->layer == layer) same_layer_objects.push_back(i); }
            );
            return same_layer_objects;
        }

        static bool    is_valid(size_t i) { return (!m_objects.empty() && i < m_objects.size() && m_objects.at(i)) ? true : false; }
        static size_t  size() { return m_objects.size(); }
        static Object* at(size_t i) { return (!m_objects.empty() && i < m_objects.size()) ? m_objects.at(i) : nullptr; }
        static i32c    make(const std::filesystem::path& path) {
            Object* sprite = new Object(path);
            if (!m_unused_ids.empty()) {
                sprite->id = m_unused_ids.back();
                m_unused_ids.pop_back();
                //Console::log("sprite::make unused size: ", m_unused_ids.size(), "\n");
                if (!m_objects.empty() && sprite->id >= 0 && sprite->id < m_objects.size() && m_objects.at(sprite->id)) {
                    delete m_objects.at(sprite->id);
                    m_objects.at(sprite->id) = nullptr;
                }
            } else {                
                sprite->id = m_objects.size();
                //Console::log("sprite::Set::make id: ", sprite->id, "\n");
                m_objects.push_back(nullptr);
            }            
            m_objects.at(sprite->id) = sprite;
            return sprite->id;
        }
        static bool erase(size_t i) {
            if (!(i < m_objects.size()) || !m_objects.at(i)) {
                //Console::log("sprite::Set::erase ", i, " can't do it!\n");
                return false;
            }
            //Console::log("sprite::Set::erase ", i, "\n");
            delete m_objects.at(i);
            m_objects.at(i) = nullptr;
            m_unused_ids.push_back(i);

            //Console::log("sprite::Set::erase unused_ids: ");
            //for (auto& i : m_unused_ids) {
                //Console::log(i, " ");
            //}
            //Console::log("\n");
            return true;
        }        
        static void update() {
            for (auto& i : m_objects) {
                if (i) i->update();
            }
        }
        static void clear() {
            for (i32 i = 0; i < m_objects.size(); ++i) {
                erase(i);
            }
            m_objects.clear();
            m_unused_ids.clear();
        }
        static void save_level(const std::filesystem::path& path, std::vector<i32>& grid_sprite_ids) {
            Console::log("sprite::Set::save\n");
            std::vector<Object*> sprites_to_save;
            for (auto& i : grid_sprite_ids) {
                u16c   rows    = m_objects.at(i)->source_rect.h / 16;
                u16c   columns = m_objects.at(i)->source_rect.w / 16;
                Vec2fc offset  = m_objects.at(i)->offset;

                for (u8 layer = 0; layer < NUM_LEVEL_LAYERS; ++layer) {
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
            Console::log("sprites_to_save size: ", sprites_to_save.size(), "\n");

            u16c size = (u16)sprites_to_save.size();

            std::ofstream out_file(path, std::ios::out | std::ios::binary);
            out_file.write((i8*)&size, sizeof(u16));

            for (auto& i : sprites_to_save) {
                if (!i) continue;
                //Console::log((int)i->layer, " ", i->offset.y, " ", i->offset.x, " ");

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
        static std::vector<Data> load_level(const std::filesystem::path& path) {
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