module;
#include <filesystem>
#include <sstream>
#include <string>

module state.edit;

import console;
import input;
import transforms;
import sprites;
import types;

namespace state {
    void Edit::handle_menu_up() {
        size_t mouse_on_menu_bar_item = -1;
        for (size_t i = 0; i < m_menu_up_bar_bg_sprite_ids.size(); ++i) {
            if (Inputs::mouse.x >= Sprites::at(m_menu_up_bar_bg_sprite_ids.at(i))->offset.x && Inputs::mouse.x <= Sprites::at(m_menu_up_bar_bg_sprite_ids.at(i))->offset.x + Sprites::at(m_menu_up_bar_bg_sprite_ids.at(i))->source_rect.w &&
                Inputs::mouse.y >= Sprites::at(m_menu_up_bar_bg_sprite_ids.at(i))->offset.y && Inputs::mouse.y <= Sprites::at(m_menu_up_bar_bg_sprite_ids.at(i))->offset.y + Sprites::at(m_menu_up_bar_bg_sprite_ids.at(i))->source_rect.h) {
                mouse_on_menu_bar_item = i;
            }
        }
        m_is_mouse_on_menu_up_list = false;
        bool is_mouse_on_menu_up_list[2] = { false };
        if (Inputs::mouse.y >= Transforms::at(m_menu_up_lists[m_menu_up_labels[0]].transform_id)->position.y - 2 &&
            Inputs::mouse.y <= Transforms::at(m_menu_up_lists[m_menu_up_labels[0]].transform_id)->position.y + m_menu_up_lists[m_menu_up_labels[0]].bg_h &&
            Inputs::mouse.x >= Transforms::at(m_menu_up_lists[m_menu_up_labels[0]].transform_id)->position.x &&
            Inputs::mouse.x <= Transforms::at(m_menu_up_lists[m_menu_up_labels[0]].transform_id)->position.x + m_menu_up_lists[m_menu_up_labels[0]].bg_w) {
            m_is_mouse_on_menu_up_list = true;
            is_mouse_on_menu_up_list[0] = true;
        } else if (Inputs::mouse.y >= Transforms::at(m_menu_up_lists[m_menu_up_labels[1]].transform_id)->position.y - 2 &&
            Inputs::mouse.y <= Transforms::at(m_menu_up_lists[m_menu_up_labels[1]].transform_id)->position.y + m_menu_up_lists[m_menu_up_labels[1]].bg_h &&
            Inputs::mouse.x >= Transforms::at(m_menu_up_lists[m_menu_up_labels[1]].transform_id)->position.x &&
            Inputs::mouse.x <= Transforms::at(m_menu_up_lists[m_menu_up_labels[1]].transform_id)->position.x + m_menu_up_lists[m_menu_up_labels[1]].bg_w)
                    
            {
            m_is_mouse_on_menu_up_list = true;
            is_mouse_on_menu_up_list[1] = true;
        }
        
        for (auto& i : m_menu_up_bar) {
           i.get()->texture("res/fonts/font_8_gray.png");
        }
        //Console::log("m_is_mouse_on_menu_up_open_list: ", m_is_mouse_on_menu_up_open_list, "\n");
        if (mouse_on_menu_bar_item == 0 || is_mouse_on_menu_up_list[0]) {
            m_menu_up_bar.at(0).get()->texture("res/fonts/font_8_white.png");
            Transforms::at(m_menu_up_lists[m_menu_up_labels[0]].transform_id)->position = {0.0f, 8.0f};
            
            f32c level_list_entry_y = Inputs::mouse.y - Transforms::at(m_menu_up_lists[m_menu_up_labels[0]].transform_id)->position.y;
            const size_t level_list_entry = (size_t)(level_list_entry_y - std::fmodf(level_list_entry_y, 8.0f)) / 8;
            if (level_list_entry < m_menu_up_lists[m_menu_up_labels[0]].items.size()) {
                m_menu_up_bar.at(0).get()->texture("res/fonts/font_8_white.png");
                for (size_t i = 0; i < m_menu_up_lists[m_menu_up_labels[0]].items.size(); ++i) {
                    if (i != level_list_entry) {
                        m_menu_up_lists[m_menu_up_labels[0]].items.at(i).get()->texture("res/fonts/font_8_gray.png");
                    }
                }
                m_menu_up_lists[m_menu_up_labels[0]].items.at(level_list_entry).get()->texture("res/fonts/font_8_white.png");

                if (Inputs::at(m_input_id)->is_pressed(input::Button::left)) {
                    Inputs::at(m_input_id)->release(input::Button::left);
                    Console::log("state::Edit::update menu up clicked on ", level_list_entry, " ", m_menu_up_lists[m_menu_up_labels[0]].items.at(level_list_entry).get()->get_text(), "\n");

                    m_level_path = m_menu_up_lists[m_menu_up_labels[0]].items.at(level_list_entry).get()->get_text();

                    clear_grid_sprites();
                    clear_level_sprites();                        

                    load_level_sprites_from(m_level_path);
                }
                if (Inputs::at(m_input_id)->is_pressed(input::Key::del)) {
                    Inputs::at(m_input_id)->release(input::Key::del);
                    m_info_message.set_offset({ m_menu_up_lists[m_menu_up_labels[0]].items.at(level_list_entry).get()->get_offset().x + m_menu_up_lists[m_menu_up_labels[0]].bg_w,
                                                m_menu_up_lists[m_menu_up_labels[0]].items.at(level_list_entry).get()->get_offset().y + 8.0f});

                    m_info_message.set_is_hidden(false);
                    m_info_message.set_text("(y/n)");

                    m_is_asked_to_remove_level = { true, level_list_entry };
                    return;
                }
            }        
            Transforms::at(m_menu_up_lists[m_menu_up_labels[1]].transform_id)->position = { 0.0f, m_menu_up_lists[m_menu_up_labels[1]].bg_h * -1.0f };
        }
        if (mouse_on_menu_bar_item == 1 || is_mouse_on_menu_up_list[1]) {
            m_menu_up_bar.at(1).get()->texture("res/fonts/font_8_white.png");
            Transforms::at(m_menu_up_lists[m_menu_up_labels[0]].transform_id)->position = { 0.0f, m_menu_up_lists[m_menu_up_labels[0]].bg_h * -1.0f };
            Transforms::at(m_menu_up_lists[m_menu_up_labels[1]].transform_id)->position = { (f32)Sprites::at(m_menu_up_bar_bg_sprite_ids.at(0))->source_rect.w, 8.0f };

            f32c list_entry_y = Inputs::mouse.y - Transforms::at(m_menu_up_lists[m_menu_up_labels[1]].transform_id)->position.y;
            const size_t list_entry = (size_t)(list_entry_y - std::fmodf(list_entry_y, 8.0f)) / 8;

            if (list_entry < m_menu_up_lists[m_menu_up_labels[1]].items.size()) {
                for (size_t i = 0; i < m_menu_up_lists[m_menu_up_labels[1]].items.size(); ++i) {
                    if (i != list_entry) {
                        m_menu_up_lists[m_menu_up_labels[1]].items.at(i).get()->texture("res/fonts/font_8_gray.png");
                    }
                }
                m_menu_up_lists[m_menu_up_labels[1]].items.at(list_entry).get()->texture("res/fonts/font_8_white.png");
            }            
        }
    }
    bool Edit::add_to_menu_up_bar(const std::string& text) {
        std::unique_ptr<BitmapText> bitmap_text = std::make_unique<BitmapText>();

        f32 offset_x = 0.0f;
        for (auto& i : m_menu_up_bar_bg_sprite_ids) {
            offset_x += (f32)Sprites::at(i)->source_rect.w;
        }

        bitmap_text->transform_id = m_menu_up_transform_id;
        bitmap_text->layer = MENU_LAYER;
        bitmap_text->set_offset({ offset_x, 0.0f });
        bitmap_text->set_text(text);

        u16c bg_w = text.size() * (int)bitmap_text->font_size;
        u16c bg_h = (int)bitmap_text->font_size;

        m_menu_up_w += bg_w;

        m_menu_up_bar_bg_sprite_ids.push_back(Sprites::make("res/planes/textures/editor_menu_bg.png"));        
        Sprites::at(m_menu_up_bar_bg_sprite_ids.back())->source_rect = { 0, 0, bg_w, bg_h };
        Sprites::at(m_menu_up_bar_bg_sprite_ids.back())->layer = MENU_LAYER - 1;
        Sprites::at(m_menu_up_bar_bg_sprite_ids.back())->transform_id = m_menu_up_transform_id;
        Sprites::at(m_menu_up_bar_bg_sprite_ids.back())->offset = { offset_x, 0.0f };        

        m_menu_up_bar.push_back(std::move(bitmap_text));
        return true;
    }

