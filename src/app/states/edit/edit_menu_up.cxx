module;
#include <cmath>
#include <filesystem>
#include <string>
#include <memory>

module state.edit;

namespace state {
    void Edit::handle_menu_up() {
        size_t mouse_on_menu_bar_item = -1;
        for (size_t i = 0; i < m_menu_up_bar_bg_sprite_ids.size(); ++i) {
            if (input::Manager::mouse.x >= sprite::Manager::get(m_menu_up_bar_bg_sprite_ids.at(i))->offset.x && input::Manager::mouse.x <= sprite::Manager::get(m_menu_up_bar_bg_sprite_ids.at(i))->offset.x + sprite::Manager::get(m_menu_up_bar_bg_sprite_ids.at(i))->source_rect.w &&
                input::Manager::mouse.y >= sprite::Manager::get(m_menu_up_bar_bg_sprite_ids.at(i))->offset.y && input::Manager::mouse.y <= sprite::Manager::get(m_menu_up_bar_bg_sprite_ids.at(i))->offset.y + sprite::Manager::get(m_menu_up_bar_bg_sprite_ids.at(i))->source_rect.h) {
                mouse_on_menu_bar_item = i;
            }
        }
        m_is_mouse_on_menu_up_list = false;
        bool is_mouse_on_menu_up_list[2] = { false };
        if (!input::Manager::get(m_input_id)->is_pressed(input::Key::ctrl)) {
            if (input::Manager::mouse.y >= transform::Manager::get(m_menu_up_lists[m_menu_up_labels[0]].transform_id)->position.y - 2 &&
                input::Manager::mouse.y <= transform::Manager::get(m_menu_up_lists[m_menu_up_labels[0]].transform_id)->position.y + m_menu_up_lists[m_menu_up_labels[0]].bg_h &&
                input::Manager::mouse.x >= transform::Manager::get(m_menu_up_lists[m_menu_up_labels[0]].transform_id)->position.x &&
                input::Manager::mouse.x <= transform::Manager::get(m_menu_up_lists[m_menu_up_labels[0]].transform_id)->position.x + m_menu_up_lists[m_menu_up_labels[0]].bg_w) {
                m_is_mouse_on_menu_up_list = true;
                is_mouse_on_menu_up_list[0] = true;
            } else if (input::Manager::mouse.y >= transform::Manager::get(m_menu_up_lists[m_menu_up_labels[1]].transform_id)->position.y - 2 &&
                input::Manager::mouse.y <= transform::Manager::get(m_menu_up_lists[m_menu_up_labels[1]].transform_id)->position.y + m_menu_up_lists[m_menu_up_labels[1]].bg_h &&
                input::Manager::mouse.x >= transform::Manager::get(m_menu_up_lists[m_menu_up_labels[1]].transform_id)->position.x &&
                input::Manager::mouse.x <= transform::Manager::get(m_menu_up_lists[m_menu_up_labels[1]].transform_id)->position.x + m_menu_up_lists[m_menu_up_labels[1]].bg_w)

            {
                m_is_mouse_on_menu_up_list = true;
                is_mouse_on_menu_up_list[1] = true;
            }
        }

        for (auto& i : m_menu_up_bar) {
            i.get()->texture("res/texture/font_8_gray.png");
        }
        //Console::log("m_is_mouse_on_menu_up_open_list: ", m_is_mouse_on_menu_up_open_list, "\n");
        if (mouse_on_menu_bar_item == 0 || is_mouse_on_menu_up_list[0]) {
            m_menu_up_bar.at(0).get()->texture("res/texture/font_8_white.png");
            transform::Manager::get(m_menu_up_lists[m_menu_up_labels[0]].transform_id)->position = { 0.0F, 8.0F };

            cF32 level_list_entry_y = input::Manager::mouse.y - transform::Manager::get(m_menu_up_lists[m_menu_up_labels[0]].transform_id)->position.y;
            const size_t level_list_entry = (size_t)(level_list_entry_y - std::fmodf(level_list_entry_y, 8.0F)) / 8;
            if (level_list_entry < m_menu_up_lists[m_menu_up_labels[0]].items.size()) {
                m_menu_up_bar.at(0).get()->texture("res/texture/font_8_white.png");
                for (size_t i = 0; i < m_menu_up_lists[m_menu_up_labels[0]].items.size(); ++i) {
                    if (i != level_list_entry) {
                        m_menu_up_lists[m_menu_up_labels[0]].items.at(i).get()->texture("res/texture/font_8_gray.png");
                    }
                }
                m_menu_up_lists[m_menu_up_labels[0]].items.at(level_list_entry).get()->texture("res/texture/font_8_white.png");

                if (input::Manager::get(m_input_id)->is_pressed(input::Button::left)) {
                    input::Manager::get(m_input_id)->release(input::Button::left);
                    Console::log("state::Edit::update menu up clicked on ", level_list_entry, " ", m_menu_up_lists[m_menu_up_labels[0]].items.at(level_list_entry).get()->get_text(), "\n");

                    m_level_path = m_menu_up_lists[m_menu_up_labels[0]].items.at(level_list_entry).get()->get_text();

                    clear_grid_sprites();
                    clear_level_sprites();

                    load_level_sprites(m_level_path);
                }
                if (input::Manager::get(m_input_id)->is_pressed(input::Key::del)) {
                    input::Manager::get(m_input_id)->release(input::Key::del);
                    m_info_message.set_offset({ m_menu_up_lists[m_menu_up_labels[0]].items.at(level_list_entry).get()->get_offset().x + m_menu_up_lists[m_menu_up_labels[0]].bg_w,
                                                m_menu_up_lists[m_menu_up_labels[0]].items.at(level_list_entry).get()->get_offset().y + 8.0F });

                    m_info_message.set_is_hidden(false);
                    m_info_message.set_text("(y/n)");

                    m_is_asked_to_remove_level = { true, level_list_entry };
                    return;
                }
            }
            transform::Manager::get(m_menu_up_lists[m_menu_up_labels[1]].transform_id)->position = { 0.0F, m_menu_up_lists[m_menu_up_labels[1]].bg_h * -1.0F };
        }
        if (mouse_on_menu_bar_item == 1 || is_mouse_on_menu_up_list[1]) {
            m_menu_up_bar.at(1).get()->texture("res/texture/font_8_white.png");
            transform::Manager::get(m_menu_up_lists[m_menu_up_labels[0]].transform_id)->position = { 0.0F, m_menu_up_lists[m_menu_up_labels[0]].bg_h * -1.0F };
            transform::Manager::get(m_menu_up_lists[m_menu_up_labels[1]].transform_id)->position = { (F32)sprite::Manager::get(m_menu_up_bar_bg_sprite_ids.at(0))->source_rect.w, 8.0F };

            cF32 list_entry_y = input::Manager::mouse.y - transform::Manager::get(m_menu_up_lists[m_menu_up_labels[1]].transform_id)->position.y;
            const size_t list_entry = (size_t)(list_entry_y - std::fmodf(list_entry_y, 8.0F)) / 8;

            if (list_entry < m_menu_up_lists[m_menu_up_labels[1]].items.size()) {
                for (size_t i = 0; i < m_menu_up_lists[m_menu_up_labels[1]].items.size(); ++i) {
                    if (i != list_entry) {
                        m_menu_up_lists[m_menu_up_labels[1]].items.at(i).get()->texture("res/texture/font_8_gray.png");
                    }
                }
                m_menu_up_lists[m_menu_up_labels[1]].items.at(list_entry).get()->texture("res/texture/font_8_white.png");
            }
        }
    }
    bool Edit::add_to_menu_up_bar(const std::string& text) {
        std::unique_ptr<BitmapText> bitmap_text = std::make_unique<BitmapText>();

        F32 offset_x = 0.0F;
        for (auto& i : m_menu_up_bar_bg_sprite_ids) {
            offset_x += (F32)sprite::Manager::get(i)->source_rect.w;
        }

        bitmap_text->transform_id = m_menu_up_transform_id;
        bitmap_text->layer = MENU_LAYER;
        bitmap_text->set_offset({ offset_x, 0.0F });
        bitmap_text->set_text(text);

        cU16 bg_w = text.size() * (int)bitmap_text->font_size;
        cU16 bg_h = (int)bitmap_text->font_size;

        m_menu_up_w += bg_w;

        m_menu_up_bar_bg_sprite_ids.push_back(sprite::Manager::make("res/texture/editor_menu_bg.png"));
        sprite::Manager::get(m_menu_up_bar_bg_sprite_ids.back())->source_rect = { 0, 0, bg_w, bg_h };
        sprite::Manager::get(m_menu_up_bar_bg_sprite_ids.back())->layer = MENU_LAYER - 1;
        sprite::Manager::get(m_menu_up_bar_bg_sprite_ids.back())->transform_id = m_menu_up_transform_id;
        sprite::Manager::get(m_menu_up_bar_bg_sprite_ids.back())->offset = { offset_x, 0.0F };

        m_menu_up_bar.emplace_back(std::move(bitmap_text));
        return true;
    }

