module state.edit;

import console;
import input;
import sprite;
import types;

namespace state {
    void Edit::handle_menu_right() {
        //Console::log("handle_menu_right\n");
        Vec2fc position = m_mouse_tile_position;
        input::Button button = input::Button::null;
        if (input::Set::at(m_input_id)->is_pressed(input::Button::left)) {
            button = input::Button::left;
        } else if (input::Set::at(m_input_id)->is_pressed(input::Button::right)) {
            button = input::Button::right;
        }

        if (button == input::Button::null) return;

        if (!m_is_showing_tile_set) {
            if (position.y < 160 && position.x >= 288) {
                input::Set::at(m_input_id)->release(button);
                if (position.x == 288) {
                    sprite::Set::at(m_active_layer_sprite_id)->offset.y = position.y + 32.0f;
                    m_layer = (u8)(position.y / 16.0f);
                } else if (position.x == 304) {
                    u8 layer = (u8)(position.y / 16.0f);
                    if (layer < m_is_hidden_layer_sprite_ids.size() - 1 && m_is_hidden_layer_sprite_ids.at(layer)) {
                        sprite::Set::at(m_is_hidden_layer_sprite_ids.at(layer))->is_hidden = !sprite::Set::at(m_is_hidden_layer_sprite_ids.at(layer))->is_hidden;
                    }
                    if (sprite::Set::at(m_is_hidden_layer_sprite_ids.at(layer))->is_hidden) {
                        remove_visible_layer(layer);
                    } else {
                        add_visible_layer(layer);
                    }
                }
            }
        } else if (m_is_showing_tile_set) {
            Console::log("handle_menu_right\n");
            Vec2fc bg_sprite_pos = sprite::Set::at(m_current_tile_set_bg_sprite_id)->offset + transform::Set::at(m_menu_down_transform_id)->position;
            Rectic bg_sprite_rect = sprite::Set::at(m_current_tile_set_bg_sprite_id)->source_rect;

            if (position.x >= bg_sprite_pos.x && position.x <= bg_sprite_pos.x + bg_sprite_rect.w &&
                position.y >= bg_sprite_pos.y && position.y <= bg_sprite_pos.y + bg_sprite_rect.h) {
                Console::log("state::Edit::handle_menu_down position: ", position.x, " ", position.y, "\n");
                Console::log("state::Edit::handle_menu_down sprite pos: ", bg_sprite_pos.x, " ", bg_sprite_pos.y, "\n");

                input::Set::at(m_input_id)->release(button);
                switch (button) {
                case input::Button::left:  init_tile_set(m_tile_set + 1); break;
                case input::Button::right: init_tile_set(m_tile_set - 1); break;
                }                
            }

        }

    }
}