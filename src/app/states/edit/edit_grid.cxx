module;
#include <sstream>

module state.edit;

import console;
import transform;
import sprite;
import types;

namespace state {
    i32c Edit::grid_sprite_id_at_offset(Vec2fc offset) {
        if (m_grid_sprite_ids.empty()) return -1;

        f32c x = offset.x - std::fmodf(offset.x, 256.0f);
        f32c y = offset.y - std::fmodf(offset.y, 256.0f);

        for (auto& i : m_grid_sprite_ids) {
            if (!sprite::Set::at(i)) continue;
            if (sprite::Set::at(i)->offset == Vec2f{ x, y }) {
                return sprite::Set::at(i)->id;
            }
        }
        return -1;
    }
    i32c Edit::grid_map_sprite_id_at_offset(Vec2fc offset) {
        if (m_grid_map_sprite_ids.empty()) return -1;

        f32c x = offset.x - std::fmodf(offset.x, 16.0f);
        f32c y = offset.y - std::fmodf(offset.y, 16.0f);

        for (auto& i : m_grid_map_sprite_ids) {
            if (!sprite::Set::at(i)) continue;
            if (sprite::Set::at(i)->offset == Vec2f{ x, y }) {
                return sprite::Set::at(i)->id;
            }
        }
        return -1;
    }
    bool Edit::add_grid_at_offset(Vec2fc offset) {
        for (auto& i : m_grid_sprite_ids) {
            if (sprite::Set::at(i)->offset == offset) {
                Console::log("state::Edit::add_grid_at_offset ", offset.x, " ", offset.y, " already added\n");
                return false;
            }
        }
        i32c grid_sprite_id = sprite::Set::make(m_grid_texture_path);
        m_grid_sprite_ids.push_back(grid_sprite_id);
        sprite::Set::at(grid_sprite_id)->id = grid_sprite_id;
        sprite::Set::at(grid_sprite_id)->source_rect = { 0, 0, 256, 256 };
        sprite::Set::at(grid_sprite_id)->offset = offset;
        sprite::Set::at(grid_sprite_id)->transform_id = m_grid_transform_id;
        sprite::Set::at(grid_sprite_id)->layer = GRID_LAYER;
        
        i32c grid_map_sprite_id = sprite::Set::make(m_grid_map_texture_path);
        m_grid_map_sprite_ids.push_back(grid_map_sprite_id);
        sprite::Set::at(grid_map_sprite_id)->id = grid_map_sprite_id;
        sprite::Set::at(grid_map_sprite_id)->source_rect = { 0, 0, 16, 16 };
        sprite::Set::at(grid_map_sprite_id)->offset = offset / 16.0f;
        sprite::Set::at(grid_map_sprite_id)->transform_id = m_grid_map_transform_id;
        sprite::Set::at(grid_map_sprite_id)->layer = GRID_MAP_LAYER;

        Console::log("state::Edit::add_grid_at_offset ", offset.x, " ", offset.y, " sprite id: ", grid_sprite_id, " map sprite id: ", grid_map_sprite_id, "\n");
        return true;
    }
    bool Edit::erase_grid_at_offset(Vec2fc offset) {
        if (m_grid_sprite_ids.empty() || m_grid_sprite_ids.size() < 2) {
            Console::log("state::Edit::erase_grid_at_offset grid size is 1");
            return false;
        }
        i32c grid_sprite_id = Edit::grid_sprite_id_at_offset(offset);
        if (grid_sprite_id == -1) return false;

        Console::log("state::Edit::erase_grid_at_offset sprite_id: ", grid_sprite_id, "\n");

        bool is_found = false;
        std::vector<i32> resized_grid_sprite_ids;
        for (auto& i : m_grid_sprite_ids) {
            if (i == grid_sprite_id) {
                is_found = true;
            } else {
                resized_grid_sprite_ids.push_back(i);
            }
        }
        if (!is_found) {
            Console::log("state::Edit::erase_grid_at_offset grid_sprite_id not found ", grid_sprite_id, "\n");
            return false;
        }
        m_grid_sprite_ids.clear();
        m_grid_sprite_ids = resized_grid_sprite_ids;
        Console::log("state::Edit::erase_grid_at_offset grid_sprite_ids size: ", m_grid_sprite_ids.size(), "\n");
        sprite::Set::erase(grid_sprite_id);

        //Console::log("state::Edit::erase_grid_at_offset ", offset.x, " ", offset.y, "\n");        

        Vec2f grid_offset = offset / 16.0f;
        //Console::log("state::Edit::erase_grid_at_offset grid_offset: ", grid_offset.x, " ", grid_offset.y, "\n");
        //return true;

        i32c grid_map_sprite_id = Edit::grid_map_sprite_id_at_offset(grid_offset);
        if (grid_map_sprite_id == -1) return false;
        if (!is_found) {
            Console::log("state::Edit::erase_grid_at_offset grid_map_sprite_id not found ", grid_map_sprite_id, "\n");
            return false;
        }
        is_found = false;
        std::vector<i32> resized_grid_map_sprite_ids;
        for (auto& i : m_grid_map_sprite_ids) {
            if (i == grid_map_sprite_id) {
                is_found = true;
            } else {
                resized_grid_map_sprite_ids.push_back(i);
            }
        }

        m_grid_map_sprite_ids.clear();
        m_grid_map_sprite_ids = resized_grid_map_sprite_ids;
        Console::log("state::Edit::erase_grid_at grid_map_sprite_ids size: ", m_grid_map_sprite_ids.size(), "\n");
        sprite::Set::erase(grid_map_sprite_id);

        return true;
    }
    bool Edit::clear_grid_sprites() {
        /*if (m_grid_transform_id != -1) {
            transform::Set::erase(m_grid_transform_id);
            m_grid_transform_id = -1;
        }*/

        /*for (size_t i = 1; i < m_grid_sprite_ids.size(); ++i) {
            sprite::Set::erase(m_grid_sprite_ids.at(i));
        }
        m_grid_sprite_ids.resize(1);
        for (size_t i = 1; i < m_grid_map_sprite_ids.size(); ++i) {
            sprite::Set::erase(m_grid_sprite_ids.at(i));
        }
        m_grid_map_sprite_ids.resize(1);*/

        for (auto& i : m_grid_sprite_ids) {
            if (!sprite::Set::at(i)) continue;
            if (sprite::Set::at(i)->transform_id == m_grid_transform_id) {
                sprite::Set::erase(i);
            }
        }        
        for (auto& i : m_grid_map_sprite_ids) {
            if (!sprite::Set::at(i)) continue;
            if (sprite::Set::at(i)->transform_id == m_grid_map_transform_id) {
                sprite::Set::erase(i);
            }
        }
        m_grid_sprite_ids.clear();
        m_grid_map_sprite_ids.clear();
        return true;
    }
}