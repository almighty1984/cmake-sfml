module;
#include <cmath>
#include <filesystem>
#include <sstream>

module state.edit;

import console;
import input;
import sprite;
import transform;
import types;

namespace state {
    void Edit::update(cF32 ts) {
        if (!is_all_valid()) {
            //Console::log("not all valid\n");
            return;
        }        
        if (input()->is_pressed(input::Key::f1)) {
            input()->release(input::Key::f1);            
            is_to_transition = true;
            m_start_info = { .type = start::Type::center,
                             .number = 0 };
            m_next_state = state::Type::game;
            return;
        }

        /*if (m_timer < 1) {
            m_timer += 1;
            return;
        }
        m_timer = 0;*/

        sprite::Manager::get(m_current_tile_sprite_id)->offset.x = view().w - 32.0F;
        sprite::Manager::get(m_grid_icon_sprite_id)->offset.x = view().w - 16.0f;

        if (input()->is_pressed(input::Key::grave)) {
            input()->release(input::Key::grave);
            is_to_change_view = true;
            if (view() == RectF{ 0.0F, 0.0F, 480.0F, 270.0F }) {
                view({ 0.0F, 0.0F, 320.0F, 180.0F });
                //move_selection_on_tile_set({})
            } else {
                view({ 0.0F, 0.0F, 480.0F, 270.0F });
            }
            
            //if (m_is_showing_tile_set) {
            //    //transform::Manager::get(m_tile_set_transform_id)->position = { 0.0F, 0.0F };

            //} else {
            //    transform::Manager::get(m_tile_set_transform_id)->position.x = view().w - 32.0F;
            //    transform::Manager::get(m_tile_set_transform_id)->position.y = view().w == 480.0F ? 240.0F : 160.0F;
            //}
            //move_tile_set({ 0.0F, 0.0F });            
        }

        //Console::log("view: ", view().w, " ", view().h, "\n");

        transform::Manager::get(m_menu_right_transform_id)->position.x = view().w - sprite::Manager::get(m_menu_right_bg_sprite_id)->source_rect.w;
        transform::Manager::get(m_menu_down_transform_id)->position.y = view().h + 16.0F - std::fmodf(view().h, 16.0F) - sprite::Manager::get(m_menu_down_bg_sprite_id)->source_rect.h;

        if (input()->is_pressed(input::Key::enter)) {
            input()->release(input::Key::enter);
            m_is_hidden_grid = !m_is_hidden_grid;
            for (auto& i : m_grid_sprite_ids) {
                if (sprite::Manager::get(i)) {
                    sprite::Manager::get(i)->is_hidden = m_is_hidden_grid;
                }
            }
        }

        if (!m_is_moving && input()->is_pressed(input::Key::ctrl) && input()->is_pressed(input::Key::z)) {
            input()->release(input::Key::z);
            undo_last_act();
            if (!m_undo_acts.empty() && m_undo_acts.back() == undo::Act::moved) {
                undo_last_act();
            }
            return;
        }
        if (m_is_typing_text_bar) {
            handle_typing_text_bar();
        }
        if (m_is_asked_to_remove_level.first) {
            if (input()->is_pressed(input::Key::y)) {
                input()->release(input::Key::y);
                m_info_message.set_is_hidden(true);
                remove_level(m_menu_up_lists[m_menu_up_labels[0]].items.at(m_is_asked_to_remove_level.second).get()->get_text());
                m_menu_up_lists[m_menu_up_labels[0]].items.at(m_is_asked_to_remove_level.second).reset();
                m_menu_up_lists[m_menu_up_labels[0]].items.clear();
                load_menu_up_list(m_menu_up_labels[0], std::filesystem::current_path() / "res" / "level");
                m_is_asked_to_remove_level = { false, 0 };
            } else if (input()->is_pressed(input::Key::n)) {
                input()->release(input::Key::n);
                m_info_message.set_is_hidden(true);
                m_is_asked_to_remove_level = { false, 0 };
            }
            return;
        }
        if (input()->is_pressed(input::Key::ctrl)) {
            if (input()->is_pressed(input::Key::s)) {
                input()->release(input::Key::s);
                if (input()->is_pressed(input::Key::shift)) {
                    init_typing_text_bar();
                    m_is_typing_text_bar = true;
                } else {
                    if (m_is_typing_text_bar) {                        
                        quit_typing_text_bar();
                        m_is_typing_text_bar = false;
                    }
                    save_typed_text_bar();
                }
                return;
            }
        }

        cVec2F mouse_remainder_16 = { std::fmodf(input::Manager::mouse.x, 16.0F), std::fmodf(input::Manager::mouse.y, 16.0F) };
        m_mouse_tile_position = input::Manager::mouse - mouse_remainder_16;
        transform::Manager::get(m_mouse_transform_id)->position = m_mouse_tile_position;
        //Console::log("mouse tile: ", m_mouse_tile_position.x, " ", m_mouse_tile_position.y, "\n");

        if (!m_is_hidden_grid_map && input()->is_pressed(input::Key::ctrl)) {
            for (auto& i : m_grid_map_sprite_ids) {
                if (m_mouse_tile_position != sprite::Manager::get(i)->offset) continue;
                if (input()->is_pressed(input::Button::left)) {
                    //Console::log("offset: ", m_mouse_tile_position.x, " ", m_mouse_tile_position.y, "\n");
                    move_level_to({ m_mouse_tile_position.x * -16.0F, m_mouse_tile_position.y * -16.0F});
                    return;
                }
            }
            if (input()->is_pressed(input::Key::g)) {
                input()->release(input::Key::g);
                //Console::log("offset: ", m_mouse_tile_position.x, " ", m_mouse_tile_position.y, "\n");
                Vec2F grid_pos = m_mouse_tile_position * 16.0F;
                if (!add_grid_at_offset(grid_pos)) {
                    erase_grid_at_offset(grid_pos);
                }
                return;
            }
        }
        if (input()->is_pressed(input::Key::g)) {
            input()->release(input::Key::g);

            Vec2F mouse_offset = m_mouse_tile_position - transform::Manager::get(m_grid_transform_id)->position;
            Vec2F grid_remainder = { std::fmodf(mouse_offset.x, 256.0F), std::fmodf(mouse_offset.y, 256.0F) };
            Vec2F grid_pos = mouse_offset - grid_remainder;
            //Console::log("state::Edit::update grid_pos: ", grid_pos.x, " ", grid_pos.y, "\n");
            if (!add_grid_at_offset(grid_pos)) {
                erase_grid_at_offset(grid_pos);
            }
        }
        
        if (input()->is_pressed(input::Key::num_0)) {
            input()->release(input::Key::num_0);
            //Console::log("state::Edit::update sprite::Manager::erase_null...");
            sprite::Manager::erase_null(m_level_transform_id);
            return;
        }
        
        m_is_mouse_on_menu_down = !m_is_hidden_menu_down && (m_mouse_tile_position.x > view().w - 48.0F || m_mouse_tile_position.y > view().h - 32.0F) ? true : false;
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
            sprite::Manager::get(m_save_sprite_id)->source_rect.x = 16;
            --m_time_left_saving;
            if (m_time_left_saving == 0) {
                sprite::Manager::get(m_save_sprite_id)->source_rect.x = 0;
            }
        }

