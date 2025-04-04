module;
#include <algorithm>
#include <sstream>

module state.edit;

import console;
import transforms;
import sprites;
import types;

namespace state {
    void Edit::move_level(Vec2fc amount) {
        Transforms::at(m_level_transform_id)->position += amount;
        Transforms::at(m_grid_transform_id)->position += amount;

        Sprites::at(m_position_on_grid_map_sprite_id)->offset = Vec2f{ 7.0f, 7.0f } - Transforms::at(m_grid_transform_id)->position / 16.0f;
    }
    void Edit::move_level_to(Vec2fc position) {
        Transforms::at(m_level_transform_id)->position = position;
        Transforms::at(m_grid_transform_id)->position = position;
        
        Sprites::at(m_position_on_grid_map_sprite_id)->offset = Vec2f{ 7.0f, 7.0f } - Transforms::at(m_grid_transform_id)->position / 16.0f;
    }
    void Edit::move_selected_on_level(Vec2fc amount) {
        for (auto& i : m_selection_on_level_sprite_ids) {
            if (!Sprites::at(i)) continue;
            Sprites::at(i)->offset += amount;
            
            if (amount.x < 0.0f && Sprites::at(i)->offset.x + Transforms::at(m_level_transform_id)->position.x < 0.0f ||
                amount.x > 0.0f && Sprites::at(i)->offset.x + Transforms::at(m_level_transform_id)->position.x > 272.0f) {
                Transforms::at(m_level_transform_id)->position.x -= amount.x;
                Transforms::at(m_grid_transform_id)->position.x -= amount.x;                
            }
            if (amount.y < 0.0f && Sprites::at(i)->offset.y + Transforms::at(m_level_transform_id)->position.y < 0.0f ||
                amount.y > 0.0f && Sprites::at(i)->offset.y + Transforms::at(m_level_transform_id)->position.y > 144.0f) {
                Transforms::at(m_level_transform_id)->position.y -= amount.y;
                Transforms::at(m_grid_transform_id)->position.y -= amount.y;                
            }
        }
        for (auto& i : m_moving_sprite_ids) {
            if (!Sprites::at(i)) continue;
            Sprites::at(i)->offset += amount;
        }
    }
    void Edit::finish_moving_selected_on_level() {
        Console::log("state::Edit::finish_moving_on_level layer: ", (int)m_layer, "\n");
                
        u16 undo_count = 0;
        for (auto& moving_sprite_id : m_moving_sprite_ids) {
            if (!Sprites::at(moving_sprite_id)) continue;
            Vec2fc offset       = Sprites::at(moving_sprite_id)->offset;
            Rectic source_rect  = Sprites::at(moving_sprite_id)->source_rect;
            u8c    tile_set     = Sprites::at(moving_sprite_id)->tile_set;
            Sprites::erase(moving_sprite_id);

            for (size_t i = 0; i < Sprites::size(); ++i) {
                if (!Sprites::at(i)) continue;
                if (Sprites::at(i)->transform_id == m_level_transform_id &&
                    Sprites::at(i)->offset == offset &&
                    Sprites::at(i)->layer == m_layer) {
                    PreviousAct prev_act = erase_sprite_on_level(m_layer, offset);
                    if (prev_act != PreviousAct::null) {
                        m_previous_acts.push_back(prev_act);
                        ++undo_count;
                    }
                    break;
                }
            }
            PreviousAct prev_act = place_sprite_on_level(m_layer, tile_set, source_rect, offset);
            if (prev_act != PreviousAct::null) {
                m_previous_acts.push_back(prev_act);
                ++undo_count;
            }
        }        
        if (undo_count > 0) {
            m_undo_counts.push_back(undo_count);
        }
        m_moving_sprite_ids.clear();
    }
    void Edit::deselect_all_on_level() {
        for (auto& i : m_selection_on_level_sprite_ids) {
            if (!Sprites::at(i)) continue;
            Sprites::erase(i);
        }
        m_selection_on_level_sprite_ids.clear();
    } 
    
