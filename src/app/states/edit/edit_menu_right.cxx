module state.edit;

import console;
import input;
import sprite;
import types;

namespace state {
    void Edit::handle_menu_right() {
        //Console::log("handle_menu_right\n");
        cVec2F position = m_mouse_tile_position;
        input::Button button = input::Button::none;
        if (input::Manager::get(m_input_id)->is_pressed(input::Button::left)) {
            button = input::Button::left;
        } else if (input::Manager::get(m_input_id)->is_pressed(input::Button::right)) {
            button = input::Button::right;
        }

        if (button == input::Button::none) return;
        if (position.y > 144.0F) return;

        if (!m_is_showing_tile_set) {            
            input::Manager::get(m_input_id)->release(button);
            //Console::log("state::Edit::handle_menu_right ", position.x, " ", position.y, "\n");
            if (position.x == transform::Manager::get(m_menu_right_transform_id)->position.x) {
                sprite::Manager::get(m_active_layer_sprite_id)->offset.y = position.y;
                m_layer = (U8)(position.y / 16.0F);
            } else if (position.x == transform::Manager::get(m_menu_right_transform_id)->position.x + 16.0F) {
                U8 layer = (U8)(position.y / 16.0F);
                if (layer < m_is_hidden_layer_sprite_ids.size() && m_is_hidden_layer_sprite_ids.at(layer)) {
                    sprite::Manager::get(m_is_hidden_layer_sprite_ids.at(layer))->is_hidden = !sprite::Manager::get(m_is_hidden_layer_sprite_ids.at(layer))->is_hidden;
                }
                if (sprite::Manager::get(m_is_hidden_layer_sprite_ids.at(layer))->is_hidden) {
                    m_visible_layers.erase(layer);                    
                } else {
                    m_visible_layers.insert(layer);                    
                }
                //Console::log("state::Edit::handle_menu_right layer: ", (int)layer, "\n");
            }
        } else if (m_is_showing_tile_set) {
            Console::log("state::Edit::handle_menu_right position: ", position.x, " ", position.y, "\n");
            cVec2F bg_sprite_pos = transform::Manager::get(m_menu_right_transform_id)->position + 
                                   sprite::Manager::get(m_text_current_tile_set_bg_sprite_id)->offset;
            cRectI bg_sprite_rect = sprite::Manager::get(m_text_current_tile_set_bg_sprite_id)->source_rect;

            if (position.x >= bg_sprite_pos.x && position.x <= bg_sprite_pos.x + bg_sprite_rect.w &&
                position.y >= bg_sprite_pos.y && position.y <= bg_sprite_pos.y + bg_sprite_rect.h) {
                //Console::log("state::Edit::handle_menu_down position: ", position.x, " ", position.y, "\n");
                Console::log("state::Edit::handle_menu_down sprite pos: ", bg_sprite_pos.x, " ", bg_sprite_pos.y, "\n");

                input::Manager::get(m_input_id)->release(button);
                switch (button) {
                case input::Button::left:  init_tile_set(m_tile_set + 1); break;
                case input::Button::right: init_tile_set(m_tile_set - 1); break; }                
            }

        }

    }
}