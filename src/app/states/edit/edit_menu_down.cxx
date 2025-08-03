module;
#include <filesystem>
#include <sstream>
#include <string>

module state.edit;

import console;
import input;
import transform;
import sprite;
import types;

namespace state {
    bool Edit::init_brush(u8 size) {
        if (size > 5) size = 1;

        if (m_mouse_sprite_ids.size() == size) return false;
        
        Vec2f start, end;
        switch (size) {
        case 1: start = {  0.0f,  0.0f }, end = { 16.0f, 16.0f }; break;
        case 2: start = {  0.0f,  0.0f }, end = { 32.0f, 32.0f }; break;
        case 3: start = {-16.0f,-16.0f }, end = { 32.0f, 32.0f }; break;
        case 4: start = {-16.0f,-16.0f }, end = { 48.0f, 48.0f }; break;
        case 5: start = {-32.0f,-32.0f }, end = { 48.0f, 48.0f }; break; }

        for (const auto& i : m_mouse_sprite_ids) {
            sprite::Set::erase(i);
        }
        m_mouse_sprite_ids.clear();

        Console::log("state::Edit::init_brush sprite_id: ");
        for (f32 x = start.x; x < end.x; x += 16.0f) {
            for (f32 y = start.y; y < end.y; y += 16.0f) {
                i32c sprite_id = sprite::Set::make(m_mouse_texture_path);
                sprite::Set::at(sprite_id)->id = sprite_id;
                sprite::Set::at(sprite_id)->transform_id = m_mouse_transform_id;
                sprite::Set::at(sprite_id)->layer = 13;
                sprite::Set::at(sprite_id)->offset.x = x;
                sprite::Set::at(sprite_id)->offset.y = y;
                m_mouse_sprite_ids.push_back(sprite_id);
                Console::log(sprite_id, " ");
            }
        }
        Console::log("\n");
        return true;
    }
    void Edit::handle_menu_down() {
        Vec2fc position = m_mouse_tile_position;
        input::Button button = input::Button::null;
        if (input::Set::at(m_input_id)->is_pressed(input::Button::left)) {
            button = input::Button::left;
        } else if (input::Set::at(m_input_id)->is_pressed(input::Button::right)) {
            button = input::Button::right;
        } else if (input::Set::at(m_input_id)->is_pressed(input::Button::middle)) {
            button = input::Button::middle;
        }

        if (button == input::Button::null) return;

        if (m_is_showing_tile_set) {
            if (m_tile_set == 255 &&
                position.x == transform::Set::at(m_menu_down_transform_id)->position.x + sprite::Set::at(m_save_sprite_id)->offset.x &&
                position.y == transform::Set::at(m_menu_down_transform_id)->position.y + sprite::Set::at(m_save_sprite_id)->offset.y) {
                if (button == input::Button::left) {
                    save_types_to_bin();
                } else if (button == input::Button::right) {
                    input::Set::at(m_input_id)->release(button);
                    if (!m_is_typing_text_bar) {
                        init_typing_text_bar();
                        m_is_typing_text_bar = true;
                    } else {
                        quit_typing_text_bar();
                        save_typed_text_bar();
                        m_is_typing_text_bar = false;
                    }
                } else if (button == input::Button::middle) {
                    load_types_from_bin();
                }
                return;
            }            
        } else if (!m_is_showing_tile_set) {
            if (position.x == transform::Set::at(m_menu_down_transform_id)->position.x + sprite::Set::at(m_save_sprite_id)->offset.x &&
                position.y == transform::Set::at(m_menu_down_transform_id)->position.y + sprite::Set::at(m_save_sprite_id)->offset.y) {
                
                input::Set::at(m_input_id)->release(button);

                if (button == input::Button::left) {                    
                    save_typed_text_bar();
                    Console::log("state::Edit::handle_menu_down save\n");                    
                } else if (button == input::Button::right) {
                    if (!m_is_typing_text_bar) {
                        init_typing_text_bar();
                        m_is_typing_text_bar = true;
                    } else {
                        quit_typing_text_bar();
                    }
                }                
            }
        }
    }    
    bool Edit::remove_level(const std::filesystem::path& path) {
        try {
            if (std::filesystem::remove(path)) {
                Console::log("state::Edit::remove_level: ", path, " deleted\n");
            } else {
                Console::error("state::Edit::remove_level: ", path, " not found\n");
            }
        } catch (const std::filesystem::filesystem_error& err) {
            Console::error("state::Edit::remove_level filesystem error : ", err.what(), "\n");
            return false;
        }
        //load_menu_up_open_list(std::filesystem::current_path() / "res" / "level");
        return true;
    }   
}