    PreviousAct Edit::place_sprite_on_level(u8c layer, u8c tile_set, Rectic source_rect, Vec2fc offset) {
        for (i32 i = 0; i < Sprites::size(); ++i) {
            if (!Sprites::at(i)) continue;
            if (Sprites::at(i)->transform_id == m_level_transform_id &&
                Sprites::at(i)->layer        == layer                &&
                Sprites::at(i)->offset       == offset) {
                
                if (Sprites::at(i)->tile_set    == tile_set &&
                    Sprites::at(i)->source_rect == source_rect
                    ) {
                    return PreviousAct::null;
                } else {
                    m_undo_replaced.push_back({ Sprites::at(i)->transform_id,
                                                Sprites::at(i)->layer,
                                                Sprites::at(i)->tile_set,
                                                Sprites::at(i)->source_rect,
                                                Sprites::at(i)->offset });

                    Console::log("state::Edit::place_sprite_on_level replacing, previous act size: ", m_previous_acts.size(), "\n");


                    Sprites::at(i)->source_rect = source_rect;
                    if (Sprites::at(i)->tile_set != tile_set) {
                        Sprites::at(i)->tile_set = tile_set;
                        Sprites::at(i)->texture("res/textures/set_" + std::to_string((int)tile_set) + ".png");
                    }
                    return PreviousAct::replaced;
                }
            }
        }
        Console::log("state::Edit::place_sprite_on_level_at_offset placing, previous acts size: ", m_previous_acts.size(), "\n");
        i32 sprite_id = Sprites::make(tile_set_texture_path(m_tile_set));
        Sprites::at(sprite_id)->id           = sprite_id;
        Sprites::at(sprite_id)->transform_id = m_level_transform_id;
        Sprites::at(sprite_id)->layer        = layer;
        Sprites::at(sprite_id)->tile_set     = tile_set;
        Sprites::at(sprite_id)->source_rect  = source_rect;
        Sprites::at(sprite_id)->offset       = offset;
        Sprites::at(sprite_id)->texture("res/textures/set_" + std::to_string((int)tile_set) + ".png");

        m_undo_placed.push_back({ Sprites::at(sprite_id)->transform_id,
                                  Sprites::at(sprite_id)->layer,
                                  Sprites::at(sprite_id)->tile_set,
                                  Sprites::at(sprite_id)->source_rect,
                                  Sprites::at(sprite_id)->offset });
        
        return PreviousAct::placed;
    }
    PreviousAct Edit::erase_sprite_on_level(u8c layer, Vec2fc offset) {
        for (size_t i = 0; i < Sprites::size(); ++i) {
            if (!Sprites::at(i)) continue;
            if (Sprites::at(i)->transform_id == m_level_transform_id &&
                Sprites::at(i)->offset       == offset               &&
                Sprites::at(i)->layer        == layer) {
                Console::log("state::Edit::erase_sprite_on_level_at_offset ", i, " ", " id: ", Sprites::at(i)->id, " previous_acts.size: ", m_previous_acts.size(), "\n");
                
                m_undo_erased.push_back({ Sprites::at(i)->transform_id,
                                          Sprites::at(i)->layer,
                                          Sprites::at(i)->tile_set,
                                          Sprites::at(i)->source_rect,
                                          Sprites::at(i)->offset });

                Sprites::erase(i);
                return PreviousAct::erased;
            }
        }
        return PreviousAct::null;
    }
    bool Edit::select_on_level(Vec2fc offset) {        
        for (auto& sel_id : m_selection_on_level_sprite_ids) {
            if (!Sprites::at(sel_id) || Sprites::at(sel_id)->transform_id != m_level_transform_id) continue;
            if (Sprites::at(sel_id)->offset == offset) {
                return false;
            }
        }
        //Console::log("state::Edit::select_on_level offset: ", offset.x, " ", offset.y, "\n");
        m_selection_on_level_sprite_ids.push_back(Sprites::make(m_mouse_texture_path));
        Sprites::at(m_selection_on_level_sprite_ids.back())->transform_id = m_level_transform_id;
        Sprites::at(m_selection_on_level_sprite_ids.back())->layer        = SELECTION_ON_LEVEL_LAYER;
        Sprites::at(m_selection_on_level_sprite_ids.back())->offset       = offset;
        return true;
    }
    void Edit::deselect_on_level(Vec2fc offset) {
        for (auto& sel_id : m_selection_on_level_sprite_ids) {
            if (!Sprites::at(sel_id) || Sprites::at(sel_id)->transform_id != m_level_transform_id) continue;
            if (Sprites::at(sel_id)->offset == offset) {
                Console::log("state::Edit::deselect_on_level aready selected, deselecting\n");
                Sprites::erase(sel_id);
                std::vector<i32> resized_selection;
                for (auto& i : m_selection_on_level_sprite_ids) {
                    if (i != sel_id) {
                        resized_selection.push_back(i);
                    }
                }
                m_selection_on_level_sprite_ids.clear();
                m_selection_on_level_sprite_ids = resized_selection;
            }
        }
    }
    std::vector<i32> Edit::find_sprites_in_selection_on_level() {
        std::vector<i32> found_sprite_ids;
        for (auto& i : m_selection_on_level_sprite_ids) {
            if (!Sprites::at(i)) continue;
            for (i32 sprite_id = 0; sprite_id < Sprites::size(); ++sprite_id) {
                if (sprite_id == i || !Sprites::at(sprite_id) ||
                    std::find(m_selection_on_level_sprite_ids.begin(),
                        m_selection_on_level_sprite_ids.end(), sprite_id) != m_selection_on_level_sprite_ids.end()) {
                    continue;
                }
                if (Sprites::at(sprite_id)->offset == Sprites::at(i)->offset &&
                    Sprites::at(sprite_id)->layer == m_layer &&
                    Sprites::at(sprite_id)->transform_id == m_level_transform_id) {
                    found_sprite_ids.push_back(sprite_id);
                }
            }
        }
        return found_sprite_ids;
    }
    void Edit::copy_selected_start_moving_on_level() {
        Console::log("state::Edit::init_copy_selection_on_level\n");
        std::vector<i32> found_sprite_ids = find_sprites_in_selection_on_level();

        for (auto& i : m_moving_sprite_ids) {
            Sprites::erase(i);
        }
        m_moving_sprite_ids.clear();
        u16 undo_count = 0;
        for (auto& i : found_sprite_ids) {
            i32 sprite_id = Sprites::make(tile_set_texture_path(Sprites::at(i)->tile_set));
            m_moving_sprite_ids.push_back(sprite_id);
            Sprites::at(sprite_id)->id = sprite_id;
            *Sprites::at(sprite_id) = *Sprites::at(i);
        }        
    }