    void Edit::load_menu_up_list(const std::string& menu, const std::filesystem::path& dir_path) {
        m_menu_up_lists[menu].items.clear();
        for (auto const& dir_entry : std::filesystem::directory_iterator{ dir_path }) {            
            add_to_menu_up_list(menu, dir_entry);            
        }
        transform::Manager::get(m_menu_up_lists[menu].transform_id)->position = { 0.0F, m_menu_up_lists[menu].bg_h * -1.0F };
    }
    bool Edit::add_to_menu_up_list(const std::string& menu, const std::filesystem::path& path) {
        if (path.string().substr(path.string().size() - 3, 3) != "bin") {
            return false;
        }
        if (menu != m_menu_up_labels[0] && menu != m_menu_up_labels[1]) {
            Console::log("state::Edit::add_to_menu_up_list ", menu, " not implemented\n");
            return false;
        }
        std::string path_str = path.string();
        std::string separator_str;
        separator_str = (char)std::filesystem::path::preferred_separator;

        const std::string folder_name = menu == m_menu_up_labels[1] ? "prefab" : "level";        

        size_t res_path_pos = path_str.find("res" + separator_str, 0);
        if (res_path_pos == std::string::npos) {
            Console::log("state::Edit::add_to_menu_up_list res" + separator_str + " not found!\n");
            return false;
        }        
        path_str.erase(0, res_path_pos);    // keep last part of the path (res/level/...)        

        std::unique_ptr<BitmapText> bitmap_text = std::make_unique<BitmapText>();
        bitmap_text->transform_id = m_menu_up_lists[menu].transform_id;
        bitmap_text->layer = MENU_LAYER;
        bitmap_text->set_offset({ 0.0F, m_menu_up_lists[menu].items.size() * (F32)bitmap_text->font_size });
        bitmap_text->set_text(path_str);

        cU16 bg_w = path_str.size() * (int)bitmap_text->font_size;
        if (bg_w > m_menu_up_lists[menu].bg_w) {
            m_menu_up_lists[menu].bg_w = bg_w;
        }
        m_menu_up_lists[menu].bg_h = (int)(bitmap_text->font_size * (m_menu_up_lists[menu].items.size() + 1));
        sprite::Manager::get(m_menu_up_lists[menu].bg_sprite_id)->source_rect = { 0, 0, m_menu_up_lists[menu].bg_w, m_menu_up_lists[menu].bg_h };

        m_menu_up_lists[menu].items.emplace_back(std::move(bitmap_text));
        return true;
    }
}