    void Edit::load_menu_up_list(const std::string& menu, const std::filesystem::path& dir_path) {
        m_menu_up_lists[menu].items.clear();
        for (auto const& dir_entry : std::filesystem::directory_iterator{ dir_path }) {
            add_to_menu_up_list(menu, dir_entry);
        }
    }
    bool Edit::add_to_menu_up_list(const std::string& menu, const std::filesystem::path& path) {
        if (menu != m_menu_up_labels[0] && menu != m_menu_up_labels[1]) {
            Console::log("state::Edit::add_to_menu_up_list ", menu, " not implemented\n");
            return false;
        }
        std::string path_str = path.string();
        std::string separator_str;
        separator_str = (char)std::filesystem::path::preferred_separator;

        std::string folder_name = "levels";
        if (menu == m_menu_up_labels[1]) {
            folder_name = "prefabs";
        }

        size_t res_path_pos = path_str.find("res" + separator_str, 0);
        if (res_path_pos == std::string::npos) {
            Console::log("state::Edit::add_to_menu_up_list res" + separator_str + " not found!\n");
            return false;
        }
        path_str.erase(0, res_path_pos);
        
        std::unique_ptr<BitmapText> bitmap_text = std::make_unique<BitmapText>();
        bitmap_text->transform_id = m_menu_up_lists[menu].transform_id;
        bitmap_text->layer = MENU_LAYER;
        bitmap_text->set_offset({ 0.0f, m_menu_up_lists[menu].items.size() * (f32)bitmap_text->font_size });
        bitmap_text->set_text(path_str);
        
        u16c bg_w = path_str.size() * (int)bitmap_text->font_size;
        if (bg_w > m_menu_up_lists[menu].bg_w) {
            m_menu_up_lists[menu].bg_w = bg_w;
        }
        m_menu_up_lists[menu].bg_h = (int)(bitmap_text->font_size * (m_menu_up_lists[menu].items.size() + 1));
        Sprites::at(m_menu_up_lists[menu].bg_sprite_id)->source_rect = { 0, 0, m_menu_up_lists[menu].bg_w, m_menu_up_lists[menu].bg_h };
        
        m_menu_up_lists[menu].items.push_back(std::move(bitmap_text));
        return true;
    }    
    void Edit::select_tool(Vec2f position) {
        Console::log("state::Edit::select_tool\n");
    }
    void Edit::init_brush(u8 size) {
        if (size > 5) size = 1;
        
        Vec2f start, end;
        switch (size) {
        case 1: start = {  0.0f,  0.0f }, end = { 16.0f, 16.0f }; break;
        case 2: start = {  0.0f,  0.0f }, end = { 32.0f, 32.0f }; break;
        case 3: start = {-16.0f,-16.0f }, end = { 32.0f, 32.0f }; break;
        case 4: start = {-16.0f,-16.0f }, end = { 48.0f, 48.0f }; break;
        case 5: start = {-32.0f,-32.0f }, end = { 48.0f, 48.0f }; break; }

        for (const auto& i : m_mouse_sprite_ids) Sprites::erase(i);        
        m_mouse_sprite_ids.clear();

        for (f32 x = start.x; x < end.x; x += 16.0f) {
            for (f32 y = start.y; y < end.y; y += 16.0f) {
                m_mouse_sprite_ids.push_back(Sprites::make(m_mouse_texture_path));
                Sprites::at(m_mouse_sprite_ids.back())->id = m_mouse_sprite_ids.back();
                Sprites::at(m_mouse_sprite_ids.back())->transform_id = m_mouse_transform_id;
                Sprites::at(m_mouse_sprite_ids.back())->layer = 13;
                Sprites::at(m_mouse_sprite_ids.back())->offset.x = x;
                Sprites::at(m_mouse_sprite_ids.back())->offset.y = y;                
            }
        }
    }
    void Edit::handle_menu_down(Vec2f position, input::Button button) {
        if (m_is_showing_tile_set) {
            Vec2fc bg_sprite_pos  = Sprites::at(m_current_tile_set_bg_sprite_id)->offset + Transforms::at(m_menu_down_transform_id)->position;            
            Rectic bg_sprite_rect = Sprites::at(m_current_tile_set_bg_sprite_id)->source_rect;            

            if (position.x >= bg_sprite_pos.x && position.x <= bg_sprite_pos.x + bg_sprite_rect.w &&
                position.y >= bg_sprite_pos.y && position.y <= bg_sprite_pos.y + bg_sprite_rect.h) {
                Console::log("state::Edit::handle_menu_down position: ", position.x, " ", position.y, "\n");
                Console::log("state::Edit::handle_menu_down sprite pos: ", bg_sprite_pos.x, " ", bg_sprite_pos.y, "\n");

                Inputs::at(m_input_id)->release(button);
                switch (button) {
                case input::Button::left:  ++m_tile_set; break;
                case input::Button::right: --m_tile_set; break; }
                init_tile_set(m_tile_set);
            }
            return;
        }
        if (position.x == 0.0f && position.y == 160.0f) {
            Console::log("state::Edit::handle_menu_down save\n");
            Inputs::at(m_input_id)->release(button);
            if (button == input::Button::left) {
                if (m_is_typing_current_level) {
                    finish_typing_current_level_and_save();
                    m_is_typing_current_level = false;
                } else {
                    //just save
                    save_current_level();
                }
            } else if (button == input::Button::right) {
                if (!m_is_typing_current_level) {
                    init_typing_current_level();
                    m_is_typing_current_level = true;
                } else {
                    quit_typing_current_level();                    
                }
            }
            /*if (m_is_typing_current_level) {
                finish_typing_current_level_and_save();
                m_is_typing_current_level = false;
            } else {
                init_typing_current_level();
                m_is_typing_current_level = true;
            }*/
            //Sprites::erase_null();
        } else if (position.y < 160) {
            Inputs::at(m_input_id)->release(button);
            if (position.x == 288) {
                Sprites::at(m_active_layer_sprite_id)->offset.y = position.y + 32.0f;
                m_layer = (u8)(position.y / 16.0f);
            } else if (position.x == 304) {
                u8 layer = (u8)(position.y / 16.0f);
                if (layer < m_is_hidden_layer_sprite_ids.size() - 1 && m_is_hidden_layer_sprite_ids.at(layer)) {
                    Sprites::at(m_is_hidden_layer_sprite_ids.at(layer))->is_hidden = !Sprites::at(m_is_hidden_layer_sprite_ids.at(layer))->is_hidden;
                }
                if (Sprites::at(m_is_hidden_layer_sprite_ids.at(layer))->is_hidden) {
                    remove_visible_layer(layer);                    
                } else {
                    add_visible_layer(layer);                    
                }
            }
        }
    }
    void Edit::clear_typed_current_level() {
        m_level_path.clear();
        m_current_level.clear_text();
    }
    bool Edit::init_typing_current_level() {
        std::string level_path_str;
        if (m_current_level.get_text().empty()) {
            std::filesystem::path level_path = std::filesystem::current_path() / "res" / "levels" / "";
            level_path_str = level_path.string();

            std::string separator_str;
            separator_str = (char)std::filesystem::path::preferred_separator;

            size_t res_path_pos = level_path_str.find("res" + separator_str + "levels", 0);

            if (res_path_pos == std::string::npos) {
                Console::log("state::Edit::init_typing_current_level res" + separator_str + "levels" + " not found!\n");
                return false;
            }
            level_path_str.erase(0, res_path_pos);
            level_path_str.insert(level_path_str.size(), ".bin");
        } else {
            level_path_str = m_current_level.get_text();
        }
        m_is_typing_current_level = true;
        
        m_typing_pos_current_level = level_path_str.size() - 4;
        level_path_str.insert(m_typing_pos_current_level, "_");
        
        m_current_level.set_text(level_path_str);

        Console::log("state::Edit::init_typing_current_level pos: ", m_typing_pos_current_level, "\n");
        return true;
    }
    bool Edit::shift_typing_pos_left() {
        if (m_typing_pos_current_level < 12) return false;
        std::string text = m_current_level.get_text();
        std::string c = text.substr(m_typing_pos_current_level - 1, 1);
        text.erase(m_typing_pos_current_level - 1, 1);
        text.insert(m_typing_pos_current_level, c);
        m_current_level.set_text(text);
        m_typing_pos_current_level -= 1;
        return true;
    }
    bool Edit::shift_typing_pos_right() {
        if (m_typing_pos_current_level > m_current_level.get_text().size() - 6) return false;
        std::string text = m_current_level.get_text();
        std::string c = text.substr(m_typing_pos_current_level + 1, 1);
        text.erase(m_typing_pos_current_level + 1, 1);
        text.insert(m_typing_pos_current_level, c);
        m_current_level.set_text(text);
        m_typing_pos_current_level += 1;
        return true;
    }
    bool Edit::insert_at_typing_pos_current_level(input::Key k, bool is_shift) {        
        if (m_typing_pos_current_level > 27) {
            return false;
        }
        std::string text = m_current_level.get_text();
        text.insert(m_typing_pos_current_level, input::string_from(k, is_shift));
        m_current_level.set_text(text);
        m_typing_pos_current_level += 1;
        return true;
    }
    bool Edit::erase_at_typing_pos_current_level() {
        if (m_typing_pos_current_level < 12) {
            return false;
        }
        std::string text = m_current_level.get_text();
        m_typing_pos_current_level -= 1;
        text.erase(m_typing_pos_current_level, 1);
        m_current_level.set_text(text);
        return true;
    }
    void Edit::handle_typing_current_level() {
        if (!m_is_typing_current_level || Inputs::at(m_input_id)->is_pressed(input::Key::ctrl)) return;
                
        if (Inputs::at(m_input_id)->is_pressed(input::Key::backspace)) {
            Inputs::at(m_input_id)->release(input::Key::backspace);
            erase_at_typing_pos_current_level();
        } else if (Inputs::at(m_input_id)->is_pressed(input::Key::enter)) {
            Inputs::at(m_input_id)->release(input::Key::enter);
            finish_typing_current_level_and_save();
        } else if (Inputs::at(m_input_id)->is_pressed(input::Key::left)) {
            Inputs::at(m_input_id)->release(input::Key::left);
            shift_typing_pos_left();
        } else if (Inputs::at(m_input_id)->is_pressed(input::Key::right)) {
            Inputs::at(m_input_id)->release(input::Key::right);
            shift_typing_pos_right();
        } else {
            input::Key key = input::Key::null;
            if (Inputs::at(m_input_id)->is_pressed(input::Key::subtract)) {
                key = input::Key::subtract;
            } else if (Inputs::at(m_input_id)->is_pressed(input::Key::period)) {
                key = input::Key::period;
            } else {
                for (i16 key_num = input::alphabet_begin(); key_num != input::alphabet_end(); ++key_num) {
                    if (Inputs::at(m_input_id)->is_pressed((input::Key)key_num)) {
                        key = (input::Key)key_num; goto found_a_key;
                    }
                }
                for (i16 key_num = input::numbers_begin(); key_num != input::numbers_end(); ++key_num) {
                    if (Inputs::at(m_input_id)->is_pressed((input::Key)key_num)) {
                        key = (input::Key)key_num; goto found_a_key;
                    }
                }
            found_a_key:
                ;
            }
            if (key != input::Key::null) {
                Inputs::at(m_input_id)->release(key);
                insert_at_typing_pos_current_level(key, Inputs::at(m_input_id)->is_pressed(input::Key::shift));
            }
        }
    }
    bool Edit::finish_typing_current_level_and_save() {
        if (m_current_level.get_text().size() < 17) {
            Console::log("state::Edit::finish_typing_current_level_and_save file name empty!\n");
            return false;
        }
        m_is_typing_current_level = false;
        std::string text = m_current_level.get_text();
        text.erase(m_typing_pos_current_level, 1);
        m_current_level.set_text(text);
        m_level_path = m_current_level.get_text();
        m_time_left_saving = 10;
        save_level_sprites_to(m_level_path);
        load_menu_up_list(m_menu_up_labels[0], std::filesystem::current_path() / "res" / "levels");
        load_menu_up_list(m_menu_up_labels[1], std::filesystem::current_path() / "res" / "prefabs");
        return true;
    }
    bool Edit::save_current_level() {
        if (m_current_level.get_text().size() < 16) {
            Console::log("state::Edit::save_current_level file name empty!\n");
            return false;
        }
        m_level_path = m_current_level.get_text();
        m_time_left_saving = 10;
        save_level_sprites_to(m_level_path);
        load_menu_up_list(m_menu_up_labels[0], std::filesystem::current_path() / "res" / "levels");
        load_menu_up_list(m_menu_up_labels[1], std::filesystem::current_path() / "res" / "prefabs");
        return true;
    }
    bool Edit::remove_level(const std::filesystem::path& path) {
        try {
            if (std::filesystem::remove(path)) {
                Console::log("state::Edit::remove_level: ", path, " deleted\n");
            } else {
                Console::log("state::Edit::remove_level: ", path, " not found\n");
            }
        } catch (const std::filesystem::filesystem_error& err) {
            Console::log("state::Edit::remove_level filesystem error : ", err.what(), "\n");
            return false;
        }
        //load_menu_up_open_list(std::filesystem::current_path() / "res" / "levels");
        return true;
    }
    bool Edit::quit_typing_current_level() {
        if (m_current_level.get_text().size() < 17) {
            Console::log("state::Edit::quit_typing_current_level file name empty!\n");
            return false;
        }
        m_is_typing_current_level = false;
        std::string text = m_current_level.get_text();
        text.erase(m_typing_pos_current_level, 1);
        m_current_level.set_text(text);
        m_level_path = m_current_level.get_text();        
        return true;
    }
}