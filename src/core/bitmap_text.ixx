module;
#include <sstream>
#include <filesystem>
#include <vector>

export module bitmap_text;

import console;
import sprite;
import transform;
import types;

export class BitmapText {
    bool   m_is_hidden = false;

    Vec2f               m_offset        = {  0.0f,  0.0f },
                        m_start_offset  = { -1.0f, -1.0f };
    std::string         m_text;
    std::vector<i32>    m_sprite_ids;
public:
    u8     layer     = 0,
           font_size = 8;
    
    i32    transform_id = -1;

    std::filesystem::path texture_path = "res/fonts/font_8_gray.png";

    ~BitmapText() {
        clear_text();
    }
    const std::string& get_text() const { return m_text; }
    void clear_text() {
        if (m_sprite_ids.empty()) return;

        //Console::log("BitmapText::clear_text ids: ");
        //for (auto& i : m_sprite_ids) {
            //Console::log(" ", i);
        //}
        //Console::log("\n");
        
        for (auto& i : m_sprite_ids) {
            sprite::Set::erase(i);
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
            sprite::Set::at(i)->texture(path);
        }
    }
    void set_is_hidden(bool is) {
        if (is == m_is_hidden) return;
        m_is_hidden = is;
        for (auto& i : m_sprite_ids) {
            if (sprite::Set::at(i)) sprite::Set::at(i)->is_hidden = is;
        }
    }
    Vec2fc get_offset() const { return m_offset; }
    void set_offset(Vec2fc offset) {
        if (offset == m_offset) return;
        for (auto& i : m_sprite_ids) {
            sprite::Set::at(i)->offset -= m_offset;
            sprite::Set::at(i)->offset += offset;
        }
        m_offset = offset;
    }
    std::vector<i32>& get_sprite_ids() { return m_sprite_ids; }

    void set_text(const std::string& text) {
        if (text == m_text || text.empty()) return;
        clear_text();
        m_text = text;
        f32 x = 0.0f, y = 0.0f;
        //Console::log("BitmapText::set_text ");
        for (size_t i = 0; i < m_text.size(); ++i) {
            if (m_text.at(i) == '\n') {
                //Console::log("BitmapText::set_set found new line\n");
                y += 16.0f, x = 0.0f;
                continue;
            }
            i32c sprite_id = sprite::Set::make(texture_path);
            //Console::log(sprite_id, " ");
            sprite::Set::at(sprite_id)->id = sprite_id;
            sprite::Set::at(sprite_id)->transform_id = transform_id;
            sprite::Set::at(sprite_id)->layer = layer;
            sprite::Set::at(sprite_id)->offset = m_offset + Vec2f{ x, y };

            Rectic texture_rect{ ((32 + m_text.at(i)) % 32) * font_size,
                                 ((m_text.at(i) - m_text.at(i) % 32 - 32) / 32) * font_size,
                                 font_size,
                                 font_size };
            
            sprite::Set::at(sprite_id)->texture_rect(texture_rect);
            sprite::Set::at(sprite_id)->is_hidden = m_is_hidden;

            m_sprite_ids.push_back(sprite_id);

            x += (f32)(font_size);
            //Console::log("BitmapText add sprite id: ", id, "\n");
        }
        /*Console::log("BitmapText::set_text ids: ");
        for (auto& i : m_sprite_ids) {
            Console::log(" ", i);
        }
        Console::log("\n");*/
    }
    template<typename T> requires std::is_convertible_v<T, std::string_view>
    void set_text(T t) {
        std::ostringstream oss{};
        oss << t;
        set_text(oss.str());
    }

    //void update() {
    //    if (!transform::Set::is_valid(transform_id)) return;

    //    /*for (auto& i : m_sprite_ids) {
    //        sprite::Set::at(i)->offset = position + sprite::Set::at(i)->start + transform::Set::at(transform_id)->position;            
    //    }     */   
    //}
};
