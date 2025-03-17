module;
#include <filesystem>
#include <fstream>
#include <sstream>
#include <vector>

export module sprites;

import console;
import sprite;
import types;
import window;

export class Sprites {
    static inline std::vector<Sprite*> m_sprites;
    static inline std::vector<size_t>  m_unused_ids;
    
public:    
    static inline u8 num_layers = 10;
    static bool    is_valid(size_t i) { return (i >= 0 && i < m_sprites.size() && m_sprites.at(i)) ? true : false; }
    static size_t  size()             { return m_sprites.size(); }
    static Sprite* at(size_t i)       { return (i >= 0 && i < m_sprites.size()) ? m_sprites.at(i) : nullptr; }
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
    static void erase(size_t i) {
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
        for (size_t i = 0; i < m_sprites.size(); ++i) {
            //Console::log("Sprites::clear erasing ", i, "\n");
            erase(i);
        }
        m_sprites.clear();
        m_unused_ids.clear();
    }
    static void save(const std::filesystem::path& path, std::vector<size_t>& grid_sprite_ids) {
        Console::log("Sprites::save\n");
        std::vector<Sprite*> sprites_to_save;
        for (auto& i : grid_sprite_ids) {
            u16 rows    = m_sprites.at(i)->source_rect.h / 16;
            u16 columns = m_sprites.at(i)->source_rect.w / 16;
            Vec2f offset = m_sprites.at(i)->offset;

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
        
        std::vector<SpriteData> sprite_data;
        for (u16 i = 0; i < size; ++i) {
            SpriteData sd;
            in_file.read((i8*)&sd.tile_set, sizeof(u8));
            in_file.read((i8*)&sd.layer,    sizeof(u8));
            in_file.read((i8*)&sd.source_y, sizeof(u16));
            in_file.read((i8*)&sd.source_x, sizeof(u16));
            in_file.read((i8*)&sd.offset_y, sizeof(u16));
            in_file.read((i8*)&sd.offset_x, sizeof(u16));
            sprite_data.push_back(sd);
        }

        in_file.close();

        return sprite_data;
    }
    static void erase_null() {
        //Console::log("Sprites::erase_null size: ", m_sprites.size(), "\n");
        /*m_unused_ids.clear();

        auto is_null = [](const Sprite* s) { return s == nullptr; };
        m_sprites.erase(std::remove_if(std::begin(m_sprites), std::end(m_sprites), is_null), std::end(m_sprites));

        for (size_t i = 0; i < m_sprites.size(); ++i) {
            m_sprites.at(i)->id = i;
        }*/


        if (m_unused_ids.empty()) return;

        m_unused_ids.clear();

        std::vector<size_t> null_ids;
        //Console::log("Sprites null:         ");
        for (size_t i = 0; i < m_sprites.size(); ++i) {
            if (!m_sprites.at(i)) {
                //Console::log(i, ",");
                null_ids.push_back(i);
            }
        }
        //Console::log("\n");
        /*Console::log("Sprites unused:       ");
        for (size_t i = 0; i < m_unused_ids.size(); ++i) {
            Console::log(m_unused_ids.at(i), ",");
        }
        Console::log("\n");

        return;*/

        if (null_ids.empty()) return;

        m_unused_ids = null_ids;


        //if (m_unused_ids.empty()) return;

        //Console::log("Sprites not null:     ");
        std::vector<size_t> not_null_ids;
        for (size_t i = m_sprites.size() - 1; i > 0; --i) {
            if (not_null_ids.size() >= null_ids.size()) {
                break;
            }
            if (m_sprites.at(i)) {
                not_null_ids.push_back(i);
            }
        }
        //Console::log("\n");

        /*Console::log("Sprites not null:     ");
        for (size_t i = 0; i < not_null_ids.size(); ++i) {
            Console::log(not_null_ids.at(i), ",");
        }
        Console::log("\n");
        return;*/

        while (!null_ids.empty()) {
            m_sprites.at(null_ids.back()) = m_sprites.at(not_null_ids.back());
            m_sprites.at(null_ids.back())->id = null_ids.back();

            m_sprites.at(not_null_ids.back()) = nullptr;
            null_ids.pop_back();
            not_null_ids.pop_back();
        }

        //Console::log("Sprites null after: ");
        for (size_t i = 0; i < m_sprites.size(); ++i) {
            if (!m_sprites.at(i)) {
                //Console::log(i, ",");
            }
        }
        //m_sprites.resize(m_sprites.size() - (m_unused_ids.size() - 1));
        //m_unused_ids.clear();
        //Console::log("\n");

        return;

       
        
        /*for (size_t i = 0; i < not_null_ids.size(); ++i) {
            m_sprites.at(m_unused_ids.back()) = m_sprites.at(not_null_ids.at(i));
            m_sprites.at(m_unused_ids.back())->id = m_unused_ids.back();
            m_sprites.at(m_unused_ids.back())->offset = m_sprites.at(not_null_ids.at(i))->offset;

            m_sprites.at(not_null_ids.at(i)) = nullptr;

            m_unused_ids.pop_back();
            m_unused_ids.push_back(not_null.at(i));
        }*/


        /*Console::log("Sprites unused after: ");
        for (size_t i = 0; i < m_unused_ids.size(); ++i) {
            Console::log(m_unused_ids.at(i), ",");
        }
        Console::log("\n");*/


        /*for (size_t i = 0; i < not_null.size(); ++i) {
            m_sprites.erase(m_sprites.begin() + not_null.at(i));
        }*/

        /*for (size_t i = 0; i < m_unused_ids.size(); ++i) {
            m_sprites.erase(m_sprites.begin() + m_unused_ids.at(i));
        }
        m_unused_ids.clear();*/

        


        


        /*Console::log("unused size:", m_unused_ids.size(), "\n");

        Console::log("unused: ");
        for (size_t i = 0; i < m_unused_ids.size(); ++i) {
            Console::log(m_unused_ids.at(i), ",");
        }
        Console::log("\n");*/

        //Console::log("Sprites::erase_null size: ", m_sprites.size(), "\n");
    }
};