    void Edit::start_moving_selected_on_level() {
        std::vector<i32> found_sprite_ids = find_sprites_in_selection_on_level();

        for (auto& i : m_moving_sprite_ids) {
            Sprites::erase(i);
        }
        m_moving_sprite_ids.clear();
        u16 undo_count = 0;
        for (auto& i : found_sprite_ids) {            
            i32 sprite_id = Sprites::make(tile_set_texture_path(Sprites::at(i)->tile_set));
            m_moving_sprite_ids.push_back(sprite_id);
            Sprites::at(sprite_id)->id = sprite_id;
            *Sprites::at(sprite_id) = *Sprites::at(i);
            
            m_previous_acts.push_back(PreviousAct::moved);
            m_undo_moved.push_back({ Sprites::at(sprite_id)->transform_id,
                                     Sprites::at(sprite_id)->layer,
                                     Sprites::at(sprite_id)->tile_set,
                                     Sprites::at(sprite_id)->source_rect,
                                     Sprites::at(sprite_id)->offset });
            Sprites::erase(i);
            ++undo_count;       
            
        }
        m_undo_counts.push_back(undo_count);
    }
    void Edit::clear_selected_on_level() {
        std::vector<i32> found_sprite_ids = find_sprites_in_selection_on_level();

        for (auto& i : m_moving_sprite_ids) {
            Sprites::erase(i);
        }
        m_moving_sprite_ids.clear();
        u16 undo_count = 0;
        for (auto& i : found_sprite_ids) {            
            PreviousAct prev_act = erase_sprite_on_level(Sprites::at(i)->layer, Sprites::at(i)->offset);
            m_previous_acts.push_back(prev_act);
            ++undo_count;            
        }        
        m_undo_counts.push_back(undo_count);
    }    
    bool Edit::level_eyedropper_at_offset(Vec2fc offset) {
        Sprite* sprite = level_sprite_at_offset(m_layer, offset);
        if (!sprite) return false;

        Console::log("state::Edit::level_eyedropper_at_offset: ", offset.x, ", ", offset.y, " id: ", sprite->id, "\n");
        
        m_tile_set = sprite->tile_set;
        m_layer = sprite->layer;
        init_tile_set(m_tile_set);

        for (auto& i : m_selection_on_tile_set_sprite_ids) {
            Sprites::erase(i);
        }
        m_selection_on_tile_set_sprite_ids.clear();
        m_selection_on_tile_set_sprite_ids.push_back(Sprites::make("res/textures/tile_selection.png"));
        Sprites::at(m_selection_on_tile_set_sprite_ids.back())->layer = SELECTION_ON_TILE_SET_LAYER;
        Sprites::at(m_selection_on_tile_set_sprite_ids.back())->transform_id = m_selection_on_tile_set_transform_id;
        Sprites::at(m_selection_on_tile_set_sprite_ids.back())->offset = { (f32)sprite->source_rect.x, (f32)sprite->source_rect.y };        
        return true;
    }
    Sprite* Edit::level_sprite_at_offset(u8c layer, Vec2fc offset) {
        for (i32 i = 0; i < Sprites::size(); ++i) {
            if (!Sprites::at(i)) continue;
            if (Sprites::at(i)->transform_id == m_level_transform_id &&
                Sprites::at(i)->layer        == layer                &&
                Sprites::at(i)->offset       == offset                 ) {                
                return Sprites::at(i);
            }
        }
        return nullptr;
    }
    void Edit::clear_level_sprites() {
        for (i32 i = 0; i < Sprites::size(); ++i) {
            if (!Sprites::at(i)) continue;
            if (Sprites::at(i)->transform_id == m_level_transform_id) {
                Console::log("state::Edit::clear_level_sprites ", i, " ", " id: ", Sprites::at(i)->id, "\n");
                Sprites::erase(i);
            }
        }
    }
    void Edit::undo_previous_act() {
        if (m_previous_acts.empty() || m_undo_counts.empty()) return;
        Console::log("state::Edit::undo_previous_act count: ", m_undo_counts.back(), "\n");

        for (u16 undo_count = 0; undo_count < m_undo_counts.back(); ++undo_count) {
            if (m_previous_acts.empty()) break;
            Console::log("Edit::undo_previous_act: ", as_string(m_previous_acts.back()), " ", m_previous_acts.size(), "\n");
            if (m_previous_acts.back() == PreviousAct::placed && !m_undo_placed.empty()) {
                Console::log("state::Edit::undo_previous_act undo place\n");
                for (size_t i = 0; i < Sprites::size(); ++i) {
                    if (!Sprites::at(i)) continue;
                    if (Sprites::at(i)->transform_id == m_level_transform_id        &&
                        Sprites::at(i)->offset       == m_undo_placed.back().offset &&
                        Sprites::at(i)->layer        == m_undo_placed.back().layer) {
                        Sprites::erase(i);
                        break;
                    }
                }
                m_undo_placed.pop_back();
            } else if (m_previous_acts.back() == PreviousAct::replaced && !m_undo_replaced.empty()) {
                Sprite* sprite = level_sprite_at_offset(m_undo_replaced.back().layer, m_undo_replaced.back().offset);
                if (sprite) {
                    Console::log("state::Edit::undo_previous_act undo replace\n");
                    sprite->transform_id = m_undo_replaced.back().transform_id;
                    sprite->layer        = m_undo_replaced.back().layer;
                    sprite->tile_set     = m_undo_replaced.back().tile_set;
                    sprite->source_rect  = m_undo_replaced.back().source_rect;
                    sprite->offset       = m_undo_replaced.back().offset;
                }
                m_undo_replaced.pop_back();
            } else if (m_previous_acts.back() == PreviousAct::erased && !m_undo_erased.empty()) {
                if (!level_sprite_at_offset(m_undo_erased.back().layer, m_undo_erased.back().offset)) {
                    Console::log("state::Edit::undo_previous_act undo erase\n");
                    size_t sprite_id = Sprites::make(tile_set_texture_path(m_undo_erased.back().tile_set));
                    Sprites::at(sprite_id)->id           = sprite_id;
                    Sprites::at(sprite_id)->transform_id = m_level_transform_id;
                    Sprites::at(sprite_id)->layer        = m_undo_erased.back().layer;
                    Sprites::at(sprite_id)->tile_set     = m_undo_erased.back().tile_set;
                    Sprites::at(sprite_id)->source_rect  = m_undo_erased.back().source_rect;
                    Sprites::at(sprite_id)->offset       = m_undo_erased.back().offset;
                }
                m_undo_erased.pop_back();
            } else if (m_previous_acts.back() == PreviousAct::moved && !m_undo_moved.empty()) {
                Console::log("state::Edit::undo_previous_act undo move\n");

                size_t sprite_id = Sprites::make(tile_set_texture_path(m_undo_moved.back().tile_set));
                Sprites::at(sprite_id)->id = sprite_id;
                Sprites::at(sprite_id)->transform_id = m_level_transform_id;
                Sprites::at(sprite_id)->layer        = m_undo_moved.back().layer;
                Sprites::at(sprite_id)->tile_set     = m_undo_moved.back().tile_set;
                Sprites::at(sprite_id)->source_rect  = m_undo_moved.back().source_rect;
                Sprites::at(sprite_id)->offset       = m_undo_moved.back().offset;
                
                m_undo_moved.pop_back();
            }
            m_previous_acts.pop_back();
        }
        m_undo_counts.pop_back();
    }
    void Edit::load_level_sprites_from(const std::filesystem::path& path) {
        Console::log("Edit::load_level_sprites_from\n");

        //m_undo_amount.clear();
        m_undo_erased.clear();
        m_undo_placed.clear();
        m_undo_replaced.clear();
        m_previous_acts.clear();

        auto sprite_data = Sprites::open(path);

        for (auto& i : sprite_data) {
            if (level_sprite_at_offset(i.layer, { (f32)i.offset_x, (f32)i.offset_y }) ) continue;

            i32 sprite_id = Sprites::make(tile_set_texture_path(i.tile_set));
            Sprites::at(sprite_id)->id = sprite_id;
            Sprites::at(sprite_id)->tile_set = i.tile_set;
            Sprites::at(sprite_id)->layer = i.layer;
            Sprites::at(sprite_id)->offset = Vec2f{ (f32)i.offset_x, (f32)i.offset_y };
            Sprites::at(sprite_id)->source_rect = Rect<i32>{ i.source_x, i.source_y, 16, 16 };
            Sprites::at(sprite_id)->transform_id = m_level_transform_id;

            //Console::log("state::Edit::load_level_sprites_from sprite ", sprite_id, " offset: ", i.offset_x, " ", i.offset_y, "\n");                                   
            Vec2f grid_remainder = { std::fmodf(Sprites::at(sprite_id)->offset.x, 256.0f), std::fmodf(Sprites::at(sprite_id)->offset.y, 256.0f) };                        
            Vec2f grid_pos = Sprites::at(sprite_id)->offset - grid_remainder;
            add_grid_sprite_id_at_offset(grid_pos);
        }
        m_current_level.set_text(path.string());

        move_level_to({ 0.0f, 0.0f });
    }
    void Edit::save_level_sprites_to(const std::filesystem::path& path) {
        //auto max = grid_tile_dimensions();
        //Console::log("state::Edit::save_level_sprites_to grid dimensions: ", max.x, " ", max.y, "\n");
        Sprites::save(m_level_path, m_grid_sprite_ids);
    }
}