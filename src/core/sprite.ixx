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
        U8 tile_set, layer;
        U16 source_y, source_x, y, x;
        Data() : tile_set(0), layer(0), source_y(0), source_x(0), y(0), x(0) {}
    };
    struct Object {
        I32   id, transform_id;
        U8    layer, tile_set;
        bool  is_bg, is_debug, is_hidden, is_leftward, is_upended;
        Vec2F offset, prev_position, start, level, origin, center;
        Color color, start_color;
        F32   rotation, start_rotation;
        RectI source_rect;
        std::filesystem::path texture_path;
        sf::Sprite sf_sprite;
        sf::Texture* sf_texture;

        Object() = delete;
        Object(std::filesystem::path path) : //m_transformed_position(),
            id(-1), transform_id(-1),
            layer(0), tile_set(0),
            is_bg(false), is_debug(false), is_hidden(false), is_leftward(false), is_upended(false),
            offset(), prev_position(-1.0F, -1.0F),
            start(), level(), origin(), center(),
            color({ 127, 127, 127 }), start_color({ 127, 127, 127 }),
            rotation(0.0F), start_rotation(0.0F),
            texture_path(path),
            sf_sprite(*texture::Manager::load(path)),
            sf_texture(texture::Manager::load(path)),
            source_rect{ 0, 0, 16, 16 } {
        }
        ~Object() {
            //Console::log("~Object layer: ", layer, " id: ", id, "\n");
            id = -1;
            transform_id = -1;
        }
        Object& operator=(const Object& other) {
            transform_id   = other.transform_id;
            layer          = other.layer;
            tile_set       = other.tile_set;
            is_debug       = other.is_debug;
            is_hidden      = other.is_hidden;
            is_leftward    = other.is_leftward;
            is_upended     = other.is_upended;
            offset         = other.offset;
            start          = other.start;
            level          = other.level;
            origin         = other.origin;
            center         = other.center;
            color          = other.color;
            start_color    = other.start_color;
            rotation       = other.rotation;
            start_rotation = other.start_rotation;
            texture_path   = other.texture_path;
            source_rect    = other.source_rect;
            sf_sprite      = other.sf_sprite;
            sf_texture     = other.sf_texture;
            return *this;
        }
        //void attach_transform(size_t id) { transform_id = id; }  void detach_transform() { transform_id = -1; }
        transform::Object* transform() { return transform::Manager::is_valid(transform_id) ? transform::Manager::get(transform_id) : nullptr; }

        cVec2U texture_size() {
            return { sf_sprite.getTexture().getSize().x, sf_sprite.getTexture().getSize().y };
        }

        void set_origin(cVec2F o) {
            origin = o;
            sf_sprite.setOrigin(sf::Vector2f(o.x, o.y));
        }
        void set_rotation(F32 degrees) {
            if (degrees > 360.0F) degrees -= 360.0F;
            rotation = degrees + start_rotation;
            if (rotation > 360.0F) rotation -= 360.0F;
            
            sf_sprite.setRotation(sf::degrees(rotation));
        }
        cF32 radians() const {
            return sf::degrees(rotation).asRadians();            
        }
        void rotate(cF32 degrees) {
            rotation += degrees;
            if (rotation > 360.0F) rotation -= 360.0F;
            else if (rotation < -360.0F) rotation += 360.0F;
            sf_sprite.setRotation(sf_sprite.getRotation() + sf::degrees(degrees));
        }
        bool texture(const std::filesystem::path path) {
            if (texture_path == path) return false;
            texture_path = path;
            sf_texture = texture::Manager::load(path);
            if (!sf_texture) return false;            
            sf_sprite.setTexture(*sf_texture, true);
            sf_sprite.setTextureRect(sf::IntRect({ (I32)source_rect.x, (I32)source_rect.y }, { (I32)source_rect.w, (I32)source_rect.h }));
            return true;
        }
        void texture_rect(cRectI rect) {
            source_rect = rect;
            sf_sprite.setTextureRect(sf::IntRect({ (I32)rect.x, (I32)rect.y }, { (I32)rect.w, (I32)rect.h }));
        }
        void update() {
            if (!transform::Manager::is_valid(transform_id)) return;

            //position = transform::Manager::get(transform_id)->position;

            texture_rect(source_rect);
            //sf_sprite.setOrigin({ origin.x * Config::scale(), origin.y * Config::scale() });
            sf_sprite.setOrigin({ origin.x, origin.y });


            //cF32 flip_x = is_leftward ? source_rect.w * 1.0F : 0.0F;
            //cF32 flip_y = is_upended  ? source_rect.h * 1.0F : 0.0F;

            
            sf_sprite.setScale({ (is_leftward ? -1.0F : 1.0F) * Config::scale(), (is_upended ? -1.0F : 1.0F) * Config::scale() });


            //if (position != prev_position) {
                //m_transformed_position = position + transform()->position;
                //prev_position = position;
                //center = { position.x + source_rect.w / 2.0F, position.y + source_rect.h / 2.0F };
                //Console::log("sprite::Object::update position: ", prev_position.x, " ", transform()->position.x, "  ", prev_position.y, " ", transform()->position.y, "\n");        
            sf_sprite.setPosition({ (transform::Manager::get(transform_id)->position.x + offset.x + origin.x) * Config::scale(),
                                    (transform::Manager::get(transform_id)->position.y + offset.y + origin.y) * Config::scale() });
            //}
        }
        /* Vec2F position() {
             return transform::Manager::get(transform_id)->position + offset;
         }*/
    };


    class Manager {
        static inline std::vector<Object*> s_objects;
        static inline std::vector<I32>     s_unused_ids;

    public:
        static std::vector<Object*>& objects() { return s_objects; }

        /*static inline std::vector<Object*> objects_in_layer(cU8 layer) {
            std::vector<Object*> same_layer_objects;
            std::for_each(m_objects.cbegin(), m_objects.cend(), [&](Object* i) {
                                                                    if (i && i->id != -1 && i->layer == layer) {
                                                                        same_layer_objects.emplace_back(i);
                                                                    }
                                                                }
            );
            return same_layer_objects;
        }*/
        static inline std::vector<I32> object_ids_in_layer(cU8 layer) {
            std::vector<I32> same_layer_object_ids;
            std::for_each(s_objects.cbegin(), s_objects.cend(), [&](Object* i) {
                    if (i && i->id != -1 && !i->is_bg && i->layer == layer && !i->is_hidden) {
                        same_layer_object_ids.emplace_back(i->id);
                    }
                }
            );
            return same_layer_object_ids;
        }

        static inline std::vector<I32> bg_object_ids_in_layer(cU8 layer) {
            std::vector<I32> same_layer_bg_object_ids;
            std::for_each(s_objects.cbegin(), s_objects.cend(), [&](Object* i) {
                if (i && i->id != -1 && i->is_bg && i->layer == layer && !i->is_hidden) {
                    same_layer_bg_object_ids.emplace_back(i->id);
                }
                }
            );
            return same_layer_bg_object_ids;
        }

        static bool    is_valid(size_t i) { return (!s_objects.empty() && i < s_objects.size() && s_objects.at(i)) ? true : false; }
        static size_t  size()        { return s_objects.size();    }
        static size_t  unused_size() { return s_unused_ids.size(); }
        static Object* get(size_t i) { return (!s_objects.empty() && i < s_objects.size()) ? s_objects.at(i) : nullptr; }
        static cI32    make(const std::filesystem::path& path) {
            Object* object = new Object(path);
            if (!s_unused_ids.empty()) {
                object->id = s_unused_ids.back();
                s_unused_ids.pop_back();
                //Console::log("sprite::Manager::make unused: ", object->id, "\n");
                if (!s_objects.empty() && object->id >= 0 && object->id < s_objects.size() && s_objects.at(object->id)) {
                    delete s_objects.at(object->id);
                    //s_objects.at(object->id) = nullptr;
                }
                s_objects.at(object->id) = object;
            } else {
                object->id = s_objects.size();
                //Console::log("sprite::Manager::make id: ", object->id, "\n");
                s_objects.emplace_back(object);
            }
            return object->id;
        }
        static bool erase(cI32 i) {
            if (i >= 0 && !s_objects.empty() && i < s_objects.size() && s_objects.at(i)) {
                //Console::log("sprite::Manager::erase ", i, "\n");
                delete s_objects.at(i);
                s_objects.at(i) = nullptr;
                s_unused_ids.emplace_back(i);
                return true;
            }
            //Console::log("sprite::Manager::erase ", i, " can't do it! size: ", s_objects.size(), "\n");
            return false;
        }
        static void update() {
            for (auto& i : s_objects) {
                if (i) i->update();
            }
        }
        static void clear() {
            for (I32 i = 0; i < s_objects.size(); ++i) {
                erase(i);                
            }
            s_objects.clear();
            s_unused_ids.clear();
        }
        static void save_level(const std::filesystem::path& path, std::vector<I32>& grid_sprite_ids) {
            Console::log("sprite::Manager::save\n");
            std::vector<Object*> to_save;
            for (auto& i : grid_sprite_ids) {
                cU16   rows    = s_objects.at(i)->source_rect.h / 16;
                cU16   columns = s_objects.at(i)->source_rect.w / 16;
                cVec2F offset  = s_objects.at(i)->offset;

                for (U8 layer = 0; layer < NUM_LEVEL_LAYERS; ++layer) {
                    for (U16 y = 0; y < rows; ++y) {
                        for (U16 x = 0; x < columns; ++x) {                                                        
                            /*std::for_each(m_objects.cbegin(), m_objects.cend(),
                            [&](Object* i) { if (i && i->offset == offset + Vec2F{ x * 16.0F, y * 16.0F }) sprites_to_save.emplace_back(i); }
                            );*/
                            for (auto& i : s_objects) {
                                if (!i) continue;
                                if (i->layer == layer && i->offset == offset + Vec2F{ x * 16.0F, y * 16.0F }) {
                                    to_save.emplace_back(i);
                                }
                            }
                        }
                    }
                }
            }
            Console::log("sprite objects to save size: ", to_save.size(), "\n");

            cU16 size = (U16)to_save.size();

            std::ofstream out_file(path, std::ios::out | std::ios::binary);
            out_file.write((I8*)&size, sizeof(U16));

            for (auto& i : to_save) {
                if (!i) continue;
                //Console::log((int)i->layer, " ", i->offset.y, " ", i->offset.x, " ");

                cU8  tile_set = i->tile_set;
                cU8  layer = i->layer;
                cU16 source_y = (U16)i->source_rect.y,
                     source_x = (U16)i->source_rect.x;
                cU16 y = (U16)i->offset.y,
                     x = (U16)i->offset.x;

                out_file.write((I8*)&tile_set, sizeof(U8));
                out_file.write((I8*)&layer, sizeof(U8));
                out_file.write((I8*)&source_y, sizeof(U16));
                out_file.write((I8*)&source_x, sizeof(U16));
                out_file.write((I8*)&y, sizeof(U16));
                out_file.write((I8*)&x, sizeof(U16));
            }
            out_file.close();
        }
        static std::vector<Data> load_level(const std::filesystem::path& path) {
            std::ifstream in_file(path, std::ios::in | std::ios::binary);
            if (!in_file.is_open()) return {};

            U16 size = 0;
            in_file.read((I8*)&size, sizeof(U16));

            Console::log("sprite::Manager::open size: ", size, "\n");

            std::vector<Data> sprite_data_vec;
            for (U16 i = 0; i < size; ++i) {
                Data sprite_data{};
                in_file.read((I8*)&sprite_data.tile_set, sizeof(U8));
                in_file.read((I8*)&sprite_data.layer, sizeof(U8));
                in_file.read((I8*)&sprite_data.source_y, sizeof(U16));
                in_file.read((I8*)&sprite_data.source_x, sizeof(U16));
                in_file.read((I8*)&sprite_data.y, sizeof(U16));
                in_file.read((I8*)&sprite_data.x, sizeof(U16));
                sprite_data_vec.emplace_back(sprite_data);
            }

            in_file.close();

            return sprite_data_vec;
        }
        static void erase_null(size_t transform_id) {
            if (s_objects.empty()
                || s_objects.size() < s_unused_ids.size()
                ) {
                return;
            }
            Console::log("sprite::Manager::erase_null s_objects.size before : ", s_objects.size(), "\n");
            if (s_unused_ids.empty()) return;

            std::ranges::sort(s_unused_ids, std::ranges::greater());

            for (auto& i : s_unused_ids) {
                Console::log("unused id: ", i, "\n");
            }

            Console::log("sprite::Manager::erase_null m_objects.size before : ", s_objects.size(), "\n");

            std::set<I32> to_be_moved;
            for (I32 i = s_objects.size() - 1; i > 0; --i) {
                if (to_be_moved.size() >= s_unused_ids.size()) {
                    Console::log("found enough to move\n");
                    break;
                }
                if (s_objects.at(i) && s_objects.at(i)->transform_id == transform_id && i > s_unused_ids.back()) {
                    to_be_moved.insert(i);
                }
            }
            /*if (to_be_moved.size() < m_unused_ids.size()) {
                Console::log("not enough to move\n");
                return;
            }*/

            std::vector<I32> to_be_erased;

            for (auto& i : to_be_moved) {
                Console::log("moving ", i, " to ", s_unused_ids.back(), "\n");
                s_objects.at(s_unused_ids.back()) = s_objects.at(i);
                s_objects.at(s_unused_ids.back())->id = s_unused_ids.back();
                s_objects.at(i) = nullptr;
                s_unused_ids.pop_back();
                //m_objects.erase(m_objects.begin() + i);
                to_be_erased.emplace_back(i);
                if (s_unused_ids.empty())
                    break;
            }

            std::ranges::sort(to_be_erased, std::ranges::less());

            for (I32 i = to_be_erased.size() - 1; i > 0; ++i) {
                Console::log("to be erased: ", i, "\n");
                s_objects.erase(s_objects.begin() + to_be_erased.at(i));
            }
            for (I32 i = 0; i < s_objects.size(); ++i) {
                if (s_objects.at(i)) {
                    s_objects.at(i)->id = i;
                }
            }
            s_unused_ids.clear();

            s_unused_ids = to_be_erased;
            Console::log("sprites.size: ", s_objects.size(), "\n");

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

            //Console::log("sprite::Manager::erase_null size after erase: ", m_objects.size(), "\n");


            return;

            Console::log("sprite::Manager::erase_null size before erase: ", s_objects.size(), "\n");
            for (size_t i = s_objects.size() - 1; i > 0; --i) {
                if (!s_objects.at(i)) {
                    Console::log("sprite::Manager::erase_null: ", i, " was null\n");
                    s_objects.erase(s_objects.begin() + i);
                }
            }

            Console::log("sprite::Manager::erase_null size after erase: ", s_objects.size(), "\n");
            for (size_t i = s_objects.size() - 1; i > 0; --i) {
                if (s_objects.at(i) && s_objects.at(i)->transform_id == transform_id) {
                    //Console::log("sprite::Manager::erase_null after id: ", i, "\n");
                    s_objects.at(i)->id = i;
                }
            }
            return;

            for (size_t i = 0; i < s_objects.size(); ++i) {
                if (!s_objects.at(i)) {
                    //Console::log("first null: ", i, " erase to ", m_objects.size() - 1, "\n");
                    //m_objects.resize(m_objects.size() - (m_objects.size() - i));
                    Console::log("sprite::Manager::erase_null erase: ", i, "\n");
                    s_objects.erase(s_objects.begin() + i);

                    if (i > s_objects.size() - 1) break;

                    //--i;

                    /*--i;
                    for (size_t j = i; j < m_objects.size(); ++j) {
                        if (m_objects.at(j))
                            m_objects.at(j)->id = j;
                    }    */
                    //return;
                } else {
                    Console::log("something was here: ", i, " id: ", s_objects.at(i)->id, "\n");
                }
            }
            for (size_t i = 0; i < s_objects.size(); ++i) {
                if (s_objects.at(i)) {
                    Console::log("sprite::Manager::erase_null after id: ", i, "\n");
                    s_objects.at(i)->id = i;
                }
            }
            Console::log("sprite::Manager::erase_null m_objects.size after: ", s_objects.size(), "\n");
        }
    };
}