        if (input()->is_pressed(input::Key::m)) {
            input()->release(input::Key::m);
            m_is_hidden_grid_map = !m_is_hidden_grid_map;
            for (auto& i : m_grid_map_sprite_ids) {
                if (sprite::Manager::get(i)) sprite::Manager::get(i)->is_hidden = !m_is_hidden_grid_map;
            }
            sprite::Manager::get(m_position_on_grid_map_sprite_id)->is_hidden = !m_is_hidden_grid_map;
        }

        if (input()->is_pressed(input::Key::num_1)) {
            input()->release(input::Key::num_1);
            m_is_showing_tile_set ?  m_tile_set_brush_size = 1 : m_level_brush_size = 1;
            init_brush(1);
        }
        if (input()->is_pressed(input::Key::num_2)) {
            input()->release(input::Key::num_2);
            m_is_showing_tile_set ? m_tile_set_brush_size = 2 : m_level_brush_size = 2;
            init_brush(2);
        }
        if (input()->is_pressed(input::Key::num_3)) {
            input()->release(input::Key::num_3);
            m_is_showing_tile_set ? m_tile_set_brush_size = 3 : m_level_brush_size = 3;
            init_brush(3);
        }
        if (input()->is_pressed(input::Key::num_4)) {
            input()->release(input::Key::num_4);
            m_is_showing_tile_set ? m_tile_set_brush_size = 4 : m_level_brush_size = 4;
            init_brush(4);
        }
        if (input()->is_pressed(input::Key::num_5)) {
            input()->release(input::Key::num_5);
            m_is_showing_tile_set ? m_tile_set_brush_size = 5 : m_level_brush_size = 5;
            init_brush(5);
        }

