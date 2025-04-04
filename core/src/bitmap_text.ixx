module;
#include <sstream>
#include <filesystem>
#include <vector>

export module bitmap_text;

import console;
import sprite;
import sprites;
import transforms;
import types;

export class BitmapText {
    bool                m_is_hidden;
    Vec2f               m_offset, m_start_offset;
    std::string         m_text;
    std::vector<size_t> m_sprite_ids;
public:
    u8     layer, font_size;
    bool   is_hidden;
    size_t transform_id;

    std::filesystem::path texture_path;

    BitmapText() : m_text(), m_sprite_ids(),
                   m_is_hidden(false),
                   m_offset(), m_start_offset(),
                   layer(0), font_size(8), transform_id(-1),
                   texture_path("res/fonts/font_8_gray.png") {
    }
    ~BitmapText() {
        clear_text();
    }
    const std::string& get_text() const { return m_text; }
    void clear_text() {
        /*Console::log("BitmapText::clear_text ids: ");
        for (auto& i : m_sprite_ids) {
            Console::log(" ", i);
        }
        Console::log("\n");*/
        
        for (auto& i : m_sprite_ids) {
            Sprites::erase(i);
        }
        m_sprite_ids.clear();        
        m_text.clear();
    }
    void texture(const std::filesystem::path path) {
        if (path == texture_path) {
            return;
        }
        texture_path = path;
        for (auto& i : m_sprite_ids) {
            Sprites::at(i)->texture(path);
        }
    }
    void set_is_hidden(const bool is) {
        if (is == m_is_hidden) return;
        m_is_hidden = is;
        for (auto& i : m_sprite_ids) {
            if (Sprites::at(i)) Sprites::at(i)->is_hidden = is;
        }
    }
    Vec2fc get_offset() const { return m_offset; }
    void set_offset(Vec2f offset) {
        if (offset == m_offset) return;
        for (auto& i : m_sprite_ids) {
            Sprites::at(i)->offset -= m_offset;
            Sprites::at(i)->offset += offset;
        }
        m_offset = offset;
    }
    void set_text(const std::string& text) {
        if (/*text == m_text || */text.empty()) return;
        clear_text();
        m_text = text;
        f32 x = 0.0f, y = 0.0f;

        for (size_t i = 0; i < m_text.size(); ++i) {
            if (m_text.at(i) == '\n') {
                y += 16.0f, x = 0.0f;
                continue;
            }
            const size_t sprite_id = Sprites::make(texture_path);
            Sprites::at(sprite_id)->transform_id = transform_id;
            Sprites::at(sprite_id)->layer = layer;
            Sprites::at(sprite_id)->id = sprite_id;
            Sprites::at(sprite_id)->offset = m_offset + Vec2f{ x, y };

            Rect<i32> texture_rect{ (32 + m_text.at(i)) % 32, (m_text.at(i) - m_text.at(i) % 32 - 32) / 32, 1, 1 };
            texture_rect *= font_size;
            Sprites::at(sprite_id)->texture_rect(texture_rect);

            Sprites::at(sprite_id)->is_hidden = m_is_hidden;

            m_sprite_ids.push_back(sprite_id);

            x += (f32)(font_size);
            //Console::log("BitmapText add sprite id: ", id, "\n");
        }
        //Console::log("BitmapText::set_text ids: ");
        /*for (auto& i : m_sprite_ids) {
            Console::log(" ", i);
        }*/
        //Console::log("\n");
    }    
    template<typename T> requires std::is_convertible_v<T, std::string_view>
    void set_text(T t) {
        std::ostringstream oss{};
        oss << t;
        set_text(oss.str());
    }

    //void update() {
    //    if (!Transforms::is_valid(transform_id)) return;

    //    /*for (auto& i : m_sprite_ids) {
    //        Sprites::at(i)->offset = position + Sprites::at(i)->start + Transforms::at(transform_id)->position;            
    //    }     */   
    //}
};
