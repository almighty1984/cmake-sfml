module;
#include <filesystem>
#include <sstream>

module state.edit;

import console;
import transforms;
import sprites;
import types;

namespace state {
    Edit::Edit() {
        Console::log("state::Edit()\n");
        for (u8 i = 0; i < NUM_VISIBLE_LAYERS; ++i) {
            add_visible_layer(i);
        }
        set_current(state::Type::Edit);

        m_input_id = Inputs::make();
        m_level_transform_id = Transforms::make();

        Console::log("state::Edit level transform id: ", m_level_transform_id, "\n");
        
        m_mouse_transform_id = Transforms::make();        
        m_mouse_sprite_ids.push_back(Sprites::make(m_mouse_texture_path));
        Sprites::at(m_mouse_sprite_ids.back())->transform_id = m_mouse_transform_id;
        Sprites::at(m_mouse_sprite_ids.back())->layer = MOUSE_LAYER;
                
        m_grid_transform_id = Transforms::make();        
        m_grid_sprite_ids.push_back(Sprites::make(m_grid_texture_path));
        Sprites::at(m_grid_sprite_ids.back())->source_rect = { 0, 0, 256, 256 };
        Sprites::at(m_grid_sprite_ids.back())->transform_id = m_grid_transform_id;
        Sprites::at(m_grid_sprite_ids.back())->layer = GRID_LAYER;
                
        m_grid_map_transform_id = Transforms::make();
        m_grid_map_sprite_ids.push_back(Sprites::make(m_grid_map_texture_path));
        Sprites::at(m_grid_map_sprite_ids.back())->source_rect = { 0, 0, 16, 16 };
        Sprites::at(m_grid_map_sprite_ids.back())->transform_id = m_grid_map_transform_id;
        Sprites::at(m_grid_map_sprite_ids.back())->layer = GRID_LAYER + 1;

        m_position_on_grid_map_sprite_id = Sprites::make("res/textures/tile_yellow.png");
        Sprites::at(m_position_on_grid_map_sprite_id)->source_rect = { 0, 0, 1, 1 };
        Sprites::at(m_position_on_grid_map_sprite_id)->offset = Vec2f{ 7.0f, 7.0f };
        Sprites::at(m_position_on_grid_map_sprite_id)->transform_id = m_grid_map_transform_id;
        Sprites::at(m_position_on_grid_map_sprite_id)->layer = GRID_LAYER + 1;

        m_selection_on_tile_set_transform_id = Transforms::make();
        Transforms::at(m_selection_on_tile_set_transform_id)->position = { 0.0f, 512.0f };
        //m_selection_on_tile_set_sprite_ids.push_back(Sprites::make("res/textures/tile_selection.png"));
        //Sprites::at(m_selection_on_tile_set_sprite_ids.back())->layer = SELECTION_ON_TILE_SET_LAYER;
        //Sprites::at(m_selection_on_tile_set_sprite_ids.back())->transform_id = m_selection_on_tile_set_transform_id;

        m_tile_set_bg_transform_id = Transforms::make();
        Transforms::at(m_tile_set_bg_transform_id)->position = { 0.0f, 512.0f };
        m_tile_set_bg_sprite_id = Sprites::make("res/planes/textures/editor_tile_set_bg.png");
        Sprites::at(m_tile_set_bg_sprite_id)->source_rect = { 0, 0, 512, 512 };
        Sprites::at(m_tile_set_bg_sprite_id)->layer = TILE_SET_BG_LAYER;
        Sprites::at(m_tile_set_bg_sprite_id)->transform_id = m_tile_set_bg_transform_id;

        m_tile_set_transform_id = Transforms::make();
        Transforms::at(m_tile_set_transform_id)->position = { 0.0f, 512.0f };
        m_tile_set_sprite_id = Sprites::make(tile_set_texture_path(0));
        Sprites::at(m_tile_set_sprite_id)->source_rect = { 0, 0, 512, 512 };
        Sprites::at(m_tile_set_sprite_id)->layer = TILE_SET_LAYER;
        Sprites::at(m_tile_set_sprite_id)->transform_id = m_tile_set_transform_id;

        m_menu_up_transform_id = Transforms::make();

        add_to_menu_up_bar(m_menu_up_labels[0]);
        add_to_menu_up_bar(m_menu_up_labels[1]);

        m_menu_up_lists[m_menu_up_labels[0]].transform_id = Transforms::make();
        m_menu_up_lists[m_menu_up_labels[0]].bg_sprite_id = Sprites::make("res/planes/textures/editor_menu_bg.png");
        Sprites::at(m_menu_up_lists[m_menu_up_labels[0]].bg_sprite_id)->layer = MENU_LAYER;
        Sprites::at(m_menu_up_lists[m_menu_up_labels[0]].bg_sprite_id)->transform_id = m_menu_up_lists[m_menu_up_labels[0]].transform_id;

        m_menu_up_lists[m_menu_up_labels[1]].transform_id = Transforms::make();
        m_menu_up_lists[m_menu_up_labels[1]].bg_sprite_id = Sprites::make("res/planes/textures/editor_menu_bg.png");
        Sprites::at(m_menu_up_lists[m_menu_up_labels[1]].bg_sprite_id)->layer = MENU_LAYER;
        Sprites::at(m_menu_up_lists[m_menu_up_labels[1]].bg_sprite_id)->transform_id = m_menu_up_lists[m_menu_up_labels[1]].transform_id;

        load_menu_up_list(m_menu_up_labels[0], std::filesystem::current_path() / "res" / "levels");
        load_menu_up_list(m_menu_up_labels[1], std::filesystem::current_path() / "res" / "prefabs");

        m_menu_down_transform_id = Transforms::make();
        //Transforms::at(m_menu_down_transform_id)->position = { 0.0f, 192.0f };
        m_menu_down_bg_sprite_id = Sprites::make("res/planes/textures/editor_menu_bg.png");
        Sprites::at(m_menu_down_bg_sprite_id)->offset = { 32.0f, 192.0f };
        Sprites::at(m_menu_down_bg_sprite_id)->source_rect = { 0, 0, 512, 512 };
        Sprites::at(m_menu_down_bg_sprite_id)->layer = MENU_LAYER;
        Sprites::at(m_menu_down_bg_sprite_id)->transform_id = m_menu_down_transform_id;
        m_menu_right_bg_sprite_id = Sprites::make("res/planes/textures/editor_menu_bg.png");
        Sprites::at(m_menu_right_bg_sprite_id)->offset = { 320, 32 };
        Sprites::at(m_menu_right_bg_sprite_id)->source_rect = { 0, 0, 512, 512 };
        Sprites::at(m_menu_right_bg_sprite_id)->layer = MENU_LAYER;
        Sprites::at(m_menu_right_bg_sprite_id)->transform_id = m_menu_down_transform_id;

        m_save_sprite_id = Sprites::make("res/textures/editor_save.png");
        Sprites::at(m_save_sprite_id)->offset = { 32.0f, 192.0f };
        Sprites::at(m_save_sprite_id)->layer = MENU_LAYER;
        Sprites::at(m_save_sprite_id)->transform_id = m_menu_down_transform_id;

        m_active_layer_sprite_id = Sprites::make("res/textures/editor_layer_active.png");
        Sprites::at(m_active_layer_sprite_id)->offset = { 320, 32 };
        Sprites::at(m_active_layer_sprite_id)->source_rect = { 0, 0, 16, 16 };
        Sprites::at(m_active_layer_sprite_id)->layer = MENU_LAYER;
        Sprites::at(m_active_layer_sprite_id)->transform_id = m_menu_down_transform_id;

        for (u8 i = 0; i < 10; ++i) {
            m_is_hidden_layer_sprite_ids.push_back(Sprites::make("res/textures/editor_layer_hidden.png"));
            Sprites::at(m_is_hidden_layer_sprite_ids.back())->offset = { 336.0f, i * 16.0f + 32.0f };
            Sprites::at(m_is_hidden_layer_sprite_ids.back())->source_rect = { 0, 0, 16, 16 };
            Sprites::at(m_is_hidden_layer_sprite_ids.back())->layer = MENU_LAYER;
            Sprites::at(m_is_hidden_layer_sprite_ids.back())->transform_id = m_menu_down_transform_id;
        }        

        m_current_level_bg_sprite_id = Sprites::make("res/textures/editor_current_level_bg.png");
        Sprites::at(m_current_level_bg_sprite_id)->offset = { 48.0f, 192.0f };
        Sprites::at(m_current_level_bg_sprite_id)->source_rect = { 0, 0, 272, 16 };
        Sprites::at(m_current_level_bg_sprite_id)->layer = MENU_LAYER;
        Sprites::at(m_current_level_bg_sprite_id)->transform_id = m_menu_down_transform_id;

        m_current_level.transform_id = m_menu_down_transform_id;
        m_current_level.set_offset({ 52.0f, 196.0f });
        m_current_level.layer = MENU_LAYER;
        m_current_level.texture_path = "res/fonts/font_8_black.png";

        m_current_tile_set_bg_sprite_id = Sprites::make("res/textures/editor_current_tile_set_bg.png");
        Sprites::at(m_current_tile_set_bg_sprite_id)->offset = { 32.0f, 192.0f };
        Sprites::at(m_current_tile_set_bg_sprite_id)->source_rect = { 0, 0, 32, 16 };
        Sprites::at(m_current_tile_set_bg_sprite_id)->layer = MENU_LAYER;
        Sprites::at(m_current_tile_set_bg_sprite_id)->transform_id = m_menu_down_transform_id;
        Sprites::at(m_current_tile_set_bg_sprite_id)->is_hidden = true;

        m_current_tile_set.transform_id = m_menu_down_transform_id;        
        m_current_tile_set.layer = MENU_LAYER;
        m_current_tile_set.texture_path = "res/fonts/font_8_black.png";
        m_current_tile_set.set_text("0");
        m_current_tile_set.set_is_hidden(true);
        m_current_tile_set.set_offset( {36.0f, 196.0f} );

        m_info_message.transform_id = m_menu_up_transform_id;
        m_info_message.set_is_hidden(true);
    }
    Edit::~Edit() {
        Console::log("~Edit()\n");
        Inputs::clear();
        Sprites::clear();
        Transforms::clear();
    }
    void Edit::clear_mouse_sprites() {
        /*if (m_mouse_transform_id != -1) {
            Transforms::erase(m_mouse_transform_id);
            m_mouse_transform_id = -1;
        }*/
        for (auto& i : m_mouse_sprite_ids) {
            Sprites::erase(i);
        }
        m_mouse_sprite_ids.clear();        
    }
    bool Edit::is_all_valid() {
        if (Inputs::is_valid(m_input_id) &&
            Transforms::is_valid(m_mouse_transform_id) &&
            Transforms::is_valid(m_level_transform_id) &&
            Transforms::is_valid(m_tile_set_transform_id) &&
            Transforms::is_valid(m_tile_set_bg_transform_id) &&
            Transforms::is_valid(m_grid_transform_id) &&
            Transforms::is_valid(m_grid_map_transform_id) &&
            Transforms::is_valid(m_menu_up_transform_id) &&
            Transforms::is_valid(m_menu_down_transform_id) &&
            Transforms::is_valid(m_selection_on_tile_set_transform_id) &&            
            Sprites::is_valid(m_menu_down_bg_sprite_id) &&
            Sprites::is_valid(m_menu_right_bg_sprite_id) &&
            Sprites::is_valid(m_position_on_grid_map_sprite_id) &&
            Sprites::is_valid(m_save_sprite_id) &&
            Sprites::is_valid(m_tile_set_sprite_id) &&
            Sprites::is_valid(m_tile_set_bg_sprite_id) &&
            Sprites::is_valid(m_active_layer_sprite_id) &&
            Sprites::is_valid(m_current_level_bg_sprite_id) &&
            Sprites::is_valid(m_current_tile_set_bg_sprite_id)) {            
        } else {
            return false;
        }        
        for (auto& i : m_is_hidden_layer_sprite_ids) {
            if (!Sprites::is_valid(i)) return false;
        }
        for (auto& i : m_mouse_sprite_ids) {
            if (!Sprites::is_valid(i)) return false;
        }
        for (auto& i : m_selection_on_tile_set_sprite_ids) {
            if (!Sprites::is_valid(i)) return false;
        }
        for (auto& i : m_selection_on_level_sprite_ids) {
            if (!Sprites::is_valid(i)) return false;
        }
        for (auto& i : m_moving_sprite_ids) {
            if (!Sprites::is_valid(i)) return false;
        }        
        for (auto& i : m_grid_sprite_ids) {
            if (!Sprites::is_valid(i)) return false;
        }
        for (auto& i : m_grid_map_sprite_ids) {
            if (!Sprites::is_valid(i)) return false;
        }
        for (auto& i : m_menu_up_bar_bg_sprite_ids) {
            if (!Sprites::is_valid(i)) return false;
        }
        return true;
    }    
}