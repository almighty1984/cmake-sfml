module;
#include <filesystem>
#include <sstream>

module state.edit;

import console;
import transform;
import sprite;
import types;

namespace state {
    Edit::Edit(u16c window_w, u16c window_h) {
        Console::log("state::Edit()\n");
        for (u8 i = 0; i < NUM_VISIBLE_LAYERS; ++i) {
            add_visible_layer(i);
        }
        set_current(state::Type::Edit);

        //load_types_from_text_file("edit.cfg");

        load_types_from_bin();

        m_input_id = input::Set::make();
        m_level_transform_id = transform::Set::make();

        Console::log("state::Edit level transform id: ", m_level_transform_id, "\n");
        
        m_mouse_transform_id = transform::Set::make();        
        m_mouse_sprite_ids.push_back(sprite::Set::make(m_mouse_texture_path));
        sprite::Set::at(m_mouse_sprite_ids.back())->transform_id = m_mouse_transform_id;
        sprite::Set::at(m_mouse_sprite_ids.back())->layer = MOUSE_LAYER;
                
        m_grid_transform_id = transform::Set::make();        
        m_grid_sprite_ids.push_back(sprite::Set::make(m_grid_texture_path));
        sprite::Set::at(m_grid_sprite_ids.back())->source_rect = { 0, 0, 256, 256 };
        sprite::Set::at(m_grid_sprite_ids.back())->transform_id = m_grid_transform_id;
        sprite::Set::at(m_grid_sprite_ids.back())->layer = GRID_LAYER;
                
        m_grid_map_transform_id = transform::Set::make();
        m_grid_map_sprite_ids.push_back(sprite::Set::make(m_grid_map_texture_path));
        sprite::Set::at(m_grid_map_sprite_ids.back())->source_rect = { 0, 0, 16, 16 };
        sprite::Set::at(m_grid_map_sprite_ids.back())->transform_id = m_grid_map_transform_id;
        sprite::Set::at(m_grid_map_sprite_ids.back())->layer = GRID_LAYER + 1;

        m_position_on_grid_map_sprite_id = sprite::Set::make("res/textures/tile_yellow.png");
        sprite::Set::at(m_position_on_grid_map_sprite_id)->source_rect = { 0, 0, 1, 1 };
        sprite::Set::at(m_position_on_grid_map_sprite_id)->offset = Vec2f{ 7.0f, 7.0f };
        sprite::Set::at(m_position_on_grid_map_sprite_id)->transform_id = m_grid_map_transform_id;
        sprite::Set::at(m_position_on_grid_map_sprite_id)->layer = GRID_LAYER + 1;

        m_selection_on_tile_set_transform_id = transform::Set::make();
        transform::Set::at(m_selection_on_tile_set_transform_id)->position = { 0.0f, 512.0f };
        //m_selection_on_tile_set_sprite_ids.push_back(sprite::Set::make("res/textures/tile_selection.png"));
        //sprite::Set::at(m_selection_on_tile_set_sprite_ids.back())->layer = SELECTION_ON_TILE_SET_LAYER;
        //sprite::Set::at(m_selection_on_tile_set_sprite_ids.back())->transform_id = m_selection_on_tile_set_transform_id;

        m_tile_set_bg_transform_id = transform::Set::make();
        transform::Set::at(m_tile_set_bg_transform_id)->position = { 0.0f, 512.0f };
        m_tile_set_bg_sprite_id = sprite::Set::make("res/textures/editor_tile_set_bg.png");
        sprite::Set::at(m_tile_set_bg_sprite_id)->source_rect = { 0, 0, 512, 512 };
        sprite::Set::at(m_tile_set_bg_sprite_id)->layer = TILE_SET_BG_LAYER;
        sprite::Set::at(m_tile_set_bg_sprite_id)->transform_id = m_tile_set_bg_transform_id;

        m_tile_set_transform_id = transform::Set::make();
        transform::Set::at(m_tile_set_transform_id)->position = { 0.0f, 512.0f };
        m_tile_set_sprite_id = sprite::Set::make(tile_set_texture_path(0));
        sprite::Set::at(m_tile_set_sprite_id)->source_rect = { 0, 0, 512, 512 };
        sprite::Set::at(m_tile_set_sprite_id)->layer = TILE_SET_LAYER;
        sprite::Set::at(m_tile_set_sprite_id)->transform_id = m_tile_set_transform_id;

        m_menu_up_transform_id = transform::Set::make();
        transform::Set::at(m_menu_up_transform_id)->position = { 0.0f, -8.0f };

        add_to_menu_up_bar(m_menu_up_labels[0]);
        add_to_menu_up_bar(m_menu_up_labels[1]);

        m_menu_up_lists[m_menu_up_labels[0]].transform_id = transform::Set::make();        
        m_menu_up_lists[m_menu_up_labels[0]].bg_sprite_id = sprite::Set::make("res/textures/editor_menu_bg.png");        

        Console::log("state::Edit::Edit bg_h: ", m_menu_up_lists[m_menu_up_labels[0]].bg_h, "\n");

        sprite::Set::at(m_menu_up_lists[m_menu_up_labels[0]].bg_sprite_id)->layer = MENU_LAYER;
        sprite::Set::at(m_menu_up_lists[m_menu_up_labels[0]].bg_sprite_id)->transform_id = m_menu_up_lists[m_menu_up_labels[0]].transform_id;

        m_menu_up_lists[m_menu_up_labels[1]].transform_id = transform::Set::make();
        m_menu_up_lists[m_menu_up_labels[1]].bg_sprite_id = sprite::Set::make("res/textures/editor_menu_bg.png");        
        sprite::Set::at(m_menu_up_lists[m_menu_up_labels[1]].bg_sprite_id)->layer = MENU_LAYER;
        sprite::Set::at(m_menu_up_lists[m_menu_up_labels[1]].bg_sprite_id)->transform_id = m_menu_up_lists[m_menu_up_labels[1]].transform_id;

        load_menu_up_list(m_menu_up_labels[0], std::filesystem::current_path() / "res" / "levels");
        load_menu_up_list(m_menu_up_labels[1], std::filesystem::current_path() / "res" / "prefabs");                

        m_menu_down_transform_id = transform::Set::make();
        //transform::Set::at(m_menu_down_transform_id)->position = { 0.0f, 192.0f };
        m_menu_down_bg_sprite_id = sprite::Set::make("res/textures/editor_menu_bg.png");
        sprite::Set::at(m_menu_down_bg_sprite_id)->offset = { 32.0f, 192.0f };
        sprite::Set::at(m_menu_down_bg_sprite_id)->source_rect = { 0, 0, 512, 512 };
        sprite::Set::at(m_menu_down_bg_sprite_id)->layer = MENU_LAYER;
        sprite::Set::at(m_menu_down_bg_sprite_id)->transform_id = m_menu_down_transform_id;
        m_menu_right_bg_sprite_id = sprite::Set::make("res/textures/editor_menu_bg.png");
        sprite::Set::at(m_menu_right_bg_sprite_id)->offset = { 320, 32 };
        sprite::Set::at(m_menu_right_bg_sprite_id)->source_rect = { 0, 0, 512, 512 };
        sprite::Set::at(m_menu_right_bg_sprite_id)->layer = MENU_LAYER;
        sprite::Set::at(m_menu_right_bg_sprite_id)->transform_id = m_menu_down_transform_id;

        m_save_sprite_id = sprite::Set::make("res/textures/editor_save.png");
        sprite::Set::at(m_save_sprite_id)->offset = { 304.0f, 192.0f };
        sprite::Set::at(m_save_sprite_id)->layer = MENU_LAYER;
        sprite::Set::at(m_save_sprite_id)->transform_id = m_menu_down_transform_id;

        m_active_layer_sprite_id = sprite::Set::make("res/textures/editor_layer_active.png");
        sprite::Set::at(m_active_layer_sprite_id)->offset = { 320.0f, 32.0f };
        sprite::Set::at(m_active_layer_sprite_id)->source_rect = { 0, 0, 16, 16 };
        sprite::Set::at(m_active_layer_sprite_id)->layer = MENU_LAYER;
        sprite::Set::at(m_active_layer_sprite_id)->transform_id = m_menu_down_transform_id;

        for (u8 i = 0; i < 10; ++i) {
            m_is_hidden_layer_sprite_ids.push_back(sprite::Set::make("res/textures/editor_layer_hidden.png"));
            sprite::Set::at(m_is_hidden_layer_sprite_ids.back())->offset = { 336.0f, i * 16.0f + 32.0f };
            sprite::Set::at(m_is_hidden_layer_sprite_ids.back())->source_rect = { 0, 0, 16, 16 };
            sprite::Set::at(m_is_hidden_layer_sprite_ids.back())->layer = MENU_LAYER;
            sprite::Set::at(m_is_hidden_layer_sprite_ids.back())->transform_id = m_menu_down_transform_id;
        }        

        m_text_bar_bg_sprite_id = sprite::Set::make("res/textures/editor_text_bar_bg.png");
        sprite::Set::at(m_text_bar_bg_sprite_id)->offset = { 32.0f, 192.0f };
        sprite::Set::at(m_text_bar_bg_sprite_id)->source_rect = { 0, 0, 272, 16 };
        sprite::Set::at(m_text_bar_bg_sprite_id)->layer = MENU_LAYER;
        sprite::Set::at(m_text_bar_bg_sprite_id)->transform_id = m_menu_down_transform_id;

        m_text_bar.transform_id = m_menu_down_transform_id;
        m_text_bar.set_offset({ 32.0f, 196.0f });
        m_text_bar.layer = MENU_LAYER;
        m_text_bar.texture_path = "res/fonts/font_8_black.png";

        m_current_tile_set_bg_sprite_id = sprite::Set::make("res/textures/editor_current_tile_set_bg.png");
        sprite::Set::at(m_current_tile_set_bg_sprite_id)->offset = { 320.0f, 192.0f };
        sprite::Set::at(m_current_tile_set_bg_sprite_id)->source_rect = { 0, 0, 32, 16 };
        sprite::Set::at(m_current_tile_set_bg_sprite_id)->layer = MENU_LAYER;
        sprite::Set::at(m_current_tile_set_bg_sprite_id)->transform_id = m_menu_down_transform_id;
        sprite::Set::at(m_current_tile_set_bg_sprite_id)->is_hidden = true;

        m_current_tile_set.transform_id = m_menu_down_transform_id;
        m_current_tile_set.layer = MENU_LAYER;
        m_current_tile_set.texture_path = "res/fonts/font_8_black.png";
        m_current_tile_set.set_is_hidden(true);
        m_current_tile_set.set_offset( {324.0f, 196.0f} );
        m_current_tile_set.set_text("0");

        m_info_message.transform_id = m_menu_up_transform_id;
        m_info_message.set_is_hidden(true);
    }
    Edit::~Edit() {
        Console::log("state::Edit::~Edit()\n");
        input::Set::clear();
        sprite::Set::clear();
        transform::Set::clear();
    }
    void Edit::clear_mouse_sprites() {        
        for (auto& i : m_mouse_sprite_ids) {
            sprite::Set::erase(i);
        }
        m_mouse_sprite_ids.clear();        
    }
    bool Edit::is_all_valid() {
        if (input::Set::is_valid(m_input_id)                               &&
            transform::Set::is_valid(m_mouse_transform_id)                 &&
            transform::Set::is_valid(m_level_transform_id)                 &&
            transform::Set::is_valid(m_tile_set_transform_id)              &&
            transform::Set::is_valid(m_tile_set_bg_transform_id)           &&
            transform::Set::is_valid(m_grid_transform_id)                  &&
            transform::Set::is_valid(m_grid_map_transform_id)              &&
            transform::Set::is_valid(m_menu_up_transform_id)               &&
            transform::Set::is_valid(m_menu_down_transform_id)             &&
            transform::Set::is_valid(m_selection_on_tile_set_transform_id) &&            
            sprite::Set::is_valid(m_menu_down_bg_sprite_id)                &&
            sprite::Set::is_valid(m_menu_right_bg_sprite_id)               &&
            sprite::Set::is_valid(m_position_on_grid_map_sprite_id)        &&
            sprite::Set::is_valid(m_save_sprite_id)                        &&
            sprite::Set::is_valid(m_tile_set_sprite_id)                    &&
            sprite::Set::is_valid(m_tile_set_bg_sprite_id)                 &&
            sprite::Set::is_valid(m_active_layer_sprite_id)                &&
            sprite::Set::is_valid(m_text_bar_bg_sprite_id)                 &&
            sprite::Set::is_valid(m_current_tile_set_bg_sprite_id)) {            
        } else {
            return false;
        }        
        for (auto& i : m_is_hidden_layer_sprite_ids)       { if (!sprite::Set::is_valid(i)) return false; }
        for (auto& i : m_mouse_sprite_ids)                 { if (!sprite::Set::is_valid(i)) return false; }
        for (auto& i : m_selection_on_tile_set_sprite_ids) { if (!sprite::Set::is_valid(i)) return false; }
        for (auto& i : m_selection_on_level_sprite_ids)    { if (!sprite::Set::is_valid(i)) return false; }
        for (auto& i : m_moving_sprite_ids)                { if (!sprite::Set::is_valid(i)) return false; }        
        for (auto& i : m_grid_sprite_ids)                  { if (!sprite::Set::is_valid(i)) return false; }
        for (auto& i : m_grid_map_sprite_ids)              { if (!sprite::Set::is_valid(i)) return false; }
        for (auto& i : m_menu_up_bar_bg_sprite_ids)        { if (!sprite::Set::is_valid(i)) return false; }
        return true;
    }    
}