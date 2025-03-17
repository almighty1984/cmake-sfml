module;
#include <sstream>

module state.edit;

import console;
import transforms;
import sprites;
import types;

namespace state {
    void Edit::move_selected_on_level(Vec2f move) {
        for (auto& i : m_selection_on_level_sprite_ids) {
            if (!Sprites::at(i)) continue;
            Sprites::at(i)->offset += move;
        }
        for (auto& i : m_moving_sprite_ids) {
            if (!Sprites::at(i)) continue;
            Sprites::at(i)->offset += move;
        }
    }
    void Edit::finish_moving_on_level() {
        Console::log("state::Edit::finish_moving_on_level layer: ", (int)m_layer, "\n");
        /*for (auto& i : m_selection_on_level_sprite_ids) {
            if (!Sprites::at(i)) continue;
            erase_sprite_on_level_at_offset(Sprites::at(i)->offset);
            Sprites::erase(i);
        }
        m_selection_on_level_sprite_ids.clear();*/

        for (auto& i : m_moving_sprite_ids) {
            if (!Sprites::at(i)) continue;

            Vec2<f32> offset       = Sprites::at(i)->offset;
            Rect<i32> source_rect  = Sprites::at(i)->source_rect;
            u8c       tile_set     = Sprites::at(i)->tile_set;
            size_t    transform_id = Sprites::at(i)->transform_id;

            erase_sprite_on_level_at_offset(Sprites::at(i)->offset);
            Sprites::erase(i);

            //place_sprite_on_level_at_offset(Sprites::at(i)->offset);
            {
                size_t sprite_id = Sprites::make(tile_set_texture_path(tile_set));
                Sprites::at(sprite_id)->offset = offset;
                Sprites::at(sprite_id)->source_rect = source_rect;
                Sprites::at(sprite_id)->tile_set = tile_set;
                Sprites::at(sprite_id)->transform_id = transform_id;
                Sprites::at(sprite_id)->layer = m_layer;
            }          

            //Sprites::erase(i);
        }
        m_moving_sprite_ids.clear();
    }
    void Edit::clear_selection_on_level() {
        for (auto& i : m_selection_on_level_sprite_ids) {
            if (!Sprites::at(i)) continue;
            Sprites::erase(i);
        }
        m_selection_on_level_sprite_ids.clear();
    }
    void Edit::place_sprite_on_level_at_offset(Vec2f offset) {
        for (auto& sel : m_selection_on_set_sprite_ids) {
            Vec2fc pos = offset + Sprites::at(sel)->offset - Sprites::at(m_selection_on_set_sprite_ids.front())->offset;

            for (size_t i = 0; i < Sprites::size(); ++i) {
                if (!Sprites::at(i)) continue;
                if (Sprites::at(i)->transform_id == m_level_transform_id &&
                    Sprites::at(i)->layer == m_layer &&
                    Sprites::at(i)->offset == pos) {
                    Console::log("state::edit::place_sprite_on_level_at_offset already placed, resetting source_rect\n");
                    Sprites::at(i)->source_rect = { (i16)Sprites::at(sel)->offset.x,
                                                    (i16)Sprites::at(sel)->offset.y + Sprites::at(m_tile_set_sprite_id)->source_rect.y,
                                                     16, 16 };
                    goto is_already_placed;
                }
            }
            {   Console::log("state::Edit::place_sprite_on_level_at_offset placing new sprite\n");
                size_t sprite_id = Sprites::make(tile_set_texture_path(0));
                Sprites::at(sprite_id)->id = sprite_id;
                Sprites::at(sprite_id)->transform_id = m_level_transform_id;
                Sprites::at(sprite_id)->source_rect = { (i16)Sprites::at(sel)->offset.x,
                                                        (i16)Sprites::at(sel)->offset.y + Sprites::at(m_tile_set_sprite_id)->source_rect.y,
                                                        16, 16 };
                Sprites::at(sprite_id)->offset      = pos;
                Sprites::at(sprite_id)->layer       = m_layer;
                Sprites::at(sprite_id)->tile_set    = m_tile_set;
            }   is_already_placed:  ;
        }
    }
    void Edit::erase_sprite_on_level_at_offset(Vec2f offset) {
        for (size_t i = 0; i < Sprites::size(); ++i) {
            if (!Sprites::at(i)) continue;
            if (Sprites::at(i)->transform_id == m_level_transform_id && Sprites::at(i)->offset == offset && Sprites::at(i)->layer == m_layer) {
                Console::log("state::Edit::erase_sprite_on_level_at_offset ", i, " ", " id: ", Sprites::at(i)->id, " offset: ", offset.x, " ", offset.y, "\n");
                Sprites::erase(i);
                return;
            }
        }
    }
    Vec2f Edit::level_sprite_source_at_offset(u8c layer, Vec2f offset) {
        for (size_t i = 0; i < Sprites::size(); ++i) {
            if (!Sprites::at(i)) continue;
            if (Sprites::at(i)->transform_id == m_level_transform_id && Sprites::at(i)->offset == offset && Sprites::at(i)->layer == layer) {
                Console::log("state::Edit::level_sprite_source_at_offset: ", offset.x, " ", offset.y, " source: ", Sprites::at(i)->source_rect.x, " ", Sprites::at(i)->source_rect.y, "\n");
                return { (f32)Sprites::at(i)->source_rect.x, (f32)Sprites::at(i)->source_rect.y };
            }
        }
        return {};
    }
    void Edit::select_on_level(Vec2f offset) {        
        for (auto& sel_id : m_selection_on_level_sprite_ids) {
            if (!Sprites::at(sel_id) || Sprites::at(sel_id)->transform_id != m_level_transform_id) continue;
            if (Sprites::at(sel_id)->offset == offset) {                    
                return;
            }
        }
        Console::log("state::Edit::select_on_level offset: ", offset.x, " ", offset.y, "\n");
        m_selection_on_level_sprite_ids.push_back(Sprites::make(m_mouse_texture_path));
        Sprites::at(m_selection_on_level_sprite_ids.back())->transform_id = m_level_transform_id;
        Sprites::at(m_selection_on_level_sprite_ids.back())->layer        = SELECTION_ON_LEVEL_LAYER;
        Sprites::at(m_selection_on_level_sprite_ids.back())->offset       = offset;        
    }
    void Edit::deselect_on_level(Vec2f offset) {        
        for (auto& sel_id : m_selection_on_level_sprite_ids) {
            if (!Sprites::at(sel_id) || Sprites::at(sel_id)->transform_id != m_level_transform_id) continue;
            if (Sprites::at(sel_id)->offset == offset) {
                Console::log("state::Edit::deselect_on_level aready selected, deselecting\n");
                Sprites::erase(sel_id);
                std::vector<size_t> resized_selection;
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
    void Edit::act_on_selection_on_level(SelectionAct act) {
        Console::log("state::Edit::act_on_selection_on_level\n");
        std::set<size_t> found_sprite_ids;
        for (auto& i : m_selection_on_level_sprite_ids) {
            if (!Sprites::at(i)) continue;
            for (size_t sprite_id = 0; sprite_id < Sprites::size(); ++sprite_id) {
                if (sprite_id == i || !Sprites::at(sprite_id) ||
                    std::find(m_selection_on_level_sprite_ids.begin(),
                        m_selection_on_level_sprite_ids.end(), sprite_id) != m_selection_on_level_sprite_ids.end()) {
                    continue;
                }
                if (Sprites::at(sprite_id)->offset == Sprites::at(i)->offset &&
                    Sprites::at(sprite_id)->layer == m_layer &&
                    Sprites::at(sprite_id)->transform_id == m_level_transform_id) {
                    found_sprite_ids.insert(sprite_id);
                }
            }
        }
        for (auto& i : found_sprite_ids) {
            if (act == SelectionAct::copy || act == SelectionAct::move) {
                size_t sprite_id = Sprites::make(tile_set_texture_path(Sprites::at(i)->tile_set));
                m_moving_sprite_ids.push_back(sprite_id);
                Sprites::at(sprite_id)->id = sprite_id;
                *Sprites::at(sprite_id) = *Sprites::at(i);
            }
            if (act == SelectionAct::clear || act == SelectionAct::move) {
                Sprites::erase(i);
            }
        }
    }
    size_t Edit::level_sprite_id_at_offset(u8c layer, Vec2fc offset) {
        for (size_t i = 0; i < Sprites::size(); ++i) {
            if (!Sprites::at(i)) return -1;
            if (Sprites::at(i)->transform_id == m_level_transform_id &&
                Sprites::at(i)->layer == layer &&
                Sprites::at(i)->offset == offset) {
                Console::log("state::Edit::level_sprite_id_at_offset layer: ", (int)layer, " position: ", offset.x, " ", offset.y, " ", "id: ", Sprites::at(i)->id, "\n");
                return Sprites::at(i)->id;
            }
        }
        return -1;
    }
    void Edit::load_level_sprites_from(const std::filesystem::path& path) {
        Console::log("Edit::load_level_sprites_from\n");
        auto sprite_data = Sprites::open(path);

        for (auto& i : sprite_data) {
            if (level_sprite_id_at_offset(i.layer, { (f32)i.offset_x, (f32)i.offset_y }) != -1) continue;

            size_t sprite_id = Sprites::make(tile_set_texture_path(i.tile_set));
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
    }
    void Edit::save_level_sprites_to(const std::filesystem::path& path) {
        //auto max = grid_tile_dimensions();
        //Console::log("state::Edit::save_level_sprites_to grid dimensions: ", max.x, " ", max.y, "\n");
        Sprites::save(m_level_path, m_grid_sprite_ids);
    }
}