module;
#include <vector>

export module sprites;

import console;
import sprite;
import types;
import window;

export class Sprites {
    static inline size_t               m_num_sprite_ids = 0;
    static inline std::vector<Sprite*> m_sprites;
    static inline std::vector<size_t>  m_unused_ids;
public:    
    static bool    is_valid(size_t id) { return (id >= 0 && id <= m_num_sprite_ids - 1) ? true : false; }
    static size_t  size()              { return m_sprites.size(); }
    static Sprite* at(size_t i)        { return (i >= 0 && i <= m_sprites.size() - 1) ? m_sprites.at(i) : nullptr; }
    static size_t  add(Sprite* sprite) {
        if (!sprite) return -1;
        Console::log("m_num_sprite_ids: ", m_num_sprite_ids, "\n");
        if (!m_unused_ids.empty()) {
            sprite->id = m_unused_ids.back();
            m_unused_ids.pop_back();
        } else {
            sprite->id = m_num_sprite_ids;
            ++m_num_sprite_ids;
            m_sprites.push_back(nullptr);
        }

        if (sprite->id >= 0 && sprite->id <= m_sprites.size() - 1 && m_sprites.at(sprite->id)) {
            Console::log("Sprites::add erase ", sprite->id, "\n");
            delete m_sprites.at(sprite->id);
            m_sprites.at(sprite->id) = nullptr;
        } else {
            m_sprites.at(sprite->id) = sprite;
        }
        m_sprites.at(sprite->id) = sprite;

        Console::log("Sprites::add sprites:");
        for (size_t i = 0; i < m_sprites.size(); ++i) {
            if (m_sprites.at(i)) {
                Console::log(" ", m_sprites.at(i)->id, " ");
            }
        }
        Console::log("\n");

        return sprite->id;
    }
    static void erase(size_t i) {        
        if (m_sprites.empty() || i < 0 || i > m_sprites.size() - 1 || !m_sprites.at(i)) {
            Console::log("Sprites::erase ", i, " can't do it\n");
            return;
        }
        Console::log("Sprites::erase ", i, "\n");
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
            Console::log("Sprites::clear erasing ", i, "\n");
            erase(i);
        }
        m_sprites.clear();
        m_unused_ids.clear();
        m_num_sprite_ids = 0;
    }
};