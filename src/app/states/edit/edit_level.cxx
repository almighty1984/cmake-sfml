module;
#include <algorithm>
#include <sstream>

module state.edit;

import console;
import transform;
import sprite;
import types;

namespace state {
    void Edit::move_level(cVec2F amount) {
        transform::Manager::get(m_level_transform_id)->position += amount;
        transform::Manager::get(m_grid_transform_id)->position += amount;

        sprite::Manager::get(m_position_on_grid_map_sprite_id)->offset = Vec2F{ 7.0F, 7.0F } - transform::Manager::get(m_grid_transform_id)->position / 16.0F;
    }
    void Edit::move_level_to(cVec2F position) {
        transform::Manager::get(m_level_transform_id)->position = position;
        transform::Manager::get(m_grid_transform_id)->position = position;
        
        sprite::Manager::get(m_position_on_grid_map_sprite_id)->offset = Vec2F{ 7.0F, 7.0F } - transform::Manager::get(m_grid_transform_id)->position / 16.0F;
    }
    void Edit::move_selected_on_level(cVec2F amount) {
        for (auto& i : m_selection_on_level_sprite_ids) {
            if (!sprite::Manager::get(i)) continue;
            sprite::Manager::get(i)->offset += amount;
            
            if (amount.x < 0.0F && sprite::Manager::get(i)->offset.x + transform::Manager::get(m_level_transform_id)->position.x < view().x ||
                amount.x > 0.0F && sprite::Manager::get(i)->offset.x + transform::Manager::get(m_level_transform_id)->position.x > view().w - 48.0F) {
                transform::Manager::get(m_level_transform_id)->position.x -= amount.x;
                transform::Manager::get(m_grid_transform_id)->position.x -= amount.x;                
            }
            if (amount.y < 0.0F && sprite::Manager::get(i)->offset.y + transform::Manager::get(m_level_transform_id)->position.y < view().y ||
                amount.y > 0.0F && sprite::Manager::get(i)->offset.y + transform::Manager::get(m_level_transform_id)->position.y > view().h - 32.0F) {
                transform::Manager::get(m_level_transform_id)->position.y -= amount.y;
                transform::Manager::get(m_grid_transform_id)->position.y -= amount.y;
            }
        }
        for (auto& i : m_moving_sprite_ids) {
            if (!sprite::Manager::get(i)) continue;
            sprite::Manager::get(i)->offset += amount;
        }
        Console::log("state::Edit::move_selected_on_level view: ", view().w, " ", view().h, "\n");
    }
    void Edit::finish_moving_selected_on_level() {
        Console::log("state::Edit::finish_moving_on_level layer: ", (int)m_layer, "\n");
                
        U16 undo_count = 0;
        for (auto& moving_sprite_id : m_moving_sprite_ids) {
            if (!sprite::Manager::get(moving_sprite_id)) continue;
            cVec2F offset       = sprite::Manager::get(moving_sprite_id)->offset;
            cRectI source_rect  = sprite::Manager::get(moving_sprite_id)->source_rect;
            cU8    tile_set     = sprite::Manager::get(moving_sprite_id)->tile_set;
            sprite::Manager::erase(moving_sprite_id);

            for (size_t i = 0; i < sprite::Manager::size(); ++i) {
                if (!sprite::Manager::get(i)) continue;
                if (sprite::Manager::get(i)->transform_id == m_level_transform_id &&
                    sprite::Manager::get(i)->offset == offset &&
                    sprite::Manager::get(i)->layer == m_layer) {
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
            if (!sprite::Manager::get(i)) continue;
            sprite::Manager::erase(i);
        }
        m_selection_on_level_sprite_ids.clear();
    } 
    
    undo::Act Edit::place_sprite_on_level(cU8 layer, cU8 tile_set, cRectI source_rect, cVec2F offset) {        
        if (tile_set == 255 && m_selection_on_tile_set_sprite_ids.size() == 1) {
            cVec2F tile_offset = { source_rect.x / 16.0F, source_rect.y / 16.0F };
            m_tile_number = std::fmodf(tile_offset.x, 32.0F) + tile_offset.y * 32.0F;

            if (m_types.find(entity::Info{ 255, m_tile_number }) != m_types.end()) {
                const std::string type_str = m_types.at(entity::Info{ 255, m_tile_number });
                Console::log("tile number: ", (int)m_tile_number, " type: ", type_str, "\n");
            }
        }

        const std::filesystem::path texture_path = "res/texture/set_" + std::to_string((int)tile_set) + ".png";

        // Replace if found
        for (auto& i : m_level_sprite_ids) {
            if (!sprite::Manager::get(i)) continue;
            if (sprite::Manager::get(i)->transform_id == m_level_transform_id &&
                sprite::Manager::get(i)->layer == layer &&
                sprite::Manager::get(i)->offset == offset) {

                if (sprite::Manager::get(i)->tile_set == tile_set &&
                    sprite::Manager::get(i)->source_rect == source_rect
                    ) {
                    return undo::Act::null;
                } else {
                    m_undo_info_replaced.push_back({ sprite::Manager::get(i)->transform_id,
                                                sprite::Manager::get(i)->layer,
                                                sprite::Manager::get(i)->tile_set,
                                                sprite::Manager::get(i)->source_rect,
                                                sprite::Manager::get(i)->offset });

                    //Console::log("state::Edit::place_sprite_on_level replacing, previous act size: ", m_undo_acts.size(), "\n");
                    sprite::Manager::get(i)->source_rect = source_rect;
                    if (sprite::Manager::get(i)->tile_set != tile_set) {
                        sprite::Manager::get(i)->tile_set = tile_set;
                        sprite::Manager::get(i)->texture(texture_path);
                    }

                    return undo::Act::replaced;
                }
            }
        }        

        // Place new
        //Console::log("state::Edit::place_sprite_on_level_at_offset placing, previous acts size: ", m_undo_acts.size(), "\n");
        cI32 sprite_id = sprite::Manager::make(tile_set_texture_path(m_tile_set));
        sprite::Manager::get(sprite_id)->id           = sprite_id;
        sprite::Manager::get(sprite_id)->transform_id = m_level_transform_id;
        sprite::Manager::get(sprite_id)->layer        = layer;
        sprite::Manager::get(sprite_id)->tile_set     = tile_set;
        sprite::Manager::get(sprite_id)->source_rect  = source_rect;
        sprite::Manager::get(sprite_id)->offset       = offset;
        sprite::Manager::get(sprite_id)->texture(texture_path);

        m_level_sprite_ids.emplace_back(sprite_id);

        m_undo_info_placed.push_back({ sprite::Manager::get(sprite_id)->transform_id,
                                       sprite::Manager::get(sprite_id)->layer,
                                       sprite::Manager::get(sprite_id)->tile_set,
                                       sprite::Manager::get(sprite_id)->source_rect,
                                       sprite::Manager::get(sprite_id)->offset });
        
        return undo::Act::placed;
    }
    undo::Act Edit::erase_sprite_on_level(cU8 layer, cVec2F offset) {
        if (m_is_showing_tile_set) return undo::Act::null;
        for (auto it = m_level_sprite_ids.begin(); it != m_level_sprite_ids.end(); ++it) {
            cI32 i = *it;

            if (!sprite::Manager::get(i)) continue;
            if (sprite::Manager::get(i)->transform_id == m_level_transform_id &&
                sprite::Manager::get(i)->offset       == offset               &&
                sprite::Manager::get(i)->layer        == layer) {
                Console::log("state::Edit::erase_sprite_on_level_at_offset ", i, " ", " id: ", sprite::Manager::get(i)->id, " previous_acts.size: ", m_undo_acts.size(), "\n");
                
                m_undo_info_erased.push_back({ sprite::Manager::get(i)->transform_id,
                                          sprite::Manager::get(i)->layer,
                                          sprite::Manager::get(i)->tile_set,
                                          sprite::Manager::get(i)->source_rect,
                                          sprite::Manager::get(i)->offset });

                sprite::Manager::erase(i);

                m_level_sprite_ids.erase(it);

                return undo::Act::erased;
            }
        }
        return undo::Act::null;
    }
    bool Edit::select_on_level(cVec2F offset) {        
        for (auto& sel_id : m_selection_on_level_sprite_ids) {
            if (!sprite::Manager::get(sel_id) || sprite::Manager::get(sel_id)->transform_id != m_level_transform_id) continue;
            if (sprite::Manager::get(sel_id)->offset == offset) {
                return false;
            }
        }
        Console::log("state::Edit::select_on_level offset: ", offset.x, " ", offset.y, "\n");
        m_selection_on_level_sprite_ids.push_back(sprite::Manager::make(m_mouse_texture_path));
        sprite::Manager::get(m_selection_on_level_sprite_ids.back())->transform_id = m_level_transform_id;
        sprite::Manager::get(m_selection_on_level_sprite_ids.back())->layer        = SELECTION_ON_LEVEL_LAYER;
        sprite::Manager::get(m_selection_on_level_sprite_ids.back())->offset       = offset;
        return true;
    }
    bool Edit::deselect_on_level(cVec2F offset) {
        for (auto& sel_id : m_selection_on_level_sprite_ids) {
            if (!sprite::Manager::get(sel_id) || sprite::Manager::get(sel_id)->transform_id != m_level_transform_id) continue;
            if (sprite::Manager::get(sel_id)->offset == offset) {
                Console::log("state::Edit::deselect_on_level aready selected, deselecting\n");
                sprite::Manager::erase(sel_id);
                std::vector<I32> resized_selection;
                for (auto& i : m_selection_on_level_sprite_ids) {
                    if (i != sel_id) {
                        resized_selection.push_back(i);
                    }
                }
                m_selection_on_level_sprite_ids.clear();
                m_selection_on_level_sprite_ids = resized_selection;
                return true;
            }
        }
        return false;
    }
    std::vector<I32> Edit::find_sprites_in_selection_on_level() {
        std::vector<I32> found_sprite_ids;
        for (auto& sel_sprite_id : m_selection_on_level_sprite_ids) {
            if (!sprite::Manager::get(sel_sprite_id)) continue;

            for (auto& sprite_id : m_level_sprite_ids) {
                if (sprite_id == sel_sprite_id || !sprite::Manager::get(sprite_id) ||
                    std::find(m_selection_on_level_sprite_ids.begin(),
                        m_selection_on_level_sprite_ids.end(), sprite_id) != m_selection_on_level_sprite_ids.end()) {
                    continue;
                }
                if (sprite::Manager::get(sprite_id)->offset == sprite::Manager::get(sel_sprite_id)->offset &&
                    sprite::Manager::get(sprite_id)->layer == m_layer &&
                    sprite::Manager::get(sprite_id)->transform_id == m_level_transform_id) {
                    found_sprite_ids.push_back(sprite_id);
                }
            }
        }
        return found_sprite_ids;
    }
    void Edit::copy_selected_start_moving_on_level() {
        Console::log("state::Edit::init_copy_selection_on_level\n");
        std::vector<I32> found_sprite_ids = find_sprites_in_selection_on_level();

        for (auto& i : m_moving_sprite_ids) {
            sprite::Manager::erase(i);
        }
        m_moving_sprite_ids.clear();
        U16 undo_count = 0;
        for (auto& i : found_sprite_ids) {
            cI32 sprite_id = sprite::Manager::make(tile_set_texture_path(sprite::Manager::get(i)->tile_set));
            m_moving_sprite_ids.emplace_back(sprite_id);
            *sprite::Manager::get(sprite_id) = *sprite::Manager::get(i);
            sprite::Manager::get(sprite_id)->id = sprite_id;
        }        
    }

    void Edit::start_moving_selected_on_level() {
        std::vector<I32> found_sprite_ids = find_sprites_in_selection_on_level();
        
        for (auto it = m_level_sprite_ids.begin(); it != m_level_sprite_ids.end(); ++it) {
            if (std::find(found_sprite_ids.begin(), found_sprite_ids.end(), (*it)) != found_sprite_ids.end()) {
                m_level_sprite_ids.erase(it);
            }
        }
        
        for (auto& i : m_moving_sprite_ids) {
            sprite::Manager::erase(i);
        }
        m_moving_sprite_ids.clear();
        U16 undo_count = 0;
        for (auto& i : found_sprite_ids) {            
            cI32 sprite_id = sprite::Manager::make(tile_set_texture_path(sprite::Manager::get(i)->tile_set));
            m_moving_sprite_ids.push_back(sprite_id);            
            *sprite::Manager::get(sprite_id) = *sprite::Manager::get(i);
            sprite::Manager::get(sprite_id)->id = sprite_id;
            
            m_undo_acts.push_back(undo::Act::moved);
            m_undo_info_moved.push_back({ sprite::Manager::get(sprite_id)->transform_id,
                                          sprite::Manager::get(sprite_id)->layer,
                                          sprite::Manager::get(sprite_id)->tile_set,
                                          sprite::Manager::get(sprite_id)->source_rect,
                                          sprite::Manager::get(sprite_id)->offset });
            sprite::Manager::erase(i);            
            ++undo_count;            
        }
        m_undo_counts.push_back(undo_count);
    }
    void Edit::clear_selected_on_level() {
        std::vector<I32> found_sprite_ids = find_sprites_in_selection_on_level();

        for (auto& i : m_moving_sprite_ids) {
            sprite::Manager::erase(i);
        }
        m_moving_sprite_ids.clear();
        U16 undo_count = 0;
        for (auto& i : found_sprite_ids) {
            undo::Act undo_act = erase_sprite_on_level(sprite::Manager::get(i)->layer, sprite::Manager::get(i)->offset);
            m_undo_acts.push_back(undo_act);
            ++undo_count;
        }
        m_undo_counts.push_back(undo_count);
    }
    bool Edit::eyedropper_on_level(cVec2F offset) {
        if (m_is_showing_tile_set) return false;

        sprite::Object* sprite = level_sprite_at_offset(m_layer, offset);
        if (!sprite) return false;
        Console::log("state::Edit::eyedropper_on_level: ", offset.x, ", ", offset.y, " id: ", sprite->id, "\n");

        m_tile_set = sprite->tile_set;
        m_layer = sprite->layer;
        init_tile_set(m_tile_set);

        for (auto& i : m_selection_on_tile_set_sprite_ids) {
            sprite::Manager::erase(i);
        }
        m_selection_on_tile_set_sprite_ids.clear();
        m_selection_on_tile_set_sprite_ids.emplace_back(sprite::Manager::make("res/texture/tile_selection.png"));
        sprite::Manager::get(m_selection_on_tile_set_sprite_ids.back())->layer = SELECTION_ON_TILE_SET_LAYER;
        sprite::Manager::get(m_selection_on_tile_set_sprite_ids.back())->transform_id = m_tile_set_scroll_transform_id;
        sprite::Manager::get(m_selection_on_tile_set_sprite_ids.back())->offset = { (F32)sprite->source_rect.x, (F32)sprite->source_rect.y };
        sprite::Manager::get(m_selection_on_tile_set_sprite_ids.back())->is_hidden = true;


        sprite::Manager::get(m_current_tile_sprite_id)->source_rect = { sprite->source_rect.x, sprite->source_rect.y, 16, 16 };

        //transform::Manager::get(m_tile_set_transform_id)->position.x = view().w - 16.0F;
        //transform::Manager::get(m_tile_set_transform_id)->position.y = view().w == 480.0F ? 240.0F : 160.0F;

        //sprite::Manager::get(m_tile_set_sprite_id)->source_rect.x = sprite::Manager::get(m_selection_on_tile_set_sprite_ids.back())->offset.x;
        //sprite::Manager::get(m_tile_set_sprite_id)->source_rect.y = sprite::Manager::get(m_selection_on_tile_set_sprite_ids.back())->offset.y;

        //Vec2F focus_position = { 144.0F - sprite::Manager::get(m_selection_on_tile_set_sprite_ids.back())->offset.x,
                                 //64.0F  - sprite::Manager::get(m_selection_on_tile_set_sprite_ids.back())->offset.y};

        //if      (focus_position.x >    0.0F)   focus_position.x =    0.0F;
        //else if (focus_position.x < -192.0F)   focus_position.x = -192.0F;
        //if      (focus_position.y >  512.0F)   focus_position.y =  512.0F;
        //else if (focus_position.y <  160.0F)   focus_position.y =  160.0F;
        
        //m_tile_set_focus = focus_position;

        return true;
    }
    sprite::Object* Edit::level_sprite_at_offset(cU8 layer, cVec2F offset) {
        for (I32 i = 0; i < sprite::Manager::size(); ++i) {
            if (!sprite::Manager::get(i)) continue;
            if (sprite::Manager::get(i)->transform_id == m_level_transform_id &&
                sprite::Manager::get(i)->layer        == layer                &&
                sprite::Manager::get(i)->offset       == offset                 ) {                
                return sprite::Manager::get(i);
            }
        }
        return nullptr;
    }
    void Edit::clear_level_sprites() {
        for (auto& i : m_level_sprite_ids) {
            sprite::Manager::erase(i);
        }
        m_level_sprite_ids.clear();        
    }
    void Edit::undo_last_act() {
        if (m_undo_acts.empty() || m_undo_counts.empty()) return;
        Console::log("state::Edit::undo_last_act count: ", m_undo_counts.back(), "\n");

        for (U16 undo_count = 0; undo_count < m_undo_counts.back(); ++undo_count) {
            if (m_undo_acts.empty()) break;
            Console::log("Edit::undo_last_act: ", undo::as_string(m_undo_acts.back()), " ", m_undo_acts.size(), "\n");
            if (m_undo_acts.back() == undo::Act::placed && !m_undo_info_placed.empty()) {
                Console::log("state::Edit::undo_last_act undo place\n");


                for (size_t i = 0; i < sprite::Manager::size(); ++i) {
                    if (!sprite::Manager::get(i)) continue;
                    if (sprite::Manager::get(i)->transform_id == m_level_transform_id        &&
                        sprite::Manager::get(i)->offset       == m_undo_info_placed.back().offset &&
                        sprite::Manager::get(i)->layer        == m_undo_info_placed.back().layer) {
                        sprite::Manager::erase(i);

                        for (auto it = m_level_sprite_ids.begin(); it != m_level_sprite_ids.end(); ++it) {
                            if ((*it) == i) {
                                m_level_sprite_ids.erase(it);
                            }
                        }
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
                    cI32 sprite_id = sprite::Manager::make(tile_set_texture_path(m_undo_info_erased.back().tile_set));
                    sprite::Manager::get(sprite_id)->id           = sprite_id;
                    sprite::Manager::get(sprite_id)->transform_id = m_level_transform_id;
                    sprite::Manager::get(sprite_id)->layer        = m_undo_info_erased.back().layer;
                    sprite::Manager::get(sprite_id)->tile_set     = m_undo_info_erased.back().tile_set;
                    sprite::Manager::get(sprite_id)->source_rect  = m_undo_info_erased.back().source_rect;
                    sprite::Manager::get(sprite_id)->offset       = m_undo_info_erased.back().offset;

                    
                    m_level_sprite_ids.emplace_back(sprite_id);
                    
                }
                m_undo_info_erased.pop_back();
            } else if (m_undo_acts.back() == undo::Act::moved && !m_undo_info_moved.empty()) {
                Console::log("state::Edit::undo_last_act undo move\n");

                cI32 sprite_id = sprite::Manager::make(tile_set_texture_path(m_undo_info_moved.back().tile_set));
                sprite::Manager::get(sprite_id)->id           = sprite_id;
                sprite::Manager::get(sprite_id)->transform_id = m_level_transform_id;
                sprite::Manager::get(sprite_id)->layer        = m_undo_info_moved.back().layer;
                sprite::Manager::get(sprite_id)->tile_set     = m_undo_info_moved.back().tile_set;
                sprite::Manager::get(sprite_id)->source_rect  = m_undo_info_moved.back().source_rect;
                sprite::Manager::get(sprite_id)->offset       = m_undo_info_moved.back().offset;
                
                m_level_sprite_ids.emplace_back(sprite_id);

                m_undo_info_moved.pop_back();
            }
            m_undo_acts.pop_back();
        }
        m_undo_counts.pop_back();
    }
    void Edit::load_level_sprites(const std::filesystem::path& path) {
        Console::log("Edit::load_level_sprites ", path, "\n");

        m_undo_info_erased.clear();
        m_undo_info_placed.clear();
        m_undo_info_replaced.clear();
        m_undo_info_moved.clear();
        m_undo_acts.clear();

        auto sprite_data = sprite::Manager::load_level(path);

        for (auto& i : sprite_data) {
            if (level_sprite_at_offset(i.layer, { (F32)i.x, (F32)i.y }) ) continue;

            //m_visible_layers.insert(i.layer);

            cI32 sprite_id = sprite::Manager::make(tile_set_texture_path(i.tile_set));
            m_level_sprite_ids.emplace_back(sprite_id);

            sprite::Manager::get(sprite_id)->id = sprite_id;
            sprite::Manager::get(sprite_id)->tile_set = i.tile_set;
            sprite::Manager::get(sprite_id)->layer = i.layer;
            sprite::Manager::get(sprite_id)->offset = Vec2F{ (F32)i.x, (F32)i.y };
            sprite::Manager::get(sprite_id)->source_rect = Rect<I32>{ i.source_x, i.source_y, 16, 16 };
            sprite::Manager::get(sprite_id)->transform_id = m_level_transform_id;


            //Console::log("state::Edit::load_level_sprites sprite ", sprite_id, " offset: ", i.x, " ", i.y, "\n");                                   
            Vec2F grid_remainder = { std::fmodf(sprite::Manager::get(sprite_id)->offset.x, 256.0F), std::fmodf(sprite::Manager::get(sprite_id)->offset.y, 256.0F) };                        
            Vec2F grid_pos = sprite::Manager::get(sprite_id)->offset - grid_remainder;
            add_grid_at_offset(grid_pos);
        }
        Console::log("\n\nstate::Edit::load_level_sprites path: ", path.string(), "\n\n");
        m_text_bar.set_text(path.string());

        move_level_to({ 0.0F, 0.0F });
    }    
}