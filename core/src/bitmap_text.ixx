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
    std::string m_text;    
    std::vector<size_t> m_sprite_ids;
public:
    u8 layer, font_size;
    bool is_hidden;

    size_t transform_id = -1;
    Vec2f position, start_position;

    std::filesystem::path texture_path;

    BitmapText() : m_text(), m_sprite_ids(),
                   layer(0), font_size(8), is_hidden(false), transform_id(-1), position(), start_position(),
                   texture_path("res/fonts/font_8_gray.png") {
    }
    ~BitmapText() {
        clear_text();
    }
    void clear_text() {
        Console::log("BitmapText::clear_text ids: ");
        for (auto& i : m_sprite_ids) {
            Console::log(" ", i);
        }
        Console::log("\n");
        
        for (auto& i : m_sprite_ids) {
            Sprites::erase(i);
        }
        m_sprite_ids.clear();
        
        m_text.clear();
    }
    void set_text(const std::string& text) {
        if (text == m_text || text.empty()) return;
        clear_text();
        m_text = text;
        u16 line_num = 0;
        
        for (size_t i = 0; i < m_text.size(); ++i) {
            const size_t sprite_id = Sprites::add(new Sprite(texture_path));
            Sprites::at(sprite_id)->transform_id = transform_id;
            Sprites::at(sprite_id)->id = sprite_id;
            Sprites::at(sprite_id)->offset = { (f32)(i * font_size), 0.0f };

            IntRect texture_rect{ (32 + m_text.at(i)) % 32, (m_text.at(i) - m_text.at(i) % 32 - 32) / 32, 1, 1 };
            texture_rect *= font_size;
            Sprites::at(sprite_id)->texture_rect(texture_rect);

            m_sprite_ids.push_back(sprite_id);
            //Console::log("BitmapText add sprite id: ", id, "\n");
        }
        Console::log("BitmapText::set_text ids: ");
        for (auto& i : m_sprite_ids) {
            Console::log(" ", i);
        }
        Console::log("\n");

    }
    
    template<typename T>
    void set_text(T t) {
        std::ostringstream oss{};
        oss << t;
        set_text(oss.str());
    }

    void update() {
        if (!Transforms::is_valid(transform_id)) return;

        /*for (auto& i : m_sprite_ids) {
            Sprites::at(i)->offset = position + Sprites::at(i)->start + Transforms::at(transform_id)->position;            
        }     */   
    }
};
