module;
#include <algorithm>
#include <sstream>

module state.edit;

import console;
import transform;
import sprite;
import types;

namespace state {
    void Edit::move_level(Vec2fc amount) {
        transform::Set::at(m_level_transform_id)->position += amount;
        transform::Set::at(m_grid_transform_id)->position += amount;

        sprite::Set::at(m_position_on_grid_map_sprite_id)->offset = Vec2f{ 7.0f, 7.0f } - transform::Set::at(m_grid_transform_id)->position / 16.0f;
    }
    void Edit::move_level_to(Vec2fc position) {
        transform::Set::at(m_level_transform_id)->position = position;
        transform::Set::at(m_grid_transform_id)->position = position;
        
        sprite::Set::at(m_position_on_grid_map_sprite_id)->offset = Vec2f{ 7.0f, 7.0f } - transform::Set::at(m_grid_transform_id)->position / 16.0f;
    }
    void Edit::move_selected_on_level(Vec2fc amount) {
        for (auto& i : m_selection_on_level_sprite_ids) {
            if (!sprite::Set::at(i)) continue;
            sprite::Set::at(i)->offset += amount;
            
            if (amount.x < 0.0f && sprite::Set::at(i)->offset.x + transform::Set::at(m_level_transform_id)->position.x < 0.0f ||
                amount.x > 0.0f && sprite::Set::at(i)->offset.x + transform::Set::at(m_level_transform_id)->position.x > 272.0f) {
                transform::Set::at(m_level_transform_id)->position.x -= amount.x;
                transform::Set::at(m_grid_transform_id)->position.x -= amount.x;                
            }
            if (amount.y < 0.0f && sprite::Set::at(i)->offset.y + transform::Set::at(m_level_transform_id)->position.y < 0.0f ||
                amount.y > 0.0f && sprite::Set::at(i)->offset.y + transform::Set::at(m_level_transform_id)->position.y > 144.0f) {
                transform::Set::at(m_level_transform_id)->position.y -= amount.y;
                transform::Set::at(m_grid_transform_id)->position.y -= amount.y;                
            }
        }
        for (auto& i : m_moving_sprite_ids) {
            if (!sprite::Set::at(i)) continue;
            sprite::Set::at(i)->offset += amount;
        }
    }
    void Edit::finish_moving_selected_on_level() {
        Console::log("state::Edit::finish_moving_on_level layer: ", (int)m_layer, "\n");
                
        u16 undo_count = 0;
        for (auto& moving_sprite_id : m_moving_sprite_ids) {
            if (!sprite::Set::at(moving_sprite_id)) continue;
            Vec2fc offset       = sprite::Set::at(moving_sprite_id)->offset;
            Rectic source_rect  = sprite::Set::at(moving_sprite_id)->source_rect;
            u8c    tile_set     = sprite::Set::at(moving_sprite_id)->tile_set;
            sprite::Set::erase(moving_sprite_id);

            for (size_t i = 0; i < sprite::Set::size(); ++i) {
                if (!sprite::Set::at(i)) continue;
                if (sprite::Set::at(i)->transform_id == m_level_transform_id &&
                    sprite::Set::at(i)->offset == offset &&
                    sprite::Set::at(i)->layer == m_layer) {
                    undo::Act prev_act = erase_sprite_on_level(m_layer, offset);
                    if (prev_act != undo::Act::null) {
                        m_undo_acts.push_back(prev_act);
                        ++undo_count;
                    }
                    break;
                }
            }
            undo::Act undo_act = place_sprite_on_level(m_layer, tile_set, source_rect, offset);
            if (undo_act != undo::Act::null) {
                m_undo_acts.push_back(undo_act);
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
            if (!sprite::Set::at(i)) continue;
            sprite::Set::erase(i);
        }
        m_selection_on_level_sprite_ids.clear();
    } 
    
    undo::Act Edit::place_sprite_on_level(u8c layer, u8c tile_set, Rectic source_rect, Vec2fc offset) {
        for (i32 i = 0; i < sprite::Set::size(); ++i) {
            if (!sprite::Set::at(i)) continue;
            if (sprite::Set::at(i)->transform_id == m_level_transform_id &&
                sprite::Set::at(i)->layer        == layer                &&
                sprite::Set::at(i)->offset       == offset) {
                
                if (sprite::Set::at(i)->tile_set    == tile_set &&
                    sprite::Set::at(i)->source_rect == source_rect
                    ) {
                    return undo::Act::null;
                } else {
                    m_undo_info_replaced.push_back({ sprite::Set::at(i)->transform_id,
                                                sprite::Set::at(i)->layer,
                                                sprite::Set::at(i)->tile_set,
                                                sprite::Set::at(i)->source_rect,
                                                sprite::Set::at(i)->offset });

                    Console::log("state::Edit::place_sprite_on_level replacing, previous act size: ", m_undo_acts.size(), "\n");


                    sprite::Set::at(i)->source_rect = source_rect;
                    if (sprite::Set::at(i)->tile_set != tile_set) {
                        sprite::Set::at(i)->tile_set = tile_set;
                        sprite::Set::at(i)->texture("res/textures/set_" + std::to_string((int)tile_set) + ".png");
                    }
                    return undo::Act::replaced;
                }
            }
        }
        Console::log("state::Edit::place_sprite_on_level_at_offset placing, previous acts size: ", m_undo_acts.size(), "\n");
        i32 sprite_id = sprite::Set::make(tile_set_texture_path(m_tile_set));
        sprite::Set::at(sprite_id)->id           = sprite_id;
        sprite::Set::at(sprite_id)->transform_id = m_level_transform_id;
        sprite::Set::at(sprite_id)->layer        = layer;
        sprite::Set::at(sprite_id)->tile_set     = tile_set;
        sprite::Set::at(sprite_id)->source_rect  = source_rect;
        sprite::Set::at(sprite_id)->offset       = offset;
        sprite::Set::at(sprite_id)->texture("res/textures/set_" + std::to_string((int)tile_set) + ".png");

        m_undo_info_placed.push_back({ sprite::Set::at(sprite_id)->transform_id,
                                  sprite::Set::at(sprite_id)->layer,
                                  sprite::Set::at(sprite_id)->tile_set,
                                  sprite::Set::at(sprite_id)->source_rect,
                                  sprite::Set::at(sprite_id)->offset });
        
        return undo::Act::placed;
    }
    undo::Act Edit::erase_sprite_on_level(u8c layer, Vec2fc offset) {
        if (m_is_showing_tile_set) return undo::Act::null;
        for (size_t i = 0; i < sprite::Set::size(); ++i) {
            if (!sprite::Set::at(i)) continue;
            if (sprite::Set::at(i)->transform_id == m_level_transform_id &&
                sprite::Set::at(i)->offset       == offset               &&
                sprite::Set::at(i)->layer        == layer) {
                Console::log("state::Edit::erase_sprite_on_level_at_offset ", i, " ", " id: ", sprite::Set::at(i)->id, " previous_acts.size: ", m_undo_acts.size(), "\n");
                
                m_undo_info_erased.push_back({ sprite::Set::at(i)->transform_id,
                                          sprite::Set::at(i)->layer,
                                          sprite::Set::at(i)->tile_set,
                                          sprite::Set::at(i)->source_rect,
                                          sprite::Set::at(i)->offset });

                sprite::Set::erase(i);                
                return undo::Act::erased;
            }
        }
        return undo::Act::null;
    }
    bool Edit::select_on_level(Vec2fc offset) {        
        for (auto& sel_id : m_selection_on_level_sprite_ids) {
            if (!sprite::Set::at(sel_id) || sprite::Set::at(sel_id)->transform_id != m_level_transform_id) continue;
            if (sprite::Set::at(sel_id)->offset == offset) {
                return false;
            }
        }
        //Console::log("state::Edit::select_on_level offset: ", offset.x, " ", offset.y, "\n");
        m_selection_on_level_sprite_ids.push_back(sprite::Set::make(m_mouse_texture_path));
        sprite::Set::at(m_selection_on_level_sprite_ids.back())->transform_id = m_level_transform_id;
        sprite::Set::at(m_selection_on_level_sprite_ids.back())->layer        = SELECTION_ON_LEVEL_LAYER;
        sprite::Set::at(m_selection_on_level_sprite_ids.back())->offset       = offset;
        return true;
    }
    void Edit::deselect_on_level(Vec2fc offset) {
        for (auto& sel_id : m_selection_on_level_sprite_ids) {
            if (!sprite::Set::at(sel_id) || sprite::Set::at(sel_id)->transform_id != m_level_transform_id) continue;
            if (sprite::Set::at(sel_id)->offset == offset) {
                Console::log("state::Edit::deselect_on_level aready selected, deselecting\n");
                sprite::Set::erase(sel_id);
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
            if (!sprite::Set::at(i)) continue;
            for (i32 sprite_id = 0; sprite_id < sprite::Set::size(); ++sprite_id) {
                if (sprite_id == i || !sprite::Set::at(sprite_id) ||
                    std::find(m_selection_on_level_sprite_ids.begin(),
                        m_selection_on_level_sprite_ids.end(), sprite_id) != m_selection_on_level_sprite_ids.end()) {
                    continue;
                }
                if (sprite::Set::at(sprite_id)->offset == sprite::Set::at(i)->offset &&
                    sprite::Set::at(sprite_id)->layer == m_layer &&
                    sprite::Set::at(sprite_id)->transform_id == m_level_transform_id) {
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
            sprite::Set::erase(i);
        }
        m_moving_sprite_ids.clear();
        u16 undo_count = 0;
        for (auto& i : found_sprite_ids) {
            i32 sprite_id = sprite::Set::make(tile_set_texture_path(sprite::Set::at(i)->tile_set));
            m_moving_sprite_ids.push_back(sprite_id);           
            *sprite::Set::at(sprite_id) = *sprite::Set::at(i);
            sprite::Set::at(sprite_id)->id = sprite_id;
        }        
    }

    void Edit::start_moving_selected_on_level() {
        std::vector<i32> found_sprite_ids = find_sprites_in_selection_on_level();

        for (auto& i : m_moving_sprite_ids) {
            sprite::Set::erase(i);
        }
        m_moving_sprite_ids.clear();
        u16 undo_count = 0;
        for (auto& i : found_sprite_ids) {            
            i32 sprite_id = sprite::Set::make(tile_set_texture_path(sprite::Set::at(i)->tile_set));
            m_moving_sprite_ids.push_back(sprite_id);            
            *sprite::Set::at(sprite_id) = *sprite::Set::at(i);
            sprite::Set::at(sprite_id)->id = sprite_id;
            
            m_undo_acts.push_back(undo::Act::moved);
            m_undo_info_moved.push_back({ sprite::Set::at(sprite_id)->transform_id,
                                     sprite::Set::at(sprite_id)->layer,
                                     sprite::Set::at(sprite_id)->tile_set,
                                     sprite::Set::at(sprite_id)->source_rect,
                                     sprite::Set::at(sprite_id)->offset });
            sprite::Set::erase(i);
            ++undo_count;       
            
        }
        m_undo_counts.push_back(undo_count);
    }
    void Edit::clear_selected_on_level() {
        std::vector<i32> found_sprite_ids = find_sprites_in_selection_on_level();

        for (auto& i : m_moving_sprite_ids) {
            sprite::Set::erase(i);
        }
        m_moving_sprite_ids.clear();
        u16 undo_count = 0;
        for (auto& i : found_sprite_ids) {
            undo::Act undo_act = erase_sprite_on_level(sprite::Set::at(i)->layer, sprite::Set::at(i)->offset);
            m_undo_acts.push_back(undo_act);
            ++undo_count;
        }
        m_undo_counts.push_back(undo_count);
    }
    bool Edit::eyedropper_on_level(Vec2fc offset) {
        if (m_is_showing_tile_set) return false;

        sprite::Object* sprite = level_sprite_at_offset(m_layer, offset);
        if (!sprite) return false;
        Console::log("state::Edit::eyedropper_on_level: ", offset.x, ", ", offset.y, " id: ", sprite->id, "\n");

        m_tile_set = sprite->tile_set;
        m_layer = sprite->layer;
        init_tile_set(m_tile_set);

        for (auto& i : m_selection_on_tile_set_sprite_ids) {
            sprite::Set::erase(i);
        }
        m_selection_on_tile_set_sprite_ids.clear();
        m_selection_on_tile_set_sprite_ids.push_back(sprite::Set::make("res/textures/tile_selection.png"));
        sprite::Set::at(m_selection_on_tile_set_sprite_ids.back())->layer = SELECTION_ON_TILE_SET_LAYER;
        sprite::Set::at(m_selection_on_tile_set_sprite_ids.back())->transform_id = m_selection_on_tile_set_transform_id;
        sprite::Set::at(m_selection_on_tile_set_sprite_ids.back())->offset = { (f32)sprite->source_rect.x, (f32)sprite->source_rect.y };
        
        Vec2f focus_position = { 144.0f - sprite::Set::at(m_selection_on_tile_set_sprite_ids.back())->offset.x,
                                  64.0f - sprite::Set::at(m_selection_on_tile_set_sprite_ids.back())->offset.y + 512.0f};

        if      (focus_position.x > 0.0f)      focus_position.x = 0.0f;
        else if (focus_position.x < -192.0f)   focus_position.x = -192.0f;
        if      (focus_position.y > 512.0f)    focus_position.y = 512.0f;
        else if (focus_position.y < 160.0f)    focus_position.y = 160.0f;
        
        transform::Set::at(m_tile_set_transform_id)->position = focus_position;

        return true;
    }
    sprite::Object* Edit::level_sprite_at_offset(u8c layer, Vec2fc offset) {
        for (i32 i = 0; i < sprite::Set::size(); ++i) {
            if (!sprite::Set::at(i)) continue;
            if (sprite::Set::at(i)->transform_id == m_level_transform_id &&
                sprite::Set::at(i)->layer        == layer                &&
                sprite::Set::at(i)->offset       == offset                 ) {                
                return sprite::Set::at(i);
            }
        }
        return nullptr;
    }
    void Edit::clear_level_sprites() {
        for (i32 i = 0; i < sprite::Set::size(); ++i) {
            if (!sprite::Set::at(i)) continue;
            if (sprite::Set::at(i)->transform_id == m_level_transform_id) {
                Console::log("state::Edit::clear_level_sprites ", i, " ", " id: ", sprite::Set::at(i)->id, "\n");
                sprite::Set::erase(i);
            }
        }
    }
    void Edit::undo_last_act() {
        if (m_undo_acts.empty() || m_undo_counts.empty()) return;
        Console::log("state::Edit::undo_last_act count: ", m_undo_counts.back(), "\n");

        for (u16 undo_count = 0; undo_count < m_undo_counts.back(); ++undo_count) {
            if (m_undo_acts.empty()) break;
            Console::log("Edit::undo_last_act: ", undo::as_string(m_undo_acts.back()), " ", m_undo_acts.size(), "\n");
            if (m_undo_acts.back() == undo::Act::placed && !m_undo_info_placed.empty()) {
                Console::log("state::Edit::undo_last_act undo place\n");
                for (size_t i = 0; i < sprite::Set::size(); ++i) {
                    if (!sprite::Set::at(i)) continue;
                    if (sprite::Set::at(i)->transform_id == m_level_transform_id        &&
                        sprite::Set::at(i)->offset       == m_undo_info_placed.back().offset &&
                        sprite::Set::at(i)->layer        == m_undo_info_placed.back().layer) {
                        sprite::Set::erase(i);
                        break;
                    }
                }
                m_undo_info_placed.pop_back();
            } else if (m_undo_acts.back() == undo::Act::replaced && !m_undo_info_replaced.empty()) {
                sprite::Object* sprite = level_sprite_at_offset(m_undo_info_replaced.back().layer, m_undo_info_replaced.back().offset);
                if (sprite) {
                    Console::log("state::Edit::undo_last_act undo replace\n");
                    sprite->transform_id = m_undo_info_replaced.back().transform_id;
                    sprite->layer        = m_undo_info_replaced.back().layer;
                    sprite->tile_set     = m_undo_info_replaced.back().tile_set;
                    sprite->source_rect  = m_undo_info_replaced.back().source_rect;
                    sprite->offset       = m_undo_info_replaced.back().offset;
                    sprite->texture(tile_set_texture_path(sprite->tile_set));
                }
                m_undo_info_replaced.pop_back();
            } else if (m_undo_acts.back() == undo::Act::erased && !m_undo_info_erased.empty()) {
                if (!level_sprite_at_offset(m_undo_info_erased.back().layer, m_undo_info_erased.back().offset)) {
                    Console::log("state::Edit::undo_last_act undo erase\n");
                    size_t sprite_id = sprite::Set::make(tile_set_texture_path(m_undo_info_erased.back().tile_set));
                    sprite::Set::at(sprite_id)->id           = sprite_id;
                    sprite::Set::at(sprite_id)->transform_id = m_level_transform_id;
                    sprite::Set::at(sprite_id)->layer        = m_undo_info_erased.back().layer;
                    sprite::Set::at(sprite_id)->tile_set     = m_undo_info_erased.back().tile_set;
                    sprite::Set::at(sprite_id)->source_rect  = m_undo_info_erased.back().source_rect;
                    sprite::Set::at(sprite_id)->offset       = m_undo_info_erased.back().offset;
                }
                m_undo_info_erased.pop_back();
            } else if (m_undo_acts.back() == undo::Act::moved && !m_undo_info_moved.empty()) {
                Console::log("state::Edit::undo_last_act undo move\n");

                size_t sprite_id = sprite::Set::make(tile_set_texture_path(m_undo_info_moved.back().tile_set));
                sprite::Set::at(sprite_id)->id           = sprite_id;
                sprite::Set::at(sprite_id)->transform_id = m_level_transform_id;
                sprite::Set::at(sprite_id)->layer        = m_undo_info_moved.back().layer;
                sprite::Set::at(sprite_id)->tile_set     = m_undo_info_moved.back().tile_set;
                sprite::Set::at(sprite_id)->source_rect  = m_undo_info_moved.back().source_rect;
                sprite::Set::at(sprite_id)->offset       = m_undo_info_moved.back().offset;
                
                m_undo_info_moved.pop_back();
            }
            m_undo_acts.pop_back();
        }
        m_undo_counts.pop_back();
    }
    void Edit::load_level_sprites(const std::filesystem::path& path) {
        Console::log("Edit::load_level_sprites\n");

        m_undo_info_erased.clear();
        m_undo_info_placed.clear();
        m_undo_info_replaced.clear();
        m_undo_info_moved.clear();
        m_undo_acts.clear();

        auto sprite_data = sprite::Set::open(path);

        for (auto& i : sprite_data) {
            if (level_sprite_at_offset(i.layer, { (f32)i.x, (f32)i.y }) ) continue;

            i32c sprite_id = sprite::Set::make(tile_set_texture_path(i.tile_set));
            sprite::Set::at(sprite_id)->id = sprite_id;
            sprite::Set::at(sprite_id)->tile_set = i.tile_set;
            sprite::Set::at(sprite_id)->layer = i.layer;
            sprite::Set::at(sprite_id)->offset = Vec2f{ (f32)i.x, (f32)i.y };
            sprite::Set::at(sprite_id)->source_rect = Rect<i32>{ i.source_x, i.source_y, 16, 16 };
            sprite::Set::at(sprite_id)->transform_id = m_level_transform_id;

            //Console::log("state::Edit::load_level_sprites sprite ", sprite_id, " offset: ", i.offset_x, " ", i.offset_y, "\n");                                   
            Vec2f grid_remainder = { std::fmodf(sprite::Set::at(sprite_id)->offset.x, 256.0f), std::fmodf(sprite::Set::at(sprite_id)->offset.y, 256.0f) };                        
            Vec2f grid_pos = sprite::Set::at(sprite_id)->offset - grid_remainder;
            add_grid_sprite_id_at_offset(grid_pos);
        }
        m_text_bar.set_text(path.string());

        move_level_to({ 0.0f, 0.0f });
    }    
}