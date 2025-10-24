module;
#include <sstream>
#include <filesystem>
#include <vector>

export module bitmap_text;

import console;
import sprite;
import transform;
import types;
import window;

export class BitmapText {
    bool   m_is_hidden = false;

    Vec2F               m_offset        = {  0.0F,  0.0F },
                        m_start_offset  = { -1.0F, -1.0F };
    std::string         m_text;
    std::vector<I32>    m_sprite_ids;
public:
    U8     layer     = 0,
           font_size = 8;
    
    I32    transform_id = -1;

    std::filesystem::path texture_path = "res/texture/font_8_gray.png";

    ~BitmapText() {
        clear_text();
    }
    const std::string& get_text() const { return m_text; }
    void clear_text() {
        //if (m_sprite_ids.empty()) return;

        ////Console::log("BitmapText::clear_text ids: ");
        ////for (auto& i : m_sprite_ids) {
        //    //Console::log(" ", i);
        ////}
        ////Console::log("\n");
        //
        for (auto& i : m_sprite_ids) {
            sprite::Manager::erase(i);
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
            sprite::Manager::get(i)->texture(path);
        }
    }
    void set_is_hidden(bool is) {
        if (is == m_is_hidden) return;
        m_is_hidden = is;
        for (auto& i : m_sprite_ids) {
            if (sprite::Manager::get(i)) sprite::Manager::get(i)->is_hidden = is;
        }
    }
    cVec2F get_offset() const { return m_offset; }
    void set_offset(cVec2F offset) {
        if (offset == m_offset) return;
        for (auto& i : m_sprite_ids) {
            sprite::Manager::get(i)->offset -= m_offset;
            sprite::Manager::get(i)->offset += offset;
        }
        m_offset = offset;
    }
    std::vector<I32>& get_sprite_ids() { return m_sprite_ids; }

    void set_text(const std::string& text) {
        if (text == m_text || text.empty()) return;
        clear_text();
        m_text = text;
        F32 x = 0.0F, y = 0.0F;
        //Console::log("BitmapText::set_text ");
        for (size_t i = 0; i < m_text.size(); ++i) {
            if (m_text.at(i) == '\n') {
                //Console::log("BitmapText::set_set found new line\n");
                y += 16.0F, x = 0.0F;
                continue;
            }
            cI32 sprite_id = sprite::Manager::make(texture_path);

            m_sprite_ids.emplace_back(sprite_id);
            //Console::log(sprite_id, " ");
            sprite::Manager::get(sprite_id)->id = sprite_id;
            sprite::Manager::get(sprite_id)->transform_id = transform_id;
            sprite::Manager::get(sprite_id)->layer = layer;
            sprite::Manager::get(sprite_id)->offset = m_offset + Vec2F{ x, y };

            cRectI texture_rect{ ((32 + m_text.at(i)) % 32) * font_size,
                                 ((m_text.at(i) - m_text.at(i) % 32 - 32) / 32) * font_size,
                                 font_size,
                                 font_size };
            
            sprite::Manager::get(sprite_id)->texture_rect(texture_rect);
            sprite::Manager::get(sprite_id)->is_hidden = m_is_hidden;

            x += (F32)(font_size);
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
    //    if (!transform::Manager::is_valid(transform_id)) return;

    //    /*for (auto& i : m_sprite_ids) {
    //        sprite::Manager::get(i)->offset = position + sprite::Manager::get(i)->start + transform::Manager::get(transform_id)->position;            
    //    }     */   
    //}

    void draw(std::unique_ptr<Window>& window) {
        for (auto& i : m_sprite_ids) {
            window->draw(sprite::Manager::get(i));
        }
    }
};