        if (input::Manager::scroll < 0.0F) {
            input::Manager::scroll = 0.0F;
            if (m_is_showing_tile_set && m_tile_set_brush_size > 1) {
                m_tile_set_brush_size -= 1;
                init_brush(m_tile_set_brush_size);
            } else if (!m_is_showing_tile_set && m_level_brush_size > 1) {
                m_level_brush_size -= 1;
                init_brush(m_level_brush_size);
            }
        } else if (input::Manager::scroll > 0.0F) {
            input::Manager::scroll = 0.0F;
            if (m_is_showing_tile_set && m_tile_set_brush_size < 5) {
                m_tile_set_brush_size += 1;
                init_brush(m_tile_set_brush_size);
            } else if (!m_is_showing_tile_set && m_level_brush_size < 5) {
                m_level_brush_size += 1;
                init_brush(m_level_brush_size);
            }
        }        
        if (input()->is_pressed(input::Key::space)) {
            input()->release(input::Key::space);
            m_is_showing_tile_set = !m_is_showing_tile_set;
            if (m_is_showing_tile_set) {
                enter_tile_set();
            } else {
                exit_tile_set();
            }
            m_is_showing_tile_set ? init_brush(m_tile_set_brush_size) : init_brush(m_level_brush_size);
            //transform::Manager::get(m_tile_set_scroll_transform_id)->position = transform::Manager::get(m_tile_set_transform_id)->position;
        }
        if (input()->is_pressed(input::Key::alt)) {
            input()->release(input::Key::alt);
            m_is_hidden_menu_down  = !m_is_hidden_menu_down;
            m_is_hidden_menu_right = !m_is_hidden_menu_right;
        }

        m_is_mouse_on_menu_up_bar = !input()->is_pressed(input::Key::ctrl) &&
                                    !m_is_showing_tile_set &&
                                    input::Manager::mouse.y > 0 && input::Manager::mouse.y <= 8 &&
                                    input::Manager::mouse.x <= m_menu_up_w ? true : false;

        if (m_is_mouse_on_menu_up_bar || m_is_mouse_on_menu_up_list) {
            transform::Manager::get(m_menu_up_transform_id)->position = { 0.0F, 0.0F };
            handle_menu_up();
            for (auto& i : m_mouse_sprite_ids) {
                sprite::Manager::get(i)->is_hidden = true;
            }
        } else {
            transform::Manager::get(m_menu_up_transform_id)->position = { 0.0F,-16.0F };
            transform::Manager::get(m_menu_up_lists[m_menu_up_labels[0]].transform_id)->position = {0.0F, m_menu_up_lists[m_menu_up_labels[0]].bg_h * -1.0F};
            transform::Manager::get(m_menu_up_lists[m_menu_up_labels[1]].transform_id)->position = { 0.0F, m_menu_up_lists[m_menu_up_labels[1]].bg_h * -1.0F };
            for (auto& i : m_mouse_sprite_ids) {
                sprite::Manager::get(i)->is_hidden = false;
            }
        }

