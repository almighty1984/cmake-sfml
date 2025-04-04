module;
#include <sstream>

module state.edit;

import console;
import transforms;
import sprites;
import types;

namespace state {
    void Edit::init_tile_set(u8c layer) {
        const std::string tile_set_str = std::to_string((int)m_tile_set);
        m_current_tile_set.set_text(tile_set_str);
        Sprites::at(m_tile_set_sprite_id)->texture("res/textures/set_" + tile_set_str + ".png");

    }
    void Edit::enter_tile_set() {
        Transforms::at(m_tile_set_bg_transform_id)->position.y = 0.0f;
        Transforms::at(m_tile_set_transform_id)->position.y -= 512.0f;        
        Sprites::at(m_current_tile_set_bg_sprite_id)->is_hidden = true;
        Sprites::at(m_current_level_bg_sprite_id)->is_hidden = true;
        Sprites::at(m_current_tile_set_bg_sprite_id)->is_hidden = false;
        Sprites::at(m_save_sprite_id)->is_hidden = true;
        m_current_tile_set.set_is_hidden(false);
        m_current_level.set_is_hidden(true);

        for (auto& i : m_is_hidden_layer_sprite_ids) {
            Sprites::at(i)->is_hidden = true;
        }
        Sprites::at(m_active_layer_sprite_id)->is_hidden = true;
    }
    void Edit::exit_tile_set() {
        Transforms::at(m_tile_set_bg_transform_id)->position.y = 512.0f;
        Transforms::at(m_tile_set_transform_id)->position.y += 512.0f;        
        Sprites::at(m_current_tile_set_bg_sprite_id)->is_hidden = false;
        Sprites::at(m_current_level_bg_sprite_id)->is_hidden = false;
        Sprites::at(m_current_tile_set_bg_sprite_id)->is_hidden = true;
        Sprites::at(m_save_sprite_id)->is_hidden = false;
        m_current_tile_set.set_is_hidden(true);
        m_current_level.set_is_hidden(false);

        for (auto& i : m_is_hidden_layer_sprite_ids) {
            Sprites::at(i)->is_hidden = false;
        }
        Sprites::at(m_active_layer_sprite_id)->is_hidden = false;
    }
    bool Edit::move_tile_set(Vec2fc amount) {
        if (Transforms::at(m_tile_set_transform_id)->position.y + amount.y < -352.0f ||
            Transforms::at(m_tile_set_transform_id)->position.y + amount.y >    0.0f ||
            Transforms::at(m_tile_set_transform_id)->position.x + amount.x < -192.0f ||
            Transforms::at(m_tile_set_transform_id)->position.x + amount.x >    0.0f) {
            return false;
        }
        Transforms::at(m_tile_set_transform_id)->position += amount;
        Transforms::at(m_selection_on_tile_set_transform_id)->position += amount;
        return true;
    }
    bool Edit::select_on_tile_set(Vec2fc offset) {
        Console::log("state::Edit::select_on_tile_set mouse_sprite_ids size: ", m_mouse_sprite_ids.size(), "\n");
        for (auto& sel_id : m_selection_on_tile_set_sprite_ids) {
            if (!Sprites::at(sel_id)) continue;
            if (Sprites::at(sel_id)->offset == offset) {
                return false;
            }
        }
        m_selection_on_tile_set_sprite_ids.push_back(Sprites::make("res/textures/tile_selection.png"));
        Sprites::at(m_selection_on_tile_set_sprite_ids.back())->layer = SELECTION_ON_TILE_SET_LAYER;
        Sprites::at(m_selection_on_tile_set_sprite_ids.back())->transform_id = m_selection_on_tile_set_transform_id;
        Sprites::at(m_selection_on_tile_set_sprite_ids.back())->offset = offset;
        return true;
    }
    bool Edit::deselect_on_tile_set(Vec2fc offset) {
        for (auto& sel_id : m_selection_on_tile_set_sprite_ids) {
            if (!Sprites::at(sel_id) || Sprites::at(sel_id)->transform_id != m_selection_on_tile_set_transform_id) continue;
            if (Sprites::at(sel_id)->offset == offset) {
                Console::log("state::Edit::deselect_on_tile_set aready selected, deselecting\n");
                Sprites::erase(sel_id);
                std::vector<i32> resized_selection;
                for (auto& i : m_selection_on_tile_set_sprite_ids) {
                    if (i != sel_id) {
                        resized_selection.push_back(i);
                    }
                }
                m_selection_on_tile_set_sprite_ids.clear();
                m_selection_on_tile_set_sprite_ids = resized_selection;
                return true;
            }
        }
        return false;
    }
    bool Edit::move_selection_on_tile_set(Vec2fc amount) {
        if (m_selection_on_tile_set_sprite_ids.empty()) return false;
        for (auto& i : m_selection_on_tile_set_sprite_ids) {
            if (Sprites::at(i)->offset.x + amount.x < 0 || Sprites::at(i)->offset.x + amount.x > 464.0f ||
                Sprites::at(i)->offset.y + amount.y < 0 || Sprites::at(i)->offset.y + amount.y > 496.0f
                ) {
                return false;
            }
            if (amount.x < 0.0f && Transforms::at(m_tile_set_transform_id)->position.x + Sprites::at(i)->offset.x + amount.x < 0.0f   ||
                amount.x > 0.0f && Transforms::at(m_tile_set_transform_id)->position.x + Sprites::at(i)->offset.x + amount.x > 272.0f ||
                amount.y < 0.0f && Transforms::at(m_tile_set_transform_id)->position.y + Sprites::at(i)->offset.y + amount.y < 0.0f   ||
                amount.y > 0.0f && Transforms::at(m_tile_set_transform_id)->position.y + Sprites::at(i)->offset.y + amount.y > 144.0f) {
                move_tile_set(-amount);
            }
        }        
        for (auto& i : m_selection_on_tile_set_sprite_ids) {
            Sprites::at(i)->offset += amount;
        }        
        return true;
    }
    bool Edit::deselect_all_on_tile_set() {
        if (m_selection_on_tile_set_sprite_ids.empty()) return false;
        for (auto& i : m_selection_on_tile_set_sprite_ids) {
            if (!Sprites::at(i)) continue;
            Sprites::erase(i);
        }
        m_selection_on_tile_set_sprite_ids.clear();
        return true;
    }
}