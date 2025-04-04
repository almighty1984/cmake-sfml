module;
#include <filesystem>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>

export module sprites;

import console;
import sprite;
import types;
import window;

export class Sprites {
    static inline std::vector<Sprite*> m_sprites;
    static inline std::vector<i32>  m_unused_ids;
    
public:
    static inline u8 num_layers = 10;
    static bool    is_valid(i32c i) { return (i >= 0 && i < m_sprites.size() && m_sprites.at(i)) ? true : false; }
    static size_t  size()           { return m_sprites.size(); }
    static Sprite* at(i32c i)       { return (i >= 0 && i < m_sprites.size()) ? m_sprites.at(i) : nullptr; }
    static size_t  make(const std::filesystem::path& path) {
        Sprite* sprite = new Sprite(path);
        if (!m_unused_ids.empty()) {
            sprite->id = m_unused_ids.back();
            m_unused_ids.pop_back();
        } else {
            sprite->id = m_sprites.size();
            m_sprites.push_back(nullptr);
        }
        if (sprite->id >= 0 && sprite->id < m_sprites.size() && m_sprites.at(sprite->id)) {            
            delete m_sprites.at(sprite->id);            
        }
        m_sprites.at(sprite->id) = sprite;
        return sprite->id;
    }
    static void erase(i32c i) {
        if (m_sprites.empty() || i < 0 || i > m_sprites.size() - 1 || !m_sprites.at(i)) {
            //Console::log("Sprites::erase ", i, " can't do it\n");
            return;
        }
        //Console::log("Sprites::erase ", i, "\n");
        delete m_sprites.at(i);
        m_sprites.at(i) = nullptr;
        m_unused_ids.push_back(i);
        //i = -1;

        /*Console::log("Sprites::erase size: ", m_sprites.size(), "\n");

        Console::log("Sprites:erase sprites:");
        for (auto& i : m_sprites) {
            Console::log(" ", i.first, " ");
        }
        Console::log("\n");*/
    }
    static void draw(u8c layer, Window& window) {
        for (auto& i : m_sprites) {
            if (!i || i->id == -1 || i->layer != layer) continue;
            if (i->transform()) {
                //i->position = i->transformTransforms::at(transform_id)->position;
                //i->position = i->transform()->position + i->start;
            }
            i->update();
            if (!i->is_hidden) {
                window.draw(i);
            }
        }
    }
    static void clear() {
        for (i32 i = 0; i < m_sprites.size(); ++i) {
            //Console::log("Sprites::clear erasing ", i, "\n");
            erase(i);
        }
        m_sprites.clear();
        m_unused_ids.clear();
    }
    static void save(const std::filesystem::path& path, std::vector<i32>& grid_sprite_ids) {
        Console::log("Sprites::save\n");
        std::vector<Sprite*> sprites_to_save;
        for (auto& i : grid_sprite_ids) {
            u16 rows    = m_sprites.at(i)->source_rect.h / 16;
            u16 columns = m_sprites.at(i)->source_rect.w / 16;
            Vec2fc offset = m_sprites.at(i)->offset;

            for (u8 layer = 0; layer < num_layers; ++layer) {
                for (u16 y = 0; y < rows; ++y) {
                    for (u16 x = 0; x < columns; ++x) {
                        for (auto& i : m_sprites) {
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
            u8c  layer    = i->layer;
            u16c source_y = (u16)i->source_rect.y, source_x = (u16)i->source_rect.x;
            u16c offset_y = (u16)i->offset.y,      offset_x = (u16)i->offset.x;

            out_file.write((i8*)&tile_set, sizeof(u8));
            out_file.write((i8*)&layer,    sizeof(u8));
            out_file.write((i8*)&source_y, sizeof(u16));
            out_file.write((i8*)&source_x, sizeof(u16));
            out_file.write((i8*)&offset_y, sizeof(u16));
            out_file.write((i8*)&offset_x, sizeof(u16));
        }
        out_file.close();
        
    }
    static std::vector<SpriteData> open(const std::filesystem::path& path) {
        std::ifstream in_file(path, std::ios::in | std::ios::binary);
        if (!in_file.is_open()) return {};

        u16 size = 0;
        in_file.read((i8*)&size, sizeof(u16));
        
        Console::log("Sprites::open size: ", size, "\n");
        
        std::vector<SpriteData> sprite_data_vec;
        for (u16 i = 0; i < size; ++i) {
            SpriteData sprite_data;
            in_file.read((i8*)&sprite_data.tile_set, sizeof(u8));
            in_file.read((i8*)&sprite_data.layer,    sizeof(u8));
            in_file.read((i8*)&sprite_data.source_y, sizeof(u16));
            in_file.read((i8*)&sprite_data.source_x, sizeof(u16));
            in_file.read((i8*)&sprite_data.offset_y, sizeof(u16));
            in_file.read((i8*)&sprite_data.offset_x, sizeof(u16));
            sprite_data_vec.push_back(sprite_data);
        }

        in_file.close();

        return sprite_data_vec;
    }
    static void erase_null(size_t transform_id) {
        if (m_sprites.empty()
            || m_sprites.size() < m_unused_ids.size()
            ) {
            return;
        }
        Console::log("Sprites::erase_null m_sprites.size before : ", m_sprites.size(), "\n");
        if (m_unused_ids.empty()) return;

        std::ranges::sort(m_unused_ids, std::ranges::greater());

        for (auto& i : m_unused_ids) {
            Console::log("unused id: ", i, "\n");
        }

        Console::log("Sprites::erase_null m_sprites.size before : ", m_sprites.size(), "\n");      

        std::set<i32> to_be_moved;
        for (i32 i = m_sprites.size() - 1; i > 0; --i) {
            if (to_be_moved.size() >= m_unused_ids.size()) {
                Console::log("found enough to move\n");
                break;
            }
            if (m_sprites.at(i) && m_sprites.at(i)->transform_id == transform_id && i > m_unused_ids.back()) {
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
            m_sprites.at(m_unused_ids.back()) = m_sprites.at(i);
            m_sprites.at(m_unused_ids.back())->id = m_unused_ids.back();        
            m_sprites.at(i) = nullptr;            
            m_unused_ids.pop_back();
            //m_sprites.erase(m_sprites.begin() + i);
            to_be_erased.push_back(i);
            if (m_unused_ids.empty())
                break;
        }

        std::ranges::sort(to_be_erased, std::ranges::less());

        for (i32 i = to_be_erased.size() - 1; i > 0; ++i) {
            Console::log("to be erased: ", i, "\n");
            m_sprites.erase(m_sprites.begin() + to_be_erased.at(i));
        }
        for (i32 i = 0; i < m_sprites.size(); ++i) {
            if (m_sprites.at(i))
                m_sprites.at(i)->id = i;
        }
        m_unused_ids.clear();

        m_unused_ids = to_be_erased;
        Console::log("sprites.size: ", m_sprites.size(), "\n");

        /*for (size_t i = m_sprites.size() - 1; i > 0; --i) {
            if (!m_sprites.at(i)) {
                Console::log("id: ", i, " was null\n");
                m_sprites.erase(m_sprites.begin() + i);
            }
        }*/

        /*for (size_t i = 0; i < m_sprites.size(); ++i) {
            if (m_sprites.at(i))
                m_sprites.at(i)->id = i;            
        }*/

        //Console::log("Sprites::erase_null size after erase: ", m_sprites.size(), "\n");


        return;
        
        Console::log("Sprites::erase_null size before erase: ", m_sprites.size(), "\n");
        for (size_t i = m_sprites.size() - 1; i > 0; --i) {
            if (!m_sprites.at(i)) {
                Console::log("Sprites::erase_null: ", i, " was null\n");
                m_sprites.erase(m_sprites.begin() + i);
            }
        }
        
        Console::log("Sprites::erase_null size after erase: ", m_sprites.size(), "\n");
        for (size_t i = m_sprites.size() - 1; i > 0; --i) {
            if (m_sprites.at(i) && m_sprites.at(i)->transform_id == transform_id) {
                //Console::log("Sprites::erase_null after id: ", i, "\n");
                m_sprites.at(i)->id = i;
            }
        }
        return;

        for (size_t i = 0; i < m_sprites.size(); ++i) {
            if (!m_sprites.at(i)) {
                //Console::log("first null: ", i, " erase to ", m_sprites.size() - 1, "\n");
                //m_sprites.resize(m_sprites.size() - (m_sprites.size() - i));
                Console::log("Sprites::erase_null erase: ", i, "\n");
                m_sprites.erase(m_sprites.begin() + i);
                
                if (i > m_sprites.size() - 1) break;

                //--i;
                
                /*--i;
                for (size_t j = i; j < m_sprites.size(); ++j) {
                    if (m_sprites.at(j))
                        m_sprites.at(j)->id = j;
                }    */            
                //return;
            } else {
                Console::log("something was here: ", i, " id: ", m_sprites.at(i)->id, "\n");
            }
        }
        for (size_t i = 0; i < m_sprites.size(); ++i) {
            if (m_sprites.at(i)) {
                Console::log("Sprites::erase_null after id: ", i, "\n");
                m_sprites.at(i)->id = i;
            }
        }
        Console::log("Sprites::erase_null m_sprites.size after: ", m_sprites.size(), "\n");
    }
};