        //Console::log("state::Edit::update tile_set position: ", transform::Manager::get(m_tile_set_transform_id)->position.x, " ", transform::Manager::get(m_tile_set_transform_id)->position.y, "\n");
        if (input()->is_pressed(input::Key::a)) {
            if (!m_moving_sprite_ids.empty()) {
                input()->release(input::Key::a);
                move_selected_on_level({ -16.0F, 0.0F });
                return;
            }
            if (input()->is_pressed(input::Key::shift)) {
                input()->release(input::Key::a);
                move_selection_on_tile_set({ -16, 0 });
            } else {
                if (m_is_showing_tile_set) {
                    move_tile_set({ 16, 0 });
                } else {
                    move_level({ 16, 0 });                    
                }
            }
        }
        if (input()->is_pressed(input::Key::d)) {
            if (!m_moving_sprite_ids.empty()) {
                input()->release(input::Key::d);
                move_selected_on_level({ 16.0F, 0.0F });
                return;
            }
            if (input()->is_pressed(input::Key::shift)) {
                input()->release(input::Key::d);
                move_selection_on_tile_set({ 16, 0 });
            } else {
                if (m_is_showing_tile_set) {
                    move_tile_set({ -16, 0 });
                } else {
                    move_level({ -16, 0 });
                }
            }
        }
        if (input()->is_pressed(input::Key::w)) {
            if (!m_moving_sprite_ids.empty()) {
                input()->release(input::Key::w);
                move_selected_on_level({ 0.0F, -16.0F });
                return;
            }
            if (input()->is_pressed(input::Key::shift)) {
                input()->release(input::Key::w);
                move_selection_on_tile_set({ 0,-16 });
            } else {
                if (m_is_showing_tile_set) {
                    move_tile_set({ 0, 16 });                    
                } else {
                    move_level({ 0, 16 });                    
                }
            }
        }
        if (input()->is_pressed(input::Key::s)) {
            if (!m_moving_sprite_ids.empty()) {
                input()->release(input::Key::s);
                move_selected_on_level({ 0.0F, 16.0F });
                return;
            }
            if (input()->is_pressed(input::Key::shift)) {
                input()->release(input::Key::s);   
                move_selection_on_tile_set({ 0, 16 });
            } else {
                if (m_is_showing_tile_set) {
                    move_tile_set({ 0, -16 });
                } else {
                    move_level({ 0, -16 });
                }
            }
        }

        //Console::log("tile set position: ", transform::Manager::get(m_tile_set_transform_id)->position.x, " ", transform::Manager::get(m_tile_set_transform_id)->position.y, "\n");

        if (input()->is_pressed(input::Key::backspace)) {
            input()->release(input::Key::backspace);
            //clear_grid_sprites();
            clear_level_sprites();
        }
        if (input()->is_pressed(input::Key::tab)) {
            input()->release(input::Key::tab);
            if (m_is_showing_tile_set) {
                m_tile_set == 255 ? init_tile_set(m_prev_tile_set) : init_tile_set(255);
            } else {
                if (input()->is_pressed(input::Key::shift) && m_layer > 0) {
                    sprite::Manager::get(m_active_layer_sprite_id)->offset.y -= 16.0F;
                    m_layer -= 1;
                } else if (!input()->is_pressed(input::Key::shift) && m_layer < 9) {
                    sprite::Manager::get(m_active_layer_sprite_id)->offset.y += 16.0F;
                    m_layer += 1;
                }
            }
        }
        
