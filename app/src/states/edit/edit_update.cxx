module;
#include <cmath>
#include <sstream>

module state.edit;

import console;
import inputs;
import sprites;
import transforms;
import types;

namespace state {
    void Edit::update() {
        if (!is_all_valid()) return;

        if (m_timer < 1) {
            m_timer += 1;
            return;
        }
        m_timer = 0;
        //Console::log("state::Edit::update current_fps: ", current_fps, "\n");

        //Console::log("list size: ", m_level_list.size(), "\n");

        //Console::log(std::filesystem::current_path(), "\n");

        Vec2fc remainder = { std::fmodf(Inputs::mouse.x, 16.0f), std::fmodf(Inputs::mouse.y, 16.0f) };
        m_mouse_tile_position = Inputs::mouse - remainder;
        Transforms::at(m_mouse_transform_id)->position = m_mouse_tile_position;
                
        if (m_mouse_tile_position.x >= 288 || m_mouse_tile_position.y >= 160) {
            //Console::log("state::Edit::update mouse tile position: ", m_mouse_tile_position.x, " ", m_mouse_tile_position.y, "\n");
            if (m_mouse_sprite_ids.size() != 1 && !m_is_mouse_on_menu_down) {
                init_brush(1);
            }
            m_is_mouse_on_menu_down = true;
        } else {
            if (m_mouse_sprite_ids.size() == 1) {
                m_is_showing_tile_set ? init_brush(m_set_brush_size) : init_brush(m_level_brush_size);                
            }
            m_is_mouse_on_menu_down = false;
        }        

        m_is_mouse_on_menu_up = false;
        if (m_is_showing_menu_up && m_mouse_tile_position.y < Transforms::at(m_menu_up_transform_id)->position.y + Sprites::at(m_menu_up_bg_sprite_id)->source_rect.h &&
            m_mouse_tile_position.x < Transforms::at(m_menu_up_transform_id)->position.x + Sprites::at(m_menu_up_bg_sprite_id)->source_rect.w) {
            m_is_mouse_on_menu_up = true;
        }

        if (m_time_left_saving > 0) {
            Sprites::at(m_save_sprite_id)->source_rect.x = 16;
            --m_time_left_saving;
            if (m_time_left_saving <= 0) {
                Sprites::at(m_save_sprite_id)->source_rect.x = 0;
            }
        }
        if (m_time_left_opening > 0) {
            Sprites::at(m_open_sprite_id)->source_rect.x = 16;
            --m_time_left_opening;
            if (m_time_left_opening <= 0) {
                Sprites::at(m_open_sprite_id)->source_rect.x = 0;
            }
        }

        if (Inputs::at(m_input_id)->is_pressed(input::Key::m)) {
            Inputs::at(m_input_id)->release(input::Key::m);
            m_is_showing_map = !m_is_showing_map;
            for (auto& i : m_grid_map_sprite_ids) {
                if (Sprites::at(i)) Sprites::at(i)->is_hidden = !m_is_showing_map;
            }
        }

        if (Inputs::at(m_input_id)->is_pressed(input::Key::num_0)) {
            Inputs::at(m_input_id)->release(input::Key::num_0);
            Console::log("state::Edit::update Sprites::erase_null...");
            Sprites::erase_null();
            Console::log("done.\n");
            return;
        }
        if (Inputs::at(m_input_id)->is_pressed(input::Key::num_1)) {
            Inputs::at(m_input_id)->release(input::Key::num_1);
            if (m_is_showing_tile_set) m_set_brush_size = 1;
            else                       m_level_brush_size = 1;
            init_brush(1);
        }
        if (Inputs::at(m_input_id)->is_pressed(input::Key::num_2)) {
            Inputs::at(m_input_id)->release(input::Key::num_2);
            if (m_is_showing_tile_set) m_set_brush_size = 3;
            else                       m_level_brush_size = 3;
            init_brush(3);
        }

        if (Inputs::at(m_input_id)->is_pressed(input::Key::space)) {
            Inputs::at(m_input_id)->release(input::Key::space);
            m_is_showing_tile_set = !m_is_showing_tile_set;
            if (m_is_showing_tile_set) {
                Transforms::at(m_tile_set_bg_transform_id)->position.y = 0.0f;
                Transforms::at(m_tile_set_transform_id)->position.y -= 512.0f;
            } else {                
                Transforms::at(m_tile_set_bg_transform_id)->position.y = 512.0f;
                Transforms::at(m_tile_set_transform_id)->position.y += 512.0f;
            }            
            m_is_showing_tile_set ? init_brush(m_set_brush_size) : init_brush(m_level_brush_size);

            Transforms::at(m_selection_on_set_transform_id)->position = Transforms::at(m_tile_set_transform_id)->position;
            //Transforms::at(m_selection_on_set_transform_id)->position.y - Transforms::at(m_tile_set_transform_id)->position.y;
        }

        if (Inputs::at(m_input_id)->is_pressed(input::Key::alt)) {
            Inputs::at(m_input_id)->release(input::Key::alt);
            m_is_showing_menu_down = !m_is_showing_menu_down;
        }
        Transforms::at(m_menu_down_transform_id)->position = { 0.0f, 0.0f };
        if (m_is_showing_menu_down) {
            Transforms::at(m_menu_down_transform_id)->position = { -32.0f, -32.0f };
        }


        if (Inputs::at(m_input_id)->is_pressed(input::Key::f1)) {
            Inputs::at(m_input_id)->release(input::Key::f1);
            m_is_showing_menu_up = !m_is_showing_menu_up;
        }
        Transforms::at(m_menu_up_transform_id)->position = { 0.0f,-16.0f };
        if (m_is_showing_menu_up) {
            Transforms::at(m_menu_up_transform_id)->position = { 0.0f, 0.0f };
        }


        if (m_is_showing_menu_up) {
            if (m_is_mouse_on_menu_up) {
                const size_t menu_y = (size_t)(Inputs::mouse.y - std::fmodf(Inputs::mouse.y, 8.0f)) / 8;
                if (menu_y < m_level_list.size()) {
                    for (size_t i = 0; i < m_level_list.size(); ++i) {
                        if (i != menu_y) m_level_list.at(i).get()->texture("res/fonts/font_8_gray.png");
                    }
                    m_level_list.at(menu_y).get()->texture("res/fonts/font_8_white.png");

                    for (auto& i : m_mouse_sprite_ids) {
                        Sprites::at(i)->is_hidden = true;
                    }
                    if (Inputs::at(m_input_id)->is_pressed(input::Button::left)) {
                        Inputs::at(m_input_id)->release(input::Button::left);
                        Console::log("state::Edit::update menu up clicked on ", menu_y, " ", m_level_list.at(menu_y).get()->get_text(), "\n");

                        m_level_path = m_level_list.at(menu_y).get()->get_text();

                        load_level_sprites_from(m_level_path);
                    }
                }
            } else {
                for (auto& i : m_level_list) {
                    i.get()->texture("res/fonts/font_8_gray.png");
                }
                for (auto& i : m_mouse_sprite_ids) {
                    Sprites::at(i)->is_hidden = false;
                }
            }
        }

        if (Inputs::scroll < 0.0f) {
            Inputs::scroll = 0.0f;
        } else if (Inputs::scroll > 0.0f) {
            Inputs::scroll = 0.0f;
        }

        //Console::log("state::Edit::update tile_set position: ", Transforms::at(m_tile_set_transform_id)->position.x, " ", Transforms::at(m_tile_set_transform_id)->position.y, "\n");
        if (Inputs::at(m_input_id)->is_pressed(input::Key::a)) {
            if (!m_selection_on_level_sprite_ids.empty()) {
                Inputs::at(m_input_id)->release(input::Key::a);
                move_selected_on_level({ -16.0f, 0.0f });
                return;
            }
            if (Inputs::at(m_input_id)->is_pressed(input::Key::ctrl)) {
                Inputs::at(m_input_id)->release(input::Key::a);
                bool is_out_of_bounds = false;
                for (auto& i : m_selection_on_set_sprite_ids) {
                    if (Sprites::at(i)->offset.x < 16) {
                        is_out_of_bounds = true;
                        break;
                    }
                }
                if (!is_out_of_bounds) {
                    for (auto& i : m_selection_on_set_sprite_ids) {
                        Sprites::at(i)->offset.x -= 16;
                    }
                }
            } else {
                if (m_is_showing_tile_set) {
                    if (Transforms::at(m_tile_set_transform_id)->position.x < 0.0f) {
                        Transforms::at(m_tile_set_transform_id)->position.x += 16;
                        Transforms::at(m_selection_on_set_transform_id)->position.x += 16;
                    }
                } else {
                    Transforms::at(m_level_transform_id)->position.x += 16;
                    Transforms::at(m_grid_transform_id)->position.x += 16;
                }
            }
        }
        if (Inputs::at(m_input_id)->is_pressed(input::Key::d)) {
            if (!m_selection_on_level_sprite_ids.empty()) {
                Inputs::at(m_input_id)->release(input::Key::d);
                move_selected_on_level({ 16.0f, 0.0f });
                return;
            }
            if (Inputs::at(m_input_id)->is_pressed(input::Key::ctrl)) {
                Inputs::at(m_input_id)->release(input::Key::d);
                bool is_out_of_bounds = false;
                for (auto& i : m_selection_on_set_sprite_ids) {
                    if (Sprites::at(i)->offset.x > 480) {
                        is_out_of_bounds = true;
                        break;
                    }
                }
                if (!is_out_of_bounds) {
                    for (auto& i : m_selection_on_set_sprite_ids) {
                        Sprites::at(i)->offset.x += 16;
                    }
                }
            } else {
                if (m_is_showing_tile_set) {
                    if (Transforms::at(m_tile_set_transform_id)->position.x > -192) {
                        Transforms::at(m_tile_set_transform_id)->position.x -= 16;
                        Transforms::at(m_selection_on_set_transform_id)->position.x -= 16;
                    }
                } else {
                    Transforms::at(m_level_transform_id)->position.x -= 16;
                    Transforms::at(m_grid_transform_id)->position.x -= 16;
                }
            }
        }
        if (Inputs::at(m_input_id)->is_pressed(input::Key::w)) {
            if (!m_selection_on_level_sprite_ids.empty()) {
                Inputs::at(m_input_id)->release(input::Key::w);
                move_selected_on_level({ 0.0f, -16.0f });
                return;
            }
            if (Inputs::at(m_input_id)->is_pressed(input::Key::ctrl)) {
                Inputs::at(m_input_id)->release(input::Key::w);
                bool is_out_of_bounds = false;
                for (auto& i : m_selection_on_set_sprite_ids) {
                    if (Sprites::at(i)->offset.y < 0) {
                        is_out_of_bounds = true;
                        break;
                    }
                }
                if (!is_out_of_bounds) {
                    for (auto& i : m_selection_on_set_sprite_ids) {
                        Sprites::at(i)->offset.y -= 16;
                    }
                }
            } else {
                if (m_is_showing_tile_set) {
                    if (Transforms::at(m_tile_set_transform_id)->position.y < 0) {
                        Transforms::at(m_tile_set_transform_id)->position.y += 16;
                        Transforms::at(m_selection_on_set_transform_id)->position.y += 16;
                    }
                } else {
                    Transforms::at(m_level_transform_id)->position.y += 16;
                    Transforms::at(m_grid_transform_id)->position.y += 16;
                }
            }
        }
        if (Inputs::at(m_input_id)->is_pressed(input::Key::s)) {
            if (!m_selection_on_level_sprite_ids.empty()) {
                Inputs::at(m_input_id)->release(input::Key::s);
                move_selected_on_level({ 0.0f, 16.0f });
                return;
            }
            if (Inputs::at(m_input_id)->is_pressed(input::Key::ctrl)) {
                Inputs::at(m_input_id)->release(input::Key::s);
                bool is_out_of_bounds = false;
                for (auto& i : m_selection_on_set_sprite_ids) {
                    if (Sprites::at(i)->offset.y > 480) {
                        is_out_of_bounds = true;
                        break;
                    }
                }
                if (!is_out_of_bounds) {
                    for (auto& i : m_selection_on_set_sprite_ids) {
                        Sprites::at(i)->offset.y += 16;
                    }
                }
            } else {
                if (m_is_showing_tile_set) {
                    if (Transforms::at(m_tile_set_transform_id)->position.y > -336) {
                        Transforms::at(m_tile_set_transform_id)->position.y -= 16;
                        Transforms::at(m_selection_on_set_transform_id)->position.y -= 16;
                    }
                } else {
                    Transforms::at(m_level_transform_id)->position.y -= 16;
                    Transforms::at(m_grid_transform_id)->position.y -= 16;
                }
            }
        }


        if (Inputs::at(m_input_id)->is_pressed(input::Key::grave)) {
            Inputs::at(m_input_id)->release(input::Key::grave);
            if (m_layer > 0) {
                Sprites::at(m_active_layer_sprite_id)->offset.y -= 16.0f;
                m_layer -= 1;
            }
        }
        if (Inputs::at(m_input_id)->is_pressed(input::Key::tab)) {
            Inputs::at(m_input_id)->release(input::Key::tab);
            if (m_layer < 9) {
                Sprites::at(m_active_layer_sprite_id)->offset.y += 16.0f;
                m_layer += 1;
            }
        }

        if (Inputs::at(m_input_id)->is_pressed(input::Button::left)) {
            if (m_is_showing_tile_set && (!m_is_showing_menu_down || (m_mouse_tile_position.x < 288 && m_mouse_tile_position.y < 160))) {
                select_on_set(m_mouse_tile_position);
            } else if (m_is_showing_menu_down && (m_mouse_tile_position.x >= 288 || m_mouse_tile_position.y >= 160)) {
                Inputs::at(m_input_id)->release(input::Button::left);
                handle_menu_click(m_mouse_tile_position);
                return;
            } else {
                Vec2f offset = Transforms::at(m_mouse_transform_id)->position - Transforms::at(m_level_transform_id)->position;
                Console::log("state::Edit::update offset: ", offset.x, " ", offset.y, "\n");
                
                for (auto& mouse_sprite_id : m_mouse_sprite_ids) {
                    if (Inputs::at(m_input_id)->is_pressed(input::Key::ctrl)) {
                        select_on_level(offset + Sprites::at(mouse_sprite_id)->offset);
                    } else {
                        clear_selection_on_level();
                        place_sprite_on_level_at_offset(offset + Sprites::at(mouse_sprite_id)->offset);
                    }
                }
            }
        }
        if (Inputs::at(m_input_id)->is_pressed(input::Button::right)) {            
            for (auto& mouse_sprite_id : m_mouse_sprite_ids) {
                Vec2f offset = Transforms::at(m_mouse_transform_id)->position - Transforms::at(m_level_transform_id)->position;
                if (Inputs::at(m_input_id)->is_pressed(input::Key::ctrl)) {
                    deselect_on_level(offset + Sprites::at(mouse_sprite_id)->offset);
                } else {
                    clear_selection_on_level();
                    erase_sprite_on_level_at_offset(offset + Sprites::at(mouse_sprite_id)->offset);
                }
            }            
        }
        if (Inputs::at(m_input_id)->is_pressed(input::Button::middle)) {
            Inputs::at(m_input_id)->release(input::Button::middle);

            size_t sprite_id = level_sprite_id_at_offset(m_layer, Transforms::at(m_mouse_transform_id)->position);

            Vec2fc position = Transforms::at(m_mouse_transform_id)->position - Transforms::at(m_level_transform_id)->position;
            Vec2fc offset = level_sprite_source_at_offset(m_layer, position);

            for (auto& i : m_selection_on_set_sprite_ids) {
                Sprites::erase(i);
            }
            m_selection_on_set_sprite_ids.clear();
            m_selection_on_set_sprite_ids.push_back(Sprites::make("res/tiles/editor_selection.png"));
            Sprites::at(m_selection_on_set_sprite_ids.back())->layer = SELECTION_ON_SET_LAYER;
            Sprites::at(m_selection_on_set_sprite_ids.back())->transform_id = m_selection_on_set_transform_id;
            Sprites::at(m_selection_on_set_sprite_ids.back())->offset = offset;
        }

        if (Inputs::at(m_input_id)->is_pressed(input::Key::h)) {
            Inputs::at(m_input_id)->release(input::Key::h);
            for (auto& i : m_grid_sprite_ids) {
                Sprites::at(i)->is_hidden = !Sprites::at(i)->is_hidden;
            }
        }

        if (Inputs::at(m_input_id)->is_pressed(input::Key::g)) {
            Inputs::at(m_input_id)->release(input::Key::g);
            Vec2f mouse_offset = m_mouse_tile_position - Transforms::at(m_grid_transform_id)->position;
            Vec2f grid_remainder = { std::fmodf(mouse_offset.x, 256.0f), std::fmodf(mouse_offset.y, 256.0f) };
            Vec2f grid_pos = mouse_offset - grid_remainder;
            //Console::log("state::Edit::update grid_pos: ", grid_pos.x, " ", grid_pos.y, "\n");
            if (!add_grid_sprite_id_at_offset(grid_pos)) {
                erase_grid_sprite_id_at_offset(grid_pos);
            }
            //expand_grid_from(Sprites::at(m_grid_sprite_ids.back())->offset, Direction::right);
        }

        if (Inputs::at(m_input_id)->is_pressed(input::Key::del)) {
            Inputs::at(m_input_id)->release(input::Key::del);

            if (!m_selection_on_level_sprite_ids.empty()) {
                act_on_selection_on_level(SelectionAct::clear);
                return;
            }
        }
        if (Inputs::at(m_input_id)->is_pressed(input::Key::c)) {
            Inputs::at(m_input_id)->release(input::Key::c);
            if (!m_selection_on_level_sprite_ids.empty()) {
                if (!m_is_moving) {
                    m_is_moving = true;
                    act_on_selection_on_level(SelectionAct::copy);
                } else {
                    finish_moving_on_level();
                    act_on_selection_on_level(SelectionAct::copy);
                }
            }
        }
        if (Inputs::at(m_input_id)->is_pressed(input::Key::x)) {
            Inputs::at(m_input_id)->release(input::Key::x);
            if (!m_selection_on_level_sprite_ids.empty()) {
                if (!m_is_moving) {
                    m_is_moving = true;
                    act_on_selection_on_level(SelectionAct::move);
                }
            }
        }

        if (Inputs::at(m_input_id)->is_pressed(input::Key::esc)) {
            Inputs::at(m_input_id)->release(input::Key::esc);
            if (m_is_showing_menu_up) {
                m_is_showing_menu_up = false;
                return;
            }
            if (m_is_moving) {
                m_is_moving = false;
                finish_moving_on_level();
                if (!m_selection_on_level_sprite_ids.empty()) {
                    clear_selection_on_level();
                    return;
                }
            } else {
                if (!m_selection_on_level_sprite_ids.empty()) {
                    clear_selection_on_level();
                    return;
                }
                if (!m_selection_on_set_sprite_ids.empty()) {
                    clear_selection_on_set();
                    return;
                }
            }

            //set_next(Type::GameInit);
        }

    }
}