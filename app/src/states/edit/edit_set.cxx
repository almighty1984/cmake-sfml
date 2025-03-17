module;
#include <sstream>

module state.edit;

import console;
import transforms;
import sprites;
import types;

namespace state {
    void Edit::select_on_set(Vec2f position) {
        Console::log("state::Edit::select_on_set mouse_sprite_ids size: ", m_mouse_sprite_ids.size(), "\n");
        if (!Inputs::at(m_input_id)->is_pressed(input::Key::ctrl)) {
            clear_selection_on_set();
        }

        for (auto& mouse_id : m_mouse_sprite_ids) {
            if (!Sprites::at(mouse_id)) continue;
            bool is_already_selected = false;
            for (auto& sel_id : m_selection_on_set_sprite_ids) {
                if (!Sprites::at(sel_id)) continue;
                Vec2f pos = position + Sprites::at(mouse_id)->offset - Transforms::at(m_selection_on_set_transform_id)->position;
                if (Sprites::at(sel_id)->offset == pos) {
                    is_already_selected = true;
                }
            }
            if (!is_already_selected) {
                Console::log("state::Edit::select_on_set mouse ", mouse_id, " offset: ", Sprites::at(mouse_id)->offset.x, " ", Sprites::at(mouse_id)->offset.y, "\n");

                m_selection_on_set_sprite_ids.push_back(Sprites::make("res/tiles/editor_selection.png"));
                Sprites::at(m_selection_on_set_sprite_ids.back())->layer = SELECTION_ON_SET_LAYER;
                Sprites::at(m_selection_on_set_sprite_ids.back())->transform_id = m_selection_on_set_transform_id;
                Sprites::at(m_selection_on_set_sprite_ids.back())->offset = position + Sprites::at(mouse_id)->offset - Transforms::at(m_selection_on_set_transform_id)->position;
            }
        }
    }
    void Edit::clear_selection_on_set() {
        for (auto& i : m_selection_on_set_sprite_ids) {
            if (!Sprites::at(i)) continue;
            Sprites::erase(i);
        }
        m_selection_on_set_sprite_ids.clear();
    }
}