        if (!m_is_hidden_menu_down &&
            m_mouse_tile_position.y >= transform::Manager::get(m_menu_down_transform_id)->position.y &&
            m_mouse_tile_position.y <= transform::Manager::get(m_menu_down_transform_id)->position.y + sprite::Manager::get(m_menu_down_bg_sprite_id)->source_rect.h &&
            m_mouse_tile_position.x >= transform::Manager::get(m_menu_down_transform_id)->position.x &&
            m_mouse_tile_position.x <= transform::Manager::get(m_menu_down_transform_id)->position.x + sprite::Manager::get(m_menu_down_bg_sprite_id)->source_rect.w
            ) {
            //Console::log("state::Edit::handle_menu_down\n");
            handle_menu_down();
            return;
        }
        //Console::log("is_showing_menu_right: ", m_is_hidden_menu_right, " ", m_mouse_tile_position.x, " ", m_mouse_tile_position.y, "\n");
        if (!m_is_hidden_menu_right &&
            m_mouse_tile_position.x >= transform::Manager::get(m_menu_right_transform_id)->position.x &&            
            m_mouse_tile_position.x <= transform::Manager::get(m_menu_right_transform_id)->position.x + sprite::Manager::get(m_menu_right_bg_sprite_id)->source_rect.w &&
            m_mouse_tile_position.y >= transform::Manager::get(m_menu_right_transform_id)->position.y &&
            m_mouse_tile_position.y <= transform::Manager::get(m_menu_right_transform_id)->position.y + sprite::Manager::get(m_menu_right_bg_sprite_id)->source_rect.h) {            
            handle_menu_right();
            return;
        }
        
        bool is_placing_on_level = false;
        if (input()->is_pressed(input::Button::left)) {
            if (m_is_showing_tile_set &&
                (m_is_hidden_menu_down || !(m_mouse_tile_position.x >= transform::Manager::get(m_menu_right_transform_id)->position.x &&
                                             m_mouse_tile_position.x <= transform::Manager::get(m_menu_right_transform_id)->position.x + sprite::Manager::get(m_menu_right_bg_sprite_id)->source_rect.w &&
                                             m_mouse_tile_position.y >= transform::Manager::get(m_menu_down_transform_id)->position.y)
                )
                ) {
                //Console::log("state::Edit::update hello\n");
                if (!input()->is_pressed(input::Key::ctrl)) {
                    deselect_all_on_tile_set();
                }
                for (auto& mouse_id : m_mouse_sprite_ids) {
                    if (!sprite::Manager::get(mouse_id)) continue;
                    cVec2F offset = transform::Manager::get(m_mouse_transform_id)->position -
                                    transform::Manager::get(m_tile_set_transform_id)->position +
                                    sprite::Manager::get(mouse_id)->offset;
                    select_on_tile_set(offset);
                }
            } else {
                //input()->release(input::Button::left);

                if (input()->is_pressed(input::Key::ctrl)) {
                    for (auto& mouse_id : m_mouse_sprite_ids) {
                        if (!sprite::Manager::get(mouse_id)) continue;
                        cVec2F offset = transform::Manager::get(m_mouse_transform_id)->position -
                                        transform::Manager::get(m_level_transform_id)->position +
                                        sprite::Manager::get(mouse_id)->offset;
                        select_on_level(offset);
                    }
                } else {
                    deselect_all_on_level();

                    is_placing_on_level = true;
                    for (auto& sel_id : m_selection_on_tile_set_sprite_ids) {
                        size_t first_sel_id = m_selection_on_tile_set_sprite_ids.front();
                        cRectI source_rect  = { (I16)sprite::Manager::get(sel_id)->offset.x,
                                                (I16)sprite::Manager::get(sel_id)->offset.y,
                                                16, 16 };
                        
                        U16 num_placed = 0;
                        for (auto& mouse_id : m_mouse_sprite_ids) {
                            cVec2F offset = sprite::Manager::get(sel_id)->offset + sprite::Manager::get(mouse_id)->offset -
                                            sprite::Manager::get(first_sel_id)->offset +
                                            transform::Manager::get(m_mouse_transform_id)->position -
                                            transform::Manager::get(m_level_transform_id)->position;

                            undo::Act undo_act = place_sprite_on_level(m_layer, m_tile_set, source_rect, offset);
                            if (undo_act != undo::Act::null) {
                                m_undo_acts.emplace_back(undo_act);
                                ++num_placed;
                            }
                        }
                        m_last_undo_count += num_placed;
                    }
                }                
            }
        }
       
