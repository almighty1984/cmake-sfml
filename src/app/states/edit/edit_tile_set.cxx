module;
#include <fstream>
#include <sstream>

module state.edit;

import console;
import transform;
import sprite;
import types;

namespace state {
    void Edit::load_types_from_text_file(const std::filesystem::path& path) {
        std::ifstream in_file(path);

        std::ostringstream oss;
        oss << in_file.rdbuf();
        in_file.close();

        std::string file_text = oss.str();

        const size_t start = file_text.find("set_255", 0);
        if (start == std::string::npos) {
            Console::error("state::Edit::load_types set_255 not found\n");
            return;
        }

        const size_t open_bracket = file_text.find("{", start);
        if (open_bracket == std::string::npos) {
            Console::error("state::Edit::load_types set_255 open bracket not found\n");
            return;
        }
        const size_t close_bracket = file_text.find("}", open_bracket);
        if (close_bracket == std::string::npos) {
            Console::error("state::Edit::load_types set_255 close bracket not found\n");
            return;
        }

        size_t label_start = file_text.find("\n", open_bracket);
        while (label_start < close_bracket) {
            while (file_text.at(label_start) == '	' || file_text.at(label_start) == ' ' || file_text.at(label_start) == '\n') {
                ++label_start;
            }
            const size_t label_end = file_text.find(" ",  label_start);
            const size_t equals    = file_text.find("=",  label_start);
            const size_t end_line  = file_text.find("\n", label_start);                        

            const std::string number_label = file_text.substr(label_start, label_end - label_start);

            cU16 number = std::stoi(number_label);

            size_t type_start = equals + 1;
            while (file_text.at(type_start) == '	' || file_text.at(type_start) == ' ' || file_text.at(type_start) == '\n') {
                ++type_start;
            }
            const std::string type = file_text.substr(type_start, end_line - type_start);

            m_types.emplace(entity::Info{ 255, number }, type);
            label_start = end_line + 1;
        }        
    }

    void Edit::init_tile_set(cU8 tile_set) {
        m_prev_tile_set = m_tile_set;
        m_tile_set = tile_set;
        const std::string tile_set_str = std::to_string((int)m_tile_set);
        m_text_current_tile_set.set_text(tile_set_str);
        sprite::Manager::get(m_tile_set_sprite_id)->texture("res/texture/set_" + tile_set_str + ".png");
        sprite::Manager::get(m_current_tile_sprite_id)->texture("res/texture/set_" + tile_set_str + ".png");

        if (!m_is_showing_tile_set) return;
        
        m_text_bar.clear_text();

        if (tile_set == 255) {
            if (!m_selection_on_tile_set_sprite_ids.empty() && 
                m_types.find(entity::Info{ 255, m_tile_number }) != m_types.end()) {
                m_text_bar.set_text(m_types.at(entity::Info{ 255, m_tile_number }));
            }
            sprite::Manager::get(m_save_sprite_id)->is_hidden = false;
        } else {
            //m_text_bar.clear_text();
            sprite::Manager::get(m_save_sprite_id)->is_hidden = true;
        }        
                
        Console::log("state::Edit::init_tile_set tile_number: ", m_tile_number, "\n");
        
        //deselect_all_on_tile_set();
    }
    void Edit::enter_tile_set() {
        Console::log("state::Edit::enter_tile_set\n");
        //transform::Manager::get(m_tile_set_bg_transform_id)->position.y = 0.0F;
        sprite::Manager::get(m_tile_set_bg_sprite_id)->is_hidden = false;
        sprite::Manager::get(m_tile_set_sprite_id)->is_hidden    = false;

        //transform::Manager::get(m_tile_set_transform_id)->position = m_tile_set_focus;

        //Console::log("tile set focus: ", m_tile_set_focus.x, " ", m_tile_set_focus.y, "\n");

        sprite::Manager::get(m_text_current_tile_set_bg_sprite_id)->is_hidden = true;
        
        for (auto& i : m_is_hidden_layer_sprite_ids) {
            sprite::Manager::get(i)->layer = 0;
        }
        sprite::Manager::get(m_active_layer_sprite_id)->is_hidden = true;
        sprite::Manager::get(m_position_on_grid_map_sprite_id)->is_hidden = true;

        for (auto& i : m_grid_sprite_ids) {
            if (sprite::Manager::get(i)) sprite::Manager::get(i)->is_hidden = true;
        }
        for (auto& i : m_grid_map_sprite_ids) {
            if (sprite::Manager::get(i)) sprite::Manager::get(i)->is_hidden = true;
        }
        sprite::Manager::get(m_position_on_grid_map_sprite_id)->is_hidden = true;
        sprite::Manager::get(m_grid_icon_sprite_id)->is_hidden = true;

        sprite::Manager::get(m_text_current_tile_set_bg_sprite_id)->is_hidden = false;        
        m_text_current_tile_set.set_is_hidden(false);
        if (m_tile_set == 255) {
            if (m_types.find(entity::Info{ 255, m_tile_number }) == m_types.end()) {
                m_text_bar.clear_text();
            } else {
                m_text_bar.set_text(m_types.at(entity::Info{ 255, m_tile_number }));
            }
            sprite::Manager::get(m_save_sprite_id)->is_hidden = false;
        } else {
            m_text_bar.clear_text();
            sprite::Manager::get(m_save_sprite_id)->is_hidden = true;
        }
        //sprite::Manager::get(m_tile_set_sprite_id)->is_hidden = false;

        for (auto& i : m_selection_on_tile_set_sprite_ids) {
            if (sprite::Manager::get(i)) {
                sprite::Manager::get(i)->is_hidden = false;
            }
        }
    }
    void Edit::exit_tile_set() {
        Console::log("state::Edit::exit_tile_set\n");
        sprite::Manager::get(m_tile_set_bg_sprite_id)->is_hidden = true;
        sprite::Manager::get(m_tile_set_sprite_id)->is_hidden = true;
        //transform::Manager::get(m_tile_set_bg_transform_id)->position.y = 512.0F;

        //transform::Manager::get(m_tile_set_transform_id)->position.x = view().w - 32.0F;
        //transform::Manager::get(m_tile_set_transform_id)->position.y = view().w == 480.0F ? 240.0F : 160.0F;

        /*if (!m_selection_on_tile_set_sprite_ids.empty() && sprite::Manager::get(m_selection_on_tile_set_sprite_ids.back())) {
            cVec2F tile_offset = sprite::Manager::get(m_selection_on_tile_set_sprite_ids.back())->offset;
            sprite::Manager::get(m_tile_set_sprite_id)->source_rect = { (I32)tile_offset.x, (I32)tile_offset.y, 16, 16 };
            sprite::Manager::get(m_tile_set_sprite_id)->layer = MENU_LAYER + 1;
        }*/

        //Console::log("tile set focus: ", m_tile_set_focus.x, " ", m_tile_set_focus.y, "\n");

        sprite::Manager::get(m_text_current_tile_set_bg_sprite_id)->is_hidden = true;

        m_text_current_tile_set.set_is_hidden(true);
           
        for (auto& i : m_is_hidden_layer_sprite_ids) {
            sprite::Manager::get(i)->layer = MENU_LAYER;
        }
        sprite::Manager::get(m_active_layer_sprite_id)->is_hidden = false;
        sprite::Manager::get(m_save_sprite_id)->is_hidden = false;
        sprite::Manager::get(m_grid_icon_sprite_id)->is_hidden = false;
        
        for (auto& i : m_grid_sprite_ids) {
            if (sprite::Manager::get(i)) sprite::Manager::get(i)->is_hidden = m_is_hidden_grid;
        }
        for (auto& i : m_grid_map_sprite_ids) {
            if (sprite::Manager::get(i)) sprite::Manager::get(i)->is_hidden = m_is_hidden_grid_map;
        }
        sprite::Manager::get(m_position_on_grid_map_sprite_id)->is_hidden = m_is_hidden_grid_map;
        

        if (m_level_path.empty()) {
            m_text_bar.clear_text();
        } else {
            m_text_bar.set_text(m_level_path.string());
        }

        for (auto& i : m_selection_on_tile_set_sprite_ids) {
            if (sprite::Manager::get(i)) {
                sprite::Manager::get(i)->is_hidden = true;
            }
        }
    }
    void Edit::move_tile_set(cVec2F amount) {
        //Console::log("state::Edit::move_tile_set ", amount.x, " ", amount.y, " ", " position: ", transform::Manager::get(m_tile_set_transform_id)->position.x, " ", transform::Manager::get(m_tile_set_transform_id)->position.y, "\n");        

        //cF32 edge_y = m_is_showing_tile_set ? 0.0F : 512.0F;
        cF32 edge_y = 0.0F;
        cVec2F prev_position = transform::Manager::get(m_tile_set_transform_id)->position;
        
        cVec2F limit = { view().w - 512.0F - 32.0F,
                         view().h == 270.0F ? -272.0F : -352.0F };

        transform::Manager::get(m_tile_set_transform_id)->position += amount;
        if (transform::Manager::get(m_tile_set_transform_id)->position.y > edge_y) {
            transform::Manager::get(m_tile_set_transform_id)->position.y = edge_y;
        } else if (transform::Manager::get(m_tile_set_transform_id)->position.y < edge_y + limit.y) {
            transform::Manager::get(m_tile_set_transform_id)->position.y = edge_y + limit.y;
        }
        if (transform::Manager::get(m_tile_set_transform_id)->position.x > 0.0F) {
            transform::Manager::get(m_tile_set_transform_id)->position.x = 0.0F;
        } else if (transform::Manager::get(m_tile_set_transform_id)->position.x < limit.x) {
            transform::Manager::get(m_tile_set_transform_id)->position.x = limit.x;
        }
        transform::Manager::get(m_tile_set_scroll_transform_id)->position += (transform::Manager::get(m_tile_set_transform_id)->position - prev_position);
        Console::log("tile set scroll position: ", transform::Manager::get(m_tile_set_scroll_transform_id)->position.x, " ", transform::Manager::get(m_tile_set_scroll_transform_id)->position.y, "\n");

        m_tile_set_focus = transform::Manager::get(m_tile_set_transform_id)->position;
    }
    bool Edit::select_on_tile_set(cVec2F offset) {        
        for (auto& sel_id : m_selection_on_tile_set_sprite_ids) {
            if (!sprite::Manager::get(sel_id)) continue;            
            if (sprite::Manager::get(sel_id)->offset == offset) {
                Console::log("state::Edit::select_on_tile_set", offset.x, " ", offset.y, " already selected\n");
                return false;
            }
        }
        cI32 sprite_id = sprite::Manager::make("res/texture/tile_selection.png");
        sprite::Manager::get(sprite_id)->layer = SELECTION_ON_TILE_SET_LAYER;
        sprite::Manager::get(sprite_id)->transform_id = m_tile_set_scroll_transform_id;
        sprite::Manager::get(sprite_id)->offset = offset;
        m_selection_on_tile_set_sprite_ids.push_back(sprite_id);

        sprite::Manager::get(m_current_tile_sprite_id)->source_rect = { (I32)offset.x, (I32)offset.y, 16,16 };
        
        m_text_bar.clear_text();
        if (m_tile_set == 255 && m_selection_on_tile_set_sprite_ids.size() == 1) {
            cVec2F tile_offset = offset / 16.0F;
            m_tile_number = std::fmodf(tile_offset.x, 32.0F) + tile_offset.y * 32.0F;

            //if (m_types.at(entity::Info{ 255, m_tile_number }) == m_text_bar.get_text()) {
                //return false;
            //}

            if (m_types.find(entity::Info{ 255, m_tile_number }) != m_types.end()) {
                m_text_bar.set_text(m_types.at(entity::Info{ 255, m_tile_number }));
                int num = 0;
                //Console::log("state::Edit::select_on_tile_set text_bar size: ", m_text_bar.get_sprite_ids().size(), "\n");
                
                for (auto& i : m_text_bar.get_sprite_ids()) {
                    if (!sprite::Manager::get(i)) {
                        Console::log("text_bar sprite_id not valid\n");
                    }
                    //Console::log(num, " ", sprite::Manager::get(i)->source_rect.x, " ", sprite::Manager::get(i)->source_rect.y, " ", sprite::Manager::get(i)->source_rect.w, " ", sprite::Manager::get(i)->source_rect.h, "\n");
                    //Console::log(num, " ", (int)sprite::Manager::get(i)->layer, "\n");
                //}
                //++num;
                }
            }            
        }
        return true;
    }
    bool Edit::deselect_on_tile_set(cVec2F offset) {
        for (auto& sel_id : m_selection_on_tile_set_sprite_ids) {
            if (!sprite::Manager::get(sel_id) || sprite::Manager::get(sel_id)->transform_id != m_tile_set_scroll_transform_id) continue;
            if (sprite::Manager::get(sel_id)->offset == offset) {
                Console::log("state::Edit::deselect_on_tile_set aready selected, deselecting\n");
                sprite::Manager::erase(sel_id);
                std::vector<I32> resized_selection;
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
    bool Edit::move_selection_on_tile_set(cVec2F amount) {
        Console::log("state::Edit::move_selection_on_tile_set ", amount.x, " ", amount.y, "\n");
        if (m_selection_on_tile_set_sprite_ids.empty()) return false;
        for (auto& i : m_selection_on_tile_set_sprite_ids) {
            if (sprite::Manager::get(i)->offset.x + amount.x < 0      || sprite::Manager::get(i)->offset.y + amount.y < 0 ||
                sprite::Manager::get(i)->offset.x + amount.x > 496.0F || sprite::Manager::get(i)->offset.y + amount.y > 496.0F) {
                return false;
            }

            //cF32 edge_y = m_is_showing_tile_set ? 0.0F : 512.0F;
            if (m_is_showing_tile_set) {
                cF32 edge_y = 0.0F;
                if (amount.x < 0.0F && transform::Manager::get(m_tile_set_transform_id)->position.x + sprite::Manager::get(i)->offset.x + amount.x < 0.0F ||
                    amount.x > 0.0F && transform::Manager::get(m_tile_set_transform_id)->position.x + sprite::Manager::get(i)->offset.x + amount.x > view().w - 48.0F ||
                    amount.y < 0.0F && transform::Manager::get(m_tile_set_transform_id)->position.y + sprite::Manager::get(i)->offset.y + amount.y < edge_y ||
                    amount.y > 0.0F && transform::Manager::get(m_tile_set_transform_id)->position.y + sprite::Manager::get(i)->offset.y + amount.y > edge_y + view().h - 32.0F) {
                    move_tile_set(-amount);
                }
            }
        }
        //transform::Manager::get(m_tile_set_transform_id)->position += amount;

        for (auto& i : m_selection_on_tile_set_sprite_ids) {
            sprite::Manager::get(i)->offset += amount;
        }

        cVec2F offset = sprite::Manager::get(m_selection_on_tile_set_sprite_ids.front())->offset;

        sprite::Manager::get(m_current_tile_sprite_id)->source_rect.x += amount.x;
        sprite::Manager::get(m_current_tile_sprite_id)->source_rect.y += amount.y;

        cVec2F tile_offset = offset / 16.0F;
        m_tile_number = std::fmodf(tile_offset.x, 32.0F) + tile_offset.y * 32.0F;

        if (m_is_showing_tile_set) {
            if (m_tile_set == 255 && m_selection_on_tile_set_sprite_ids.size() == 1) {
                if (m_types.find(entity::Info{ 255, m_tile_number }) != m_types.end()) {
                    m_text_bar.set_text(m_types.at(entity::Info{ 255, m_tile_number }));
                }
            }
        }
        return true;
    }
    bool Edit::deselect_all_on_tile_set() {        
        if (m_selection_on_tile_set_sprite_ids.empty()) return false;
        Console::log("state::Edit::deselect_all_on_tile_set\n");
        for (auto& i : m_selection_on_tile_set_sprite_ids) {
            if (!sprite::Manager::get(i)) continue;
            sprite::Manager::erase(i);
        }
        m_selection_on_tile_set_sprite_ids.clear();
        return true;
    }
}