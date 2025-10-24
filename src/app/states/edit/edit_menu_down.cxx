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
    bool Edit::init_brush(U8 size) {
        if (size > 5) size = 1;

        if (m_mouse_sprite_ids.size() == size) return false;
        
        Vec2F start, end;
        switch (size) {
        case 1: start = {  0.0F,  0.0F }, end = { 16.0F, 16.0F }; break;
        case 2: start = {  0.0F,  0.0F }, end = { 32.0F, 32.0F }; break;
        case 3: start = {-16.0F,-16.0F }, end = { 32.0F, 32.0F }; break;
        case 4: start = {-16.0F,-16.0F }, end = { 48.0F, 48.0F }; break;
        case 5: start = {-32.0F,-32.0F }, end = { 48.0F, 48.0F }; break; }

        for (const auto& i : m_mouse_sprite_ids) {
            sprite::Manager::erase(i);
        }
        m_mouse_sprite_ids.clear();

        Console::log("state::Edit::init_brush sprite_id: ");
        for (F32 x = start.x; x < end.x; x += 16.0F) {
            for (F32 y = start.y; y < end.y; y += 16.0F) {
                cI32 sprite_id = sprite::Manager::make(m_mouse_texture_path);
                sprite::Manager::get(sprite_id)->id = sprite_id;
                sprite::Manager::get(sprite_id)->transform_id = m_mouse_transform_id;
                sprite::Manager::get(sprite_id)->layer = 13;
                sprite::Manager::get(sprite_id)->offset.x = x;
                sprite::Manager::get(sprite_id)->offset.y = y;
                m_mouse_sprite_ids.push_back(sprite_id);
                Console::log(sprite_id, " ");
            }
        }
        Console::log("\n");
        return true;
    }
    void Edit::handle_menu_down() {
        cVec2F position = m_mouse_tile_position;
        input::Button button = input::Button::none;
        if (input::Manager::get(m_input_id)->is_pressed(input::Button::left)) {
            button = input::Button::left;
        } else if (input::Manager::get(m_input_id)->is_pressed(input::Button::right)) {
            button = input::Button::right;
        } else if (input::Manager::get(m_input_id)->is_pressed(input::Button::middle)) {
            button = input::Button::middle;
        }

        if (button == input::Button::none) return;
        

        if (m_is_showing_tile_set) {
            if (m_tile_set == 255 &&
                position.x == transform::Manager::get(m_menu_down_transform_id)->position.x + sprite::Manager::get(m_save_sprite_id)->offset.x &&
                position.y == transform::Manager::get(m_menu_down_transform_id)->position.y + sprite::Manager::get(m_save_sprite_id)->offset.y) {
                if (button == input::Button::left) {
                    save_types_to_bin();
                } else if (button == input::Button::right) {
                    input::Manager::get(m_input_id)->release(button);
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
            if (view().w == 320 && m_mouse_tile_position.x == 304 && m_mouse_tile_position.y == 160 ||
                view().w == 480 && m_mouse_tile_position.x == 464 && m_mouse_tile_position.y == 240
                ) {
                input::Manager::get(m_input_id)->release(button);                    
                Console::log("position: ", m_mouse_tile_position.x, " ", m_mouse_tile_position.y, "  ", view().w, " ", view().h, "\n");

                if (button == input::Button::left) {
                    m_is_hidden_grid_map = !m_is_hidden_grid_map;
                    for (auto& i : m_grid_map_sprite_ids) {
                        if (sprite::Manager::get(i)) sprite::Manager::get(i)->is_hidden = m_is_hidden_grid_map;
                    }
                    sprite::Manager::get(m_position_on_grid_map_sprite_id)->is_hidden = m_is_hidden_grid_map;                    
                } else if (button == input::Button::right) {
                    m_is_hidden_grid = !m_is_hidden_grid;
                    for (auto& i : m_grid_sprite_ids) {
                        if (sprite::Manager::get(i)) sprite::Manager::get(i)->is_hidden = m_is_hidden_grid;                        
                    }                    
                }
                return;
            }
            

            if (position.x == transform::Manager::get(m_menu_down_transform_id)->position.x + sprite::Manager::get(m_save_sprite_id)->offset.x &&
                position.y == transform::Manager::get(m_menu_down_transform_id)->position.y + sprite::Manager::get(m_save_sprite_id)->offset.y) {
                
                input::Manager::get(m_input_id)->release(button);

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