        if (input()->is_pressed(input::Button::right)) {
            if (m_is_showing_tile_set && (m_is_hidden_menu_down || (m_mouse_tile_position.x < 288 && m_mouse_tile_position.y < 160))) {
                for (auto& mouse_sprite_id : m_mouse_sprite_ids) {
                    Vec2F offset = transform::Manager::get(m_mouse_transform_id)->position -
                                   transform::Manager::get(m_tile_set_transform_id)->position +
                                   sprite::Manager::get(mouse_sprite_id)->offset;
                    deselect_on_tile_set(offset);
                }
            }
        }

        bool is_erasing_on_level = false;
        if (input()->is_pressed(input::Button::right)) {
            is_erasing_on_level = true;
            U16 num_erased = 0;
            for (auto& mouse_sprite_id : m_mouse_sprite_ids) {
                Vec2F offset = transform::Manager::get(m_mouse_transform_id)->position -
                               transform::Manager::get(m_level_transform_id)->position +
                               sprite::Manager::get(mouse_sprite_id)->offset;
                if (input()->is_pressed(input::Key::ctrl)) {
                    deselect_on_level(offset);
                } else {
                    deselect_all_on_level();
                    undo::Act undo_act = erase_sprite_on_level(m_layer, offset);
                    if (undo_act != undo::Act::null) {
                        m_undo_acts.emplace_back(undo_act);
                        ++num_erased;
                    }
                }
            }
            m_last_undo_count += num_erased;
        }
        if (!is_placing_on_level && !is_erasing_on_level) {
            if (m_last_undo_count > 0) {
                m_undo_counts.emplace_back(m_last_undo_count);
                m_last_undo_count = 0;
            }
        }

        if (input()->is_pressed(input::Button::middle)) {
            input()->release(input::Button::middle);
            cVec2F offset = transform::Manager::get(m_mouse_transform_id)->position - transform::Manager::get(m_level_transform_id)->position;
            eyedropper_on_level(offset);
            //m_is_showing_tile_set ? m_tile_set_brush_size = 1 : m_level_brush_size = 1;
            //init_brush(1);
        }

        if (input()->is_pressed(input::Key::h)) {
            input()->release(input::Key::h);
            m_is_hidden_grid = !m_is_hidden_grid;
            for (auto& i : m_grid_sprite_ids) {
                sprite::Manager::get(i)->is_hidden = m_is_hidden_grid;
            }
        }

        if (input()->is_pressed(input::Key::del)) {
            input()->release(input::Key::del);

            if (!m_selection_on_level_sprite_ids.empty()) {
                clear_selected_on_level();
                return;
            }
        }
        if (input()->is_pressed(input::Key::c)) {
            input()->release(input::Key::c);
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
        if (input()->is_pressed(input::Key::x)) {
            input()->release(input::Key::x);
            if (!m_selection_on_level_sprite_ids.empty()) {
                m_is_moving ? finish_moving_selected_on_level() : start_moving_selected_on_level();
                m_is_moving = !m_is_moving;
            }
        }
        if (input()->is_pressed(input::Key::esc)) {
            input()->release(input::Key::esc);  
            if (m_is_typing_text_bar) {
                quit_typing_text_bar();
                return;
            }
            if (m_is_typing_text_bar) {
                quit_typing_text_bar();
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
    }
}