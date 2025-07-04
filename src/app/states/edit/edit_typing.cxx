module;
#include <filesystem>
#include <fstream>
#include <string>

module state.edit;

namespace state {
    bool Edit::init_typing_text_bar() {
        if (m_is_showing_tile_set) {
            if (m_tile_set != 255) return false;
            std::string type_str = m_text_bar.get_text();
            m_typing_pos = type_str.size();
            type_str.append("_");
            m_text_bar.set_text(type_str);
            return true;
        } else {
            std::string level_path_str;
            if (m_text_bar.get_text().empty()) {
                std::filesystem::path level_path = std::filesystem::current_path() / "res" / "levels" / "";
                level_path_str = level_path.string();

                std::string separator_str;
                separator_str = (char)std::filesystem::path::preferred_separator;

                size_t res_path_pos = level_path_str.find("res" + separator_str + "levels", 0);

                if (res_path_pos == std::string::npos) {
                    Console::log("state::Edit::init_typing_text_bar res" + separator_str + "levels" + " not found!\n");
                    return false;
                }
                level_path_str.erase(0, res_path_pos);
                level_path_str.insert(level_path_str.size(), ".bin");
            } else {
                level_path_str = m_text_bar.get_text();
            }
            m_is_typing_text_bar = true;

            m_typing_pos = level_path_str.size() - 4;
            level_path_str.insert(m_typing_pos, "_");

            m_text_bar.set_text(level_path_str);

            Console::log("state::Edit::init_typing_text_bar pos: ", m_typing_pos, "\n");
            return true;
        }
    }
    bool Edit::quit_typing_text_bar() {
        if (!m_is_typing_text_bar) return false;
        m_is_typing_text_bar = false;
        if (m_is_showing_tile_set) {
            if (m_text_bar.get_text().empty()) {
                Console::log("state::Edit::quit_typing_text_bar_current_type empty!\n");
                return false;
            }
            std::string type_str = m_text_bar.get_text();
            if (type_str.size() > 1 && type_str.back() == '_') {
                type_str.pop_back();
                m_text_bar.set_text(type_str);
                if (m_types.find(tile::Info{ 255,  m_tile_number }) != m_types.end()) {
                    m_types.at(tile::Info{ 255, m_tile_number }) = type_str;
                } else {
                    //std::pair<u8, u16> tile_info = { 255, m_tile_number };
                    m_types.emplace(tile::Info{ 255, m_tile_number }, type_str);
                }                
            } else if (type_str.size() == 1 && type_str.back() == '_') {
                m_text_bar.clear_text();
                if (m_types.find(tile::Info{ 255,  m_tile_number }) != m_types.end()) {
                    m_types.erase(tile::Info{ 255, m_tile_number });
                }
                //m_types.at({ 255, m_tile_number }).clear();
            }
            
            return true;
        } else {
            if (m_text_bar.get_text().size() < 17) {
                Console::log("state::Edit::quit_typing_text_bar_current_level file name empty!\n");
                return false;
            }            
            std::string level_path_str = m_text_bar.get_text();
            level_path_str.erase(m_typing_pos, 1);
            m_text_bar.set_text(level_path_str);
            //m_level_path = m_text_bar.get_text();
            m_level_path = level_path_str;
            return true;
        }
    }
    void Edit::handle_typing_text_bar() {
        if (!m_is_typing_text_bar || input::Set::at(m_input_id)->is_pressed(input::Key::ctrl)) return;

        if (input::Set::at(m_input_id)->is_pressed(input::Key::del)) {
            input::Set::at(m_input_id)->release(input::Key::del);
            if (m_is_showing_tile_set) {
                m_text_bar.clear_text();
                if (m_types.find(tile::Info{ 255, m_tile_number }) != m_types.end()) {
                    m_types.erase(tile::Info{ 255, m_tile_number });
                }
            } else {

            }
            m_is_typing_text_bar = false;
        } else if (input::Set::at(m_input_id)->is_pressed(input::Key::backspace)) {
            input::Set::at(m_input_id)->release(input::Key::backspace);
            erase_at_text_bar_typing_pos();
        } else if (input::Set::at(m_input_id)->is_pressed(input::Key::enter)) {
            input::Set::at(m_input_id)->release(input::Key::enter);
            save_typed_text_bar();
        } else if (input::Set::at(m_input_id)->is_pressed(input::Key::left)) {
            input::Set::at(m_input_id)->release(input::Key::left);
            shift_text_bar_typing_pos_left();
        } else if (input::Set::at(m_input_id)->is_pressed(input::Key::right)) {
            input::Set::at(m_input_id)->release(input::Key::right);
            shift_text_bar_typing_pos_right();
        } else {
            input::Key key = input::Key::null;
            if (input::Set::at(m_input_id)->is_pressed(input::Key::subtract)) {
                key = input::Key::subtract;
            } else if (input::Set::at(m_input_id)->is_pressed(input::Key::period)) {
                key = input::Key::period;
            } else {
                for (i16 key_num = input::alphabet_begin(); key_num != input::alphabet_end(); ++key_num) {
                    if (input::Set::at(m_input_id)->is_pressed((input::Key)key_num)) {
                        key = (input::Key)key_num; goto found_a_key;
                    }
                }
                for (i16 key_num = input::numbers_begin(); key_num != input::numbers_end(); ++key_num) {
                    if (input::Set::at(m_input_id)->is_pressed((input::Key)key_num)) {
                        key = (input::Key)key_num; goto found_a_key;
                    }
                }
                found_a_key:;
            }
            if (key != input::Key::null) {
                input::Set::at(m_input_id)->release(key);
                insert_at_text_bar_typing_pos(key, input::Set::at(m_input_id)->is_pressed(input::Key::shift));
            }
        }
    }
    bool Edit::shift_text_bar_typing_pos_left() {
        if ((m_is_showing_tile_set && m_typing_pos < 1) ||
            (!m_is_showing_tile_set && m_typing_pos < 12)) {
            return false;
        }
        std::string text = m_text_bar.get_text();
        std::string c = text.substr(m_typing_pos - 1, 1);
        text.erase(m_typing_pos - 1, 1);
        text.insert(m_typing_pos, c);
        m_text_bar.set_text(text);
        m_typing_pos -= 1;
        return true;
    }
    bool Edit::shift_text_bar_typing_pos_right() {
        if ((m_is_showing_tile_set && m_typing_pos >= m_text_bar.get_text().size() - 1) ||
            (!m_is_showing_tile_set && m_typing_pos > m_text_bar.get_text().size() - 6)) {
            return false;
        }
        std::string text = m_text_bar.get_text();
        std::string c = text.substr(m_typing_pos + 1, 1);
        text.erase(m_typing_pos + 1, 1);
        text.insert(m_typing_pos, c);
        m_text_bar.set_text(text);        
        m_typing_pos += 1;
        return true;
    }
    bool Edit::insert_at_text_bar_typing_pos(input::Key key, bool is_shift) {
        if (m_typing_pos > 27) return false;
        if (m_is_showing_tile_set) {            
            std::string type_str = m_text_bar.get_text();
            type_str.insert(m_typing_pos, input::string_from(key, is_shift));
            m_text_bar.set_text(type_str);
        } else {            
            std::string text = m_text_bar.get_text();
            text.insert(m_typing_pos, input::string_from(key, is_shift));
            m_text_bar.set_text(text);
        }
        m_typing_pos += 1;
        return true;
    }
    bool Edit::erase_at_text_bar_typing_pos() {
        if (m_is_showing_tile_set) {
            if (m_typing_pos < 1) return false;
            std::string type_str = m_text_bar.get_text();
            m_typing_pos -= 1;
            type_str.erase(m_typing_pos, 1);
            m_text_bar.set_text(type_str);
        } else {
            if (m_typing_pos < 12) return false;            
            std::string text = m_text_bar.get_text();
            m_typing_pos -= 1;
            text.erase(m_typing_pos, 1);
            m_text_bar.set_text(text);
        }
        return true;
    }
    bool Edit::save_typed_text_bar() {
        if (m_text_bar.get_text().empty()) return false;        
        m_is_typing_text_bar = false;

        if (m_is_showing_tile_set) {            
            std::string text = m_text_bar.get_text();
            if (text.back() == '_') {
                if (text.size() == 1) {
                    m_text_bar.clear_text();
                } else {
                    text.erase(m_typing_pos, 1);
                    m_text_bar.set_text(text);
                    Console::log(m_text_bar.get_text(), "\n");
                }
            }
            if (m_types.find(tile::Info{ 255, m_tile_number }) == m_types.end() && !m_text_bar.get_text().empty()) {
                m_types.emplace(tile::Info{ 255, m_tile_number }, m_text_bar.get_text());                
            } else {
                if (m_text_bar.get_text().empty()) {
                    m_types.erase(tile::Info{ 255, m_tile_number });
                } else {
                    m_types.at(tile::Info{ 255, m_tile_number }) = m_text_bar.get_text();
                }
            }
            return true;
        } else {
            if (m_text_bar.get_text().back() == '_' && m_text_bar.get_text().size() < 17 || m_text_bar.get_text().size() < 16) {
                Console::log("state::Edit::finish_typing_current_level_and_save text size: ", m_text_bar.get_text().size(), "\n");
                return false;
            }            
            std::string text = m_text_bar.get_text();
            if (text.substr(text.size() - 4, 4) == "_.bin") {
                text.erase(m_typing_pos, 1);
                m_text_bar.set_text(text);
            }            
            m_level_path = m_text_bar.get_text();
            m_time_left_saving = 10;

            sprite::Set::save(m_level_path, m_grid_sprite_ids);

            load_menu_up_list(m_menu_up_labels[0], std::filesystem::current_path() / "res" / "levels");
            load_menu_up_list(m_menu_up_labels[1], std::filesystem::current_path() / "res" / "prefabs");
            return true;
        }
    }
    bool Edit::save_types_to_bin() {
        Console::log("state::Edit::save_types_to_bin\n");
        
        const std::filesystem::path path = std::filesystem::current_path() / "res" / "types.bin";

        std::ofstream out_file(path, std::ios::out | std::ios::binary);

        u16 type_count = m_types.size();

        out_file.write((i8*)&type_count, sizeof(u16));

        for (u16 i = 0; i < 32 * 32; ++i) {
            if (m_types.find(tile::Info{ 255, i }) != m_types.end()) {
                //Console::log(i, " ", m_types.at({ 255, i }), "\n");
                const char* type_data = m_types.at(tile::Info{ 255, i }).data();

                out_file.write((i8*)&i, sizeof(u16));
                out_file.write((i8*)type_data, sizeof(i8) * 32);
            }
        }
        out_file.close();
        return true;        
    }
    bool Edit::load_types_from_bin() {
        const std::filesystem::path path = std::filesystem::current_path() / "res" / "types.bin";

        std::ifstream in_file(path, std::ios::in | std::ios::binary);
        if (!in_file.is_open()) return false;

        m_types.clear();

        u16 type_count = 0;
        in_file.read((i8*)&type_count, sizeof(u16));

        Console::log("state::Edit::load_types count: ", type_count, "\n");
                
        for (u16 i = 0; i < type_count; ++i) {
            u16 number = 0;
            char type[32] = {};
            in_file.read((i8*)&number, sizeof(u16));
            in_file.read((i8*)type, sizeof(char) * 32);

            //Console::log("state::Edit::load_types_from_bin type: ", number, " ", type, "\n");

            //const std::pair<u8, u16> tile_info = { 255, number };

            m_types.emplace(tile::Info{ 255, number }, type);
            
        }

        in_file.close();

        return true;
    }
}