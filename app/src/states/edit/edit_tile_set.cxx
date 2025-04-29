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

            u16c number = std::stoi(number_label);

            size_t type_start = equals + 1;
            while (file_text.at(type_start) == '	' || file_text.at(type_start) == ' ' || file_text.at(type_start) == '\n') {
                ++type_start;
            }

            const std::string type = file_text.substr(type_start, end_line - type_start);

            //const std::pair<u8, u16> tile_info = { 255, number };

            m_types.emplace(tile::Info{ 255, number }, type);

            label_start = end_line + 1;
        }


        
    }

    void Edit::init_tile_set(u8c tile_set) {
        m_prev_tile_set = m_tile_set;
        m_tile_set = tile_set;
        const std::string tile_set_str = std::to_string((int)m_tile_set);
        m_current_tile_set.set_text(tile_set_str);
        sprite::Set::at(m_tile_set_sprite_id)->texture("res/textures/set_" + tile_set_str + ".png");

        if (!m_is_showing_tile_set) return;
        
        m_text_bar.clear_text();

        if (tile_set == 255) {
            if (!m_selection_on_tile_set_sprite_ids.empty() && 
                m_types.find(tile::Info{ 255, m_tile_number }) != m_types.end()) {
                m_text_bar.set_text(m_types.at(tile::Info{ 255, m_tile_number }));
            }
            sprite::Set::at(m_save_sprite_id)->is_hidden = false;
        } else {
            //m_text_bar.clear_text();
            sprite::Set::at(m_save_sprite_id)->is_hidden = true;
        }        
                
        Console::log("state::Edit::init_tile_set tile_number: ", m_tile_number, "\n");
        
        //deselect_all_on_tile_set();
    }
    void Edit::enter_tile_set() {
        Console::log("state::Edit::enter_tile_set\n");
        transform::Set::at(m_tile_set_bg_transform_id)->position.y = 0.0f;
        transform::Set::at(m_tile_set_transform_id)->position.y -= 512.0f;
        sprite::Set::at(m_current_tile_set_bg_sprite_id)->is_hidden = true;
        
        for (auto& i : m_is_hidden_layer_sprite_ids) {
            sprite::Set::at(i)->is_hidden = true;
        }
        sprite::Set::at(m_active_layer_sprite_id)->is_hidden = true;

        sprite::Set::at(m_current_tile_set_bg_sprite_id)->is_hidden = false;        
        m_current_tile_set.set_is_hidden(false);
        if (m_tile_set == 255) {
            if (m_types.find(tile::Info{ 255, m_tile_number }) == m_types.end()) {
                m_text_bar.clear_text();
            } else {
                m_text_bar.set_text(m_types.at(tile::Info{ 255, m_tile_number }));
            }
            sprite::Set::at(m_save_sprite_id)->is_hidden = false;
        } else {
            m_text_bar.clear_text();
            sprite::Set::at(m_save_sprite_id)->is_hidden = true;
        }
        
    }
    void Edit::exit_tile_set() {
        Console::log("state::Edit::exit_tile_set\n");
        transform::Set::at(m_tile_set_bg_transform_id)->position.y = 512.0f;
        transform::Set::at(m_tile_set_transform_id)->position.y += 512.0f;
        sprite::Set::at(m_current_tile_set_bg_sprite_id)->is_hidden = true;
        m_current_tile_set.set_is_hidden(true);
           
        for (auto& i : m_is_hidden_layer_sprite_ids) {
            sprite::Set::at(i)->is_hidden = false;
        }
        sprite::Set::at(m_active_layer_sprite_id)->is_hidden = false;
        sprite::Set::at(m_save_sprite_id)->is_hidden = false;

        if (m_level_path.empty()) {
            m_text_bar.clear_text();
        } else {
            m_text_bar.set_text(m_level_path.string());
        }
    }
    void Edit::move_tile_set(Vec2fc amount) {
        //Console::log("state::Edit::move_tile_set ", amount.x, " ", amount.y, " ", " position: ", transform::Set::at(m_tile_set_transform_id)->position.x, " ", transform::Set::at(m_tile_set_transform_id)->position.y, "\n");        
        f32c edge_y = m_is_showing_tile_set ? 0.0f : 512.0f;
        Vec2fc prev_position = transform::Set::at(m_tile_set_transform_id)->position;

        transform::Set::at(m_tile_set_transform_id)->position += amount;
        if (transform::Set::at(m_tile_set_transform_id)->position.y > edge_y) {
            transform::Set::at(m_tile_set_transform_id)->position.y = edge_y;
        } else if (transform::Set::at(m_tile_set_transform_id)->position.y < edge_y - 352.0f) {
            transform::Set::at(m_tile_set_transform_id)->position.y = edge_y - 352.0f;
        }
        if (transform::Set::at(m_tile_set_transform_id)->position.x > 0.0f) {
            transform::Set::at(m_tile_set_transform_id)->position.x = 0.0f;
        } else if (transform::Set::at(m_tile_set_transform_id)->position.x < -192.0f) {
            transform::Set::at(m_tile_set_transform_id)->position.x = -192.0f;
        }
        transform::Set::at(m_selection_on_tile_set_transform_id)->position += (transform::Set::at(m_tile_set_transform_id)->position - prev_position);
    }
    bool Edit::select_on_tile_set(Vec2fc offset) {        
        for (auto& sel_id : m_selection_on_tile_set_sprite_ids) {
            if (!sprite::Set::at(sel_id)) continue;            
            if (sprite::Set::at(sel_id)->offset == offset) {
                Console::log("state::Edit::select_on_tile_set", offset.x, " ", offset.y, " already selected\n");
                return false;
            }
        }
        m_selection_on_tile_set_sprite_ids.push_back(sprite::Set::make("res/textures/tile_selection.png"));
        sprite::Set::at(m_selection_on_tile_set_sprite_ids.back())->layer = SELECTION_ON_TILE_SET_LAYER;
        sprite::Set::at(m_selection_on_tile_set_sprite_ids.back())->transform_id = m_selection_on_tile_set_transform_id;
        sprite::Set::at(m_selection_on_tile_set_sprite_ids.back())->offset = offset;
        
        m_text_bar.clear_text();
        if (m_tile_set == 255 && m_selection_on_tile_set_sprite_ids.size() == 1) {
            Vec2fc tile_offset = offset / 16.0f;
            m_tile_number = std::fmodf(tile_offset.x, 32.0f) + tile_offset.y * 32.0f;

            if (m_types.find(tile::Info{ 255, m_tile_number }) != m_types.end()) {
                m_text_bar.set_text(m_types.at(tile::Info{ 255, m_tile_number }));
                int num = 0;
                //Console::log("state::Edit::select_on_tile_set text_bar size: ", m_text_bar.get_sprite_ids().size(), "\n");
                
                for (auto& i : m_text_bar.get_sprite_ids()) {
                    if (!sprite::Set::at(i)) {
                        Console::log("text_bar sprite_id not valid\n");
                    }
                    //Console::log(num, " ", sprite::Set::at(i)->source_rect.x, " ", sprite::Set::at(i)->source_rect.y, " ", sprite::Set::at(i)->source_rect.w, " ", sprite::Set::at(i)->source_rect.h, "\n");
                    //Console::log(num, " ", (int)sprite::Set::at(i)->layer, "\n");
                //}
                //++num;
                }
            }            
        }
        return true;
    }
    bool Edit::deselect_on_tile_set(Vec2fc offset) {
        for (auto& sel_id : m_selection_on_tile_set_sprite_ids) {
            if (!sprite::Set::at(sel_id) || sprite::Set::at(sel_id)->transform_id != m_selection_on_tile_set_transform_id) continue;
            if (sprite::Set::at(sel_id)->offset == offset) {
                Console::log("state::Edit::deselect_on_tile_set aready selected, deselecting\n");
                sprite::Set::erase(sel_id);
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
        Console::log("state::Edit::move_selection_on_tile_set ", amount.x, " ", amount.y, "\n");
        if (m_selection_on_tile_set_sprite_ids.empty()) return false;
        for (auto& i : m_selection_on_tile_set_sprite_ids) {
            if (sprite::Set::at(i)->offset.x + amount.x < 0 || sprite::Set::at(i)->offset.x + amount.x > 464.0f ||
                sprite::Set::at(i)->offset.y + amount.y < 0 || sprite::Set::at(i)->offset.y + amount.y > 496.0f
                ) {
                return false;
            }
            f32c edge_y = m_is_showing_tile_set ? 0.0f : 512.0f;
            if (amount.x < 0.0f && transform::Set::at(m_tile_set_transform_id)->position.x + sprite::Set::at(i)->offset.x + amount.x < 0.0f    ||
                amount.x > 0.0f && transform::Set::at(m_tile_set_transform_id)->position.x + sprite::Set::at(i)->offset.x + amount.x > 272.0f  ||
                amount.y < 0.0f && transform::Set::at(m_tile_set_transform_id)->position.y + sprite::Set::at(i)->offset.y + amount.y < edge_y ||
                amount.y > 0.0f && transform::Set::at(m_tile_set_transform_id)->position.y + sprite::Set::at(i)->offset.y + amount.y > edge_y + 144.0f) {
                move_tile_set(-amount);
            }
        }
        //transform::Set::at(m_tile_set_transform_id)->position += amount;

        for (auto& i : m_selection_on_tile_set_sprite_ids) {
            sprite::Set::at(i)->offset += amount;
        }

        Vec2fc offset = sprite::Set::at(m_selection_on_tile_set_sprite_ids.front())->offset;
        Vec2fc tile_offset = offset / 16.0f;
        m_tile_number = std::fmodf(tile_offset.x, 32.0f) + tile_offset.y * 32.0f;

        if (m_tile_set == 255 && m_selection_on_tile_set_sprite_ids.size() == 1) {
            if (m_types.find(tile::Info{ 255, m_tile_number }) != m_types.end()) {
                m_text_bar.set_text(m_types.at(tile::Info{ 255, m_tile_number }));
            } else {
                m_text_bar.clear_text();
            }
        }
        return true;
    }
    bool Edit::deselect_all_on_tile_set() {
        Console::log("state::Edit::deselect_all_on_tile_set\n");
        if (m_selection_on_tile_set_sprite_ids.empty()) return false;
        for (auto& i : m_selection_on_tile_set_sprite_ids) {
            if (!sprite::Set::at(i)) continue;
            sprite::Set::erase(i);
        }
        m_selection_on_tile_set_sprite_ids.clear();
        return true;
    }



}