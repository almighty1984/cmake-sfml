module;
#include <cmath>
#include <filesystem>
#include <sstream>

module state.edit;

import console;
import inputs;
import sprites;
import transforms;
import types;

namespace state {
    void Edit::update() {
        if (!is_all_valid()) {
            //Console::log("not all valid\n");
            return;
        }

        if (m_timer < 1) {
            m_timer += 1;
            return;
        }
        m_timer = 0;
        
        if (!m_is_moving && Inputs::at(m_input_id)->is_pressed(input::Key::ctrl) && Inputs::at(m_input_id)->is_pressed(input::Key::z)) {
            Inputs::at(m_input_id)->release(input::Key::z);
            undo_previous_act();
            if (!m_previous_acts.empty() && m_previous_acts.back() == PreviousAct::moved) {
                undo_previous_act();
            }
            return;
        }

        if (m_is_typing_current_level) {
            handle_typing_current_level();
        }
        if (m_is_asked_to_remove_level.first) {
            if (Inputs::at(m_input_id)->is_pressed(input::Key::y)) {
                Inputs::at(m_input_id)->release(input::Key::y);
                m_info_message.set_is_hidden(true);
                remove_level(m_menu_up_lists[m_menu_up_labels[0]].items.at(m_is_asked_to_remove_level.second).get()->get_text());
                m_menu_up_lists[m_menu_up_labels[0]].items.at(m_is_asked_to_remove_level.second).reset();
                m_menu_up_lists[m_menu_up_labels[0]].items.clear();
                load_menu_up_list(m_menu_up_labels[0], std::filesystem::current_path() / "res" / "levels");
                m_is_asked_to_remove_level = { false, 0 };
            } else if (Inputs::at(m_input_id)->is_pressed(input::Key::n)) {
                Inputs::at(m_input_id)->release(input::Key::n);
                m_info_message.set_is_hidden(true);
                m_is_asked_to_remove_level = { false, 0 };
            }
            return;
        }
        if (Inputs::at(m_input_id)->is_pressed(input::Key::ctrl)) {
            if (Inputs::at(m_input_id)->is_pressed(input::Key::s)) {
                Inputs::at(m_input_id)->release(input::Key::s);
                if (Inputs::at(m_input_id)->is_pressed(input::Key::shift)) {
                    init_typing_current_level();
                    m_is_typing_current_level = true;
                } else {
                    // suspect something might go wrong here
                    if (!m_is_typing_current_level) {                        
                        init_typing_current_level();
                        m_is_typing_current_level = true;
                    }
                    finish_typing_current_level_and_save();
                }
                return;
            }
        }

        //Console::log("current level typing pos: ", m_typing_pos_current_level, "\n");

        Vec2fc mouse_remainder_16 = { std::fmodf(Inputs::mouse.x, 16.0f), std::fmodf(Inputs::mouse.y, 16.0f) };
        m_mouse_tile_position = Inputs::mouse - mouse_remainder_16;
        Transforms::at(m_mouse_transform_id)->position = m_mouse_tile_position;
        //Console::log("mouse tile: ", m_mouse_tile_position.x, " ", m_mouse_tile_position.y, "\n");

        if (m_is_showing_grid_map && Inputs::at(m_input_id)->is_pressed(input::Key::ctrl)) {
            for (auto& i : m_grid_map_sprite_ids) {
                if (m_mouse_tile_position != Sprites::at(i)->offset) continue;
                if (Inputs::at(m_input_id)->is_pressed(input::Button::left)) {
                    Console::log("offset: ", m_mouse_tile_position.x, " ", m_mouse_tile_position.y, "\n");
                    move_level_to({ m_mouse_tile_position.x * -16.0f, m_mouse_tile_position.y * -16.0f});
                    return;
                }
            }
            if (Inputs::at(m_input_id)->is_pressed(input::Key::g)) {
                Inputs::at(m_input_id)->release(input::Key::g);
                Console::log("offset: ", m_mouse_tile_position.x, " ", m_mouse_tile_position.y, "\n");
                Vec2f grid_pos = m_mouse_tile_position * 16.0f;
                if (!add_grid_sprite_id_at_offset(grid_pos)) {
                    erase_grid_sprite_id_at_offset(grid_pos);
                }
                return;
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
        
        if (Inputs::at(m_input_id)->is_pressed(input::Key::num_0)) {
            Inputs::at(m_input_id)->release(input::Key::num_0);
            //clear_grid_sprites();
            //clear_mouse_sprites();


            //clear_selection_on_level();
            //clear_selection_on_tile_set();

            //m_mouse_sprite_ids.clear();
            //m_grid_sprite_ids.clear();

            //Console::log("state::Edit::update Sprites::erase_null...");
            Sprites::erase_null(m_level_transform_id);


            //init_grid();
            //init_mouse();

            //Console::log("done.\n");
            return;
        }
        
        m_is_mouse_on_menu_down = m_is_showing_menu_down && (m_mouse_tile_position.x >= 288 || m_mouse_tile_position.y >= 160) ? true : false;
        if (m_is_mouse_on_menu_down) {
            if (m_mouse_sprite_ids.size() != 1) {
                init_brush(1);
            }
        } else {
            if (m_mouse_sprite_ids.size() == 1) {
                m_is_showing_tile_set ? init_brush(m_tile_set_brush_size) : init_brush(m_level_brush_size);
            }
        }

        //Console::log("state::Edit::update is_mouse_on_level_list: ", m_is_mouse_on_menu_up_open_list, "\n");

        if (m_time_left_saving > 0) {
            Sprites::at(m_save_sprite_id)->source_rect.x = 16;
            --m_time_left_saving;
            if (m_time_left_saving <= 0) {
                Sprites::at(m_save_sprite_id)->source_rect.x = 0;
            }
        }

        if (Inputs::at(m_input_id)->is_pressed(input::Key::m)) {
            Inputs::at(m_input_id)->release(input::Key::m);
            m_is_showing_grid_map = !m_is_showing_grid_map;
            for (auto& i : m_grid_map_sprite_ids) {
                if (Sprites::at(i)) Sprites::at(i)->is_hidden = !m_is_showing_grid_map;
            }
            Sprites::at(m_position_on_grid_map_sprite_id)->is_hidden = !m_is_showing_grid_map;
        }

        if (Inputs::at(m_input_id)->is_pressed(input::Key::num_1)) {
            Inputs::at(m_input_id)->release(input::Key::num_1);
            m_is_showing_tile_set ?  m_tile_set_brush_size = 1 : m_level_brush_size = 1;
            init_brush(1);
        }
        if (Inputs::at(m_input_id)->is_pressed(input::Key::num_2)) {
            Inputs::at(m_input_id)->release(input::Key::num_2);
            m_is_showing_tile_set ? m_tile_set_brush_size = 2 : m_level_brush_size = 2;
            init_brush(2);
        }
        if (Inputs::at(m_input_id)->is_pressed(input::Key::num_3)) {
            Inputs::at(m_input_id)->release(input::Key::num_3);
            m_is_showing_tile_set ? m_tile_set_brush_size = 3 : m_level_brush_size = 3;
            init_brush(3);
        }
        if (Inputs::at(m_input_id)->is_pressed(input::Key::num_4)) {
            Inputs::at(m_input_id)->release(input::Key::num_4);
            m_is_showing_tile_set ? m_tile_set_brush_size = 4 : m_level_brush_size = 4;
            init_brush(4);
        }
        if (Inputs::at(m_input_id)->is_pressed(input::Key::num_5)) {
            Inputs::at(m_input_id)->release(input::Key::num_5);
            m_is_showing_tile_set ? m_tile_set_brush_size = 5 : m_level_brush_size = 5;
            init_brush(5);
        }

        if (Inputs::scroll < 0.0f) {
            if (m_is_showing_tile_set && m_tile_set_brush_size > 1) {
                m_tile_set_brush_size -= 1;
                init_brush(m_tile_set_brush_size);
            } else if (!m_is_showing_tile_set && m_level_brush_size > 1) {
                m_level_brush_size -= 1;
                init_brush(m_level_brush_size);
            }
        } else if (Inputs::scroll > 0.0f) {
            if (m_is_showing_tile_set && m_tile_set_brush_size < 5) {
                m_tile_set_brush_size += 1;
                init_brush(m_tile_set_brush_size);
            } else if (!m_is_showing_tile_set && m_level_brush_size < 5) {
                m_level_brush_size += 1;
                init_brush(m_level_brush_size);
            }
        }

        if (Inputs::at(m_input_id)->is_pressed(input::Key::space)) {
            Inputs::at(m_input_id)->release(input::Key::space);
            m_is_showing_tile_set = !m_is_showing_tile_set;
            if (m_is_showing_tile_set) {
                enter_tile_set();
            } else {
                exit_tile_set();
            }
            m_is_showing_tile_set ? init_brush(m_tile_set_brush_size) : init_brush(m_level_brush_size);
            Transforms::at(m_selection_on_tile_set_transform_id)->position = Transforms::at(m_tile_set_transform_id)->position;            
        }
        if (Inputs::at(m_input_id)->is_pressed(input::Key::alt)) {
            Inputs::at(m_input_id)->release(input::Key::alt);
            m_is_showing_menu_down = !m_is_showing_menu_down;
        }
        Transforms::at(m_menu_down_transform_id)->position = { 0.0f, 0.0f };
        if (m_is_showing_menu_down) {
            Transforms::at(m_menu_down_transform_id)->position = { -32.0f, -32.0f };
        }

                
        m_is_mouse_on_menu_up_bar = !Inputs::at(m_input_id)->is_pressed(input::Key::ctrl) &&
                                    !m_is_showing_tile_set &&
                                    Inputs::mouse.y > 0 && Inputs::mouse.y <= 8 &&
                                    Inputs::mouse.x <= m_menu_up_w ? true : false;

        if (m_is_mouse_on_menu_up_bar || m_is_mouse_on_menu_up_list) {
            Transforms::at(m_menu_up_transform_id)->position = { 0.0f, 0.0f };
            handle_menu_up();
            for (auto& i : m_mouse_sprite_ids) {
                Sprites::at(i)->is_hidden = true;
            }
        } else {
            Transforms::at(m_menu_up_transform_id)->position = { 0.0f,-16.0f };
            Transforms::at(m_menu_up_lists[m_menu_up_labels[0]].transform_id)->position = {0.0f, m_menu_up_lists[m_menu_up_labels[0]].bg_h * -1.0f};
            Transforms::at(m_menu_up_lists[m_menu_up_labels[1]].transform_id)->position = { 0.0f, m_menu_up_lists[m_menu_up_labels[1]].bg_h * -1.0f };
            for (auto& i : m_mouse_sprite_ids) {
                Sprites::at(i)->is_hidden = false;
            }
        }
        //Console::log("mouse pos: ", Inputs::mouse.x, " ", Inputs::mouse.y, "\n");
        if (Inputs::scroll < 0.0f) {
            Inputs::scroll = 0.0f;
        } else if (Inputs::scroll > 0.0f) {
            Inputs::scroll = 0.0f;
        }

        //Console::log("state::Edit::update tile_set position: ", Transforms::at(m_tile_set_transform_id)->position.x, " ", Transforms::at(m_tile_set_transform_id)->position.y, "\n");
        if (Inputs::at(m_input_id)->is_pressed(input::Key::a)) {
            if (!m_moving_sprite_ids.empty()) {
                Inputs::at(m_input_id)->release(input::Key::a);
                move_selected_on_level({ -16.0f, 0.0f });
                return;
            }
            if (Inputs::at(m_input_id)->is_pressed(input::Key::shift)) {
                Inputs::at(m_input_id)->release(input::Key::a);
                move_selection_on_tile_set({ -16, 0 });
            } else {
                if (m_is_showing_tile_set) {
                    move_tile_set({ 16, 0 });
                } else {
                    move_level({ 16, 0 });                    
                }
            }
        }
        if (Inputs::at(m_input_id)->is_pressed(input::Key::d)) {
            if (!m_moving_sprite_ids.empty()) {
                Inputs::at(m_input_id)->release(input::Key::d);
                move_selected_on_level({ 16.0f, 0.0f });
                return;
            }
            if (Inputs::at(m_input_id)->is_pressed(input::Key::shift)) {
                Inputs::at(m_input_id)->release(input::Key::d);
                move_selection_on_tile_set({ 16, 0 });
            } else {
                if (m_is_showing_tile_set) {
                    move_tile_set({ -16, 0 });
                } else {
                    move_level({ -16, 0 });
                }
            }
        }
        if (Inputs::at(m_input_id)->is_pressed(input::Key::w)) {
            if (!m_moving_sprite_ids.empty()) {
                Inputs::at(m_input_id)->release(input::Key::w);
                move_selected_on_level({ 0.0f, -16.0f });
                return;
            }
            if (Inputs::at(m_input_id)->is_pressed(input::Key::shift)) {
                Inputs::at(m_input_id)->release(input::Key::w);
                move_selection_on_tile_set({ 0,-16 });
            } else {
                if (m_is_showing_tile_set) {
                    move_tile_set({ 0, 16 });                    
                } else {
                    move_level({ 0, 16 });                    
                }
            }
        }
        if (Inputs::at(m_input_id)->is_pressed(input::Key::s)) {
            if (!m_moving_sprite_ids.empty()) {
                Inputs::at(m_input_id)->release(input::Key::s);
                move_selected_on_level({ 0.0f, 16.0f });
                return;
            }
            if (Inputs::at(m_input_id)->is_pressed(input::Key::shift)) {
                Inputs::at(m_input_id)->release(input::Key::s);
                move_selection_on_tile_set({ 0, 16 });
            } else {
                if (m_is_showing_tile_set) {
                    move_tile_set({ 0, -16 });
                } else {
                    move_level({ 0, -16 });                    
                }
            }
        }

        if (Inputs::at(m_input_id)->is_pressed(input::Key::grave)) {
            Inputs::at(m_input_id)->release(input::Key::grave);
            //clear_grid_sprites();
            clear_level_sprites();
            clear_typed_current_level();
        }
        if (Inputs::at(m_input_id)->is_pressed(input::Key::tab)) {
            Inputs::at(m_input_id)->release(input::Key::tab);
            if (Inputs::at(m_input_id)->is_pressed(input::Key::shift) && m_layer > 0) {
                Sprites::at(m_active_layer_sprite_id)->offset.y -= 16.0f;
                m_layer -= 1;
            } else if (!Inputs::at(m_input_id)->is_pressed(input::Key::shift) && m_layer < 9) {
                Sprites::at(m_active_layer_sprite_id)->offset.y += 16.0f;
                m_layer += 1;
            }
        }
        
        
        bool is_placing_on_level = false;
        if (Inputs::at(m_input_id)->is_pressed(input::Button::left)) {
            if (m_is_showing_tile_set && (!m_is_showing_menu_down || (m_mouse_tile_position.x < 288 && m_mouse_tile_position.y < 160))) {
                if (!Inputs::at(m_input_id)->is_pressed(input::Key::ctrl)) {
                    deselect_all_on_tile_set();
                }
                for (auto& mouse_id : m_mouse_sprite_ids) {
                    if (!Sprites::at(mouse_id)) continue;
                    Vec2fc offset = Transforms::at(m_mouse_transform_id)->position -
                                    Transforms::at(m_tile_set_transform_id)->position +
                                    Sprites::at(mouse_id)->offset;
                    select_on_tile_set(offset);
                }
            } else if (m_is_showing_menu_down && (m_mouse_tile_position.x >= 288 || m_mouse_tile_position.y >= 160)) {                
                handle_menu_down(m_mouse_tile_position, input::Button::left);
                return;
            } else {
                //Inputs::at(m_input_id)->release(input::Button::left);

                if (Inputs::at(m_input_id)->is_pressed(input::Key::ctrl)) {
                    for (auto& mouse_id : m_mouse_sprite_ids) {
                        if (!Sprites::at(mouse_id)) continue;
                        Vec2fc offset = Transforms::at(m_mouse_transform_id)->position -
                                        Transforms::at(m_level_transform_id)->position +
                                        Sprites::at(mouse_id)->offset;
                        select_on_level(offset);
                    }
                } else {
                    deselect_all_on_level();

                    is_placing_on_level = true;
                    for (auto& sel_id : m_selection_on_tile_set_sprite_ids) {
                        size_t first_sel_id = m_selection_on_tile_set_sprite_ids.front();
                        Rectic source_rect  = { (i16)Sprites::at(sel_id)->offset.x,
                                                (i16)Sprites::at(sel_id)->offset.y,
                                                16, 16 };
                        
                        u16 num_placed = 0;
                        for (auto& mouse_id : m_mouse_sprite_ids) {
                            Vec2fc offset = Sprites::at(sel_id)->offset + Sprites::at(mouse_id)->offset -
                                            Sprites::at(first_sel_id)->offset +
                                            Transforms::at(m_mouse_transform_id)->position -
                                            Transforms::at(m_level_transform_id)->position;

                            PreviousAct prev_act = place_sprite_on_level(m_layer, m_tile_set, source_rect, offset);
                            if (prev_act != PreviousAct::null) {
                                m_previous_acts.push_back(prev_act);
                                ++num_placed;
                            }
                        }
                        m_last_undo_count += num_placed;
                    }
                }                
            }
        }
       
        if (Inputs::at(m_input_id)->is_pressed(input::Button::right)) {
            if (m_is_showing_tile_set && (!m_is_showing_menu_down || (m_mouse_tile_position.x < 288 && m_mouse_tile_position.y < 160))) {
                for (auto& mouse_sprite_id : m_mouse_sprite_ids) {
                    Vec2f offset = Transforms::at(m_mouse_transform_id)->position -
                                   Transforms::at(m_tile_set_transform_id)->position +
                                   Sprites::at(mouse_sprite_id)->offset;
                    deselect_on_tile_set(offset);
                }
            } else if (m_is_showing_menu_down && (m_mouse_tile_position.x >= 288 || m_mouse_tile_position.y >= 160)) {
                handle_menu_down(m_mouse_tile_position, input::Button::right);
            }
        }

        bool is_erasing_on_level = false;
        if (Inputs::at(m_input_id)->is_pressed(input::Button::right)) {
            is_erasing_on_level = true;
            u16 num_erased = 0;
            for (auto& mouse_sprite_id : m_mouse_sprite_ids) {
                Vec2f offset = Transforms::at(m_mouse_transform_id)->position -
                               Transforms::at(m_level_transform_id)->position +
                               Sprites::at(mouse_sprite_id)->offset;
                if (Inputs::at(m_input_id)->is_pressed(input::Key::ctrl)) {                    
                    deselect_on_level(offset);                    
                } else {
                    deselect_all_on_level();
                    PreviousAct prev_act = erase_sprite_on_level(m_layer, offset);
                    if (prev_act != PreviousAct::null) {
                        m_previous_acts.push_back(prev_act);
                        ++num_erased;
                    }
                }
            }
            m_last_undo_count += num_erased;
        }
        if (!is_placing_on_level && !is_erasing_on_level) {
            if (m_last_undo_count > 0) {
                m_undo_counts.push_back(m_last_undo_count);
                m_last_undo_count = 0;
            }
        }

        if (Inputs::at(m_input_id)->is_pressed(input::Button::middle)) {
            Inputs::at(m_input_id)->release(input::Button::middle);
            Vec2fc offset = Transforms::at(m_mouse_transform_id)->position - Transforms::at(m_level_transform_id)->position;
            level_eyedropper_at_offset(offset);
        }

        if (Inputs::at(m_input_id)->is_pressed(input::Key::h)) {
            Inputs::at(m_input_id)->release(input::Key::h);
            for (auto& i : m_grid_sprite_ids) {
                Sprites::at(i)->is_hidden = !Sprites::at(i)->is_hidden;
            }
        }        

        if (Inputs::at(m_input_id)->is_pressed(input::Key::del)) {
            Inputs::at(m_input_id)->release(input::Key::del);

            if (!m_selection_on_level_sprite_ids.empty()) {
                clear_selected_on_level();
                return;
            }
        }
        if (Inputs::at(m_input_id)->is_pressed(input::Key::c)) {
            Inputs::at(m_input_id)->release(input::Key::c);
            if (!m_selection_on_level_sprite_ids.empty()) {
                if (!m_is_moving) {
                    m_is_moving = true;
                    copy_selected_start_moving_on_level();
                } else {
                    finish_moving_selected_on_level();
                    copy_selected_start_moving_on_level();
                }
            }
        }
        if (Inputs::at(m_input_id)->is_pressed(input::Key::x)) {
            Inputs::at(m_input_id)->release(input::Key::x);
            if (!m_selection_on_level_sprite_ids.empty()) {
                m_is_moving ? finish_moving_selected_on_level() : start_moving_selected_on_level();
                m_is_moving = !m_is_moving;
            }
        }
        if (Inputs::at(m_input_id)->is_pressed(input::Key::esc)) {
            Inputs::at(m_input_id)->release(input::Key::esc);  
            if (m_is_typing_current_level) {                
                quit_typing_current_level();
                return;
            }
            if (m_is_moving) {
                m_is_moving = false;
                finish_moving_selected_on_level();
                if (!m_selection_on_level_sprite_ids.empty()) {
                    deselect_all_on_level();
                    return;
                }
            } else {
                if (!m_selection_on_level_sprite_ids.empty()) {
                    deselect_all_on_level();
                    return;
                }
                if (!m_selection_on_tile_set_sprite_ids.empty()) {
                    deselect_all_on_tile_set();
                    return;
                }
            }            
        }
        if (Inputs::at(m_input_id)->is_pressed(input::Key::f1)) {
            Inputs::at(m_input_id)->release(input::Key::f1);
            set_next(Type::Game);
        }

    }
}