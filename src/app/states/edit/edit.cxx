module;
#include <filesystem>
#include <sstream>

module state.edit;

import console;
import transform;
import sprite;
import types;

namespace state {
    Edit::Edit(cU16 window_w, cU16 window_h, const std::filesystem::path& level_path) {
        Console::log("state::Edit()\n");
        for (U8 i = 0; i < NUM_VISIBLE_LAYERS; ++i) {
            m_visible_layers.insert(i);
        }
        m_current_state = m_next_state = state::Type::edit;

        //load_types_from_text_file("edit.cfg");

        load_types_from_bin();

        m_input_id = input::Manager::make();
        m_level_transform_id = transform::Manager::make();

        Console::log("state::Edit level transform id: ", m_level_transform_id, "\n");

        m_tile_set_scroll_transform_id = transform::Manager::make();        
        transform::Manager::get(m_tile_set_scroll_transform_id)->position = { 0.0F, 0.0F };
        //m_selection_on_tile_set_sprite_ids.emplace_back(sprite::Manager::make("res/texture/tile_selection.png"));
        //sprite::Manager::get(m_selection_on_tile_set_sprite_ids.back())->layer = SELECTION_ON_TILE_SET_LAYER;
        //sprite::Manager::get(m_selection_on_tile_set_sprite_ids.back())->transform_id = m_tile_set_scroll_transform_id;

        m_tile_set_bg_transform_id = transform::Manager::make();
        transform::Manager::get(m_tile_set_bg_transform_id)->position = { 0.0F, 0.0F };
        m_tile_set_bg_sprite_id = sprite::Manager::make("res/texture/editor_tile_set_bg.png");
        sprite::Manager::get(m_tile_set_bg_sprite_id)->source_rect = { 0, 0, 512, 512 };
        sprite::Manager::get(m_tile_set_bg_sprite_id)->layer = TILE_SET_BG_LAYER;
        sprite::Manager::get(m_tile_set_bg_sprite_id)->is_hidden = true;
        sprite::Manager::get(m_tile_set_bg_sprite_id)->transform_id = m_tile_set_bg_transform_id;

        m_tile_set_transform_id = transform::Manager::make();
        //transform::Manager::get(m_tile_set_transform_id)->position = { 304.0F, 240.0F };
        m_tile_set_sprite_id = sprite::Manager::make(tile_set_texture_path(0));        
        sprite::Manager::get(m_tile_set_sprite_id)->source_rect = { 0, 0, 512, 512 };        
        sprite::Manager::get(m_tile_set_sprite_id)->layer = TILE_SET_LAYER;
        sprite::Manager::get(m_tile_set_sprite_id)->is_hidden = true;
        sprite::Manager::get(m_tile_set_sprite_id)->transform_id = m_tile_set_transform_id;        


        m_menu_up_transform_id = transform::Manager::make();
        transform::Manager::get(m_menu_up_transform_id)->position = { 0.0F, -8.0F };

        add_to_menu_up_bar(m_menu_up_labels[0]);
        add_to_menu_up_bar(m_menu_up_labels[1]);

        m_menu_up_lists[m_menu_up_labels[0]].transform_id = transform::Manager::make();        
        m_menu_up_lists[m_menu_up_labels[0]].bg_sprite_id = sprite::Manager::make("res/texture/editor_menu_bg.png");        

        Console::log("state::Edit::Edit bg_h: ", m_menu_up_lists[m_menu_up_labels[0]].bg_h, "\n");

        sprite::Manager::get(m_menu_up_lists[m_menu_up_labels[0]].bg_sprite_id)->layer = MENU_LAYER;
        sprite::Manager::get(m_menu_up_lists[m_menu_up_labels[0]].bg_sprite_id)->transform_id = m_menu_up_lists[m_menu_up_labels[0]].transform_id;

        m_menu_up_lists[m_menu_up_labels[1]].transform_id = transform::Manager::make();
        m_menu_up_lists[m_menu_up_labels[1]].bg_sprite_id = sprite::Manager::make("res/texture/editor_menu_bg.png");        
        sprite::Manager::get(m_menu_up_lists[m_menu_up_labels[1]].bg_sprite_id)->layer = MENU_LAYER;
        sprite::Manager::get(m_menu_up_lists[m_menu_up_labels[1]].bg_sprite_id)->transform_id = m_menu_up_lists[m_menu_up_labels[1]].transform_id;

        load_menu_up_list(m_menu_up_labels[0], std::filesystem::current_path() / "res" / "level");
        load_menu_up_list(m_menu_up_labels[1], std::filesystem::current_path() / "res" / "prefab");                

        m_menu_down_transform_id = transform::Manager::make();
        transform::Manager::get(m_menu_down_transform_id)->position = { 0.0F, 160.0F };
        m_menu_down_bg_sprite_id = sprite::Manager::make("res/texture/editor_menu_bg.png");
        sprite::Manager::get(m_menu_down_bg_sprite_id)->offset = { 0.0F, 0.0F };
        sprite::Manager::get(m_menu_down_bg_sprite_id)->source_rect = { 0, 0, 512, 32 };
        sprite::Manager::get(m_menu_down_bg_sprite_id)->layer = MENU_LAYER;
        sprite::Manager::get(m_menu_down_bg_sprite_id)->transform_id = m_menu_down_transform_id;

        m_current_tile_sprite_id = sprite::Manager::make(tile_set_texture_path(0));
        sprite::Manager::get(m_current_tile_sprite_id)->source_rect = { 0, 0, 16, 16 };
        sprite::Manager::get(m_current_tile_sprite_id)->layer = MENU_LAYER + 1;
        sprite::Manager::get(m_current_tile_sprite_id)->transform_id = m_menu_down_transform_id;
        sprite::Manager::get(m_current_tile_sprite_id)->offset.x = view().w - 32.0f;

        m_grid_icon_sprite_id = sprite::Manager::make("res/texture/editor_tile_grid.png");
        sprite::Manager::get(m_grid_icon_sprite_id)->source_rect = { 0, 0, 16, 16 };
        sprite::Manager::get(m_grid_icon_sprite_id)->layer = MENU_LAYER + 1;
        sprite::Manager::get(m_grid_icon_sprite_id)->transform_id = m_menu_down_transform_id;

        m_menu_right_transform_id = transform::Manager::make();
        transform::Manager::get(m_menu_right_transform_id)->position = { 288.0F, 0.0F };
        m_menu_right_bg_sprite_id = sprite::Manager::make("res/texture/editor_menu_bg.png");
        sprite::Manager::get(m_menu_right_bg_sprite_id)->offset = { 0, 0 };
        sprite::Manager::get(m_menu_right_bg_sprite_id)->source_rect = { 0, 0, 32, 512 };
        sprite::Manager::get(m_menu_right_bg_sprite_id)->layer = MENU_LAYER;
        sprite::Manager::get(m_menu_right_bg_sprite_id)->transform_id = m_menu_right_transform_id;

        m_save_sprite_id = sprite::Manager::make("res/texture/editor_save.png");
        sprite::Manager::get(m_save_sprite_id)->offset = { 272.0F, 0.0F };
        sprite::Manager::get(m_save_sprite_id)->layer = MENU_LAYER;
        sprite::Manager::get(m_save_sprite_id)->transform_id = m_menu_down_transform_id;

        m_active_layer_sprite_id = sprite::Manager::make("res/texture/editor_layer_active.png");
        sprite::Manager::get(m_active_layer_sprite_id)->offset = { 0.0F, 0.0F };
        sprite::Manager::get(m_active_layer_sprite_id)->source_rect = { 0, 0, 16, 16 };
        sprite::Manager::get(m_active_layer_sprite_id)->layer = MENU_LAYER;
        sprite::Manager::get(m_active_layer_sprite_id)->transform_id = m_menu_right_transform_id;

        for (U8 i = 0; i < 10; ++i) {
            m_is_hidden_layer_sprite_ids.emplace_back(sprite::Manager::make("res/texture/editor_layer_hidden.png"));
            sprite::Manager::get(m_is_hidden_layer_sprite_ids.back())->offset = { 16.0F, i * 16.0F };
            sprite::Manager::get(m_is_hidden_layer_sprite_ids.back())->source_rect = { 0, 0, 16, 16 };
            sprite::Manager::get(m_is_hidden_layer_sprite_ids.back())->layer = MENU_LAYER;
            sprite::Manager::get(m_is_hidden_layer_sprite_ids.back())->transform_id = m_menu_right_transform_id;
        }



        m_text_bar_bg_sprite_id = sprite::Manager::make("res/texture/editor_text_bar_bg.png");
        sprite::Manager::get(m_text_bar_bg_sprite_id)->offset = { 0.0F, 0.0F };
        sprite::Manager::get(m_text_bar_bg_sprite_id)->source_rect = { 0, 0, 272, 16 };
        sprite::Manager::get(m_text_bar_bg_sprite_id)->layer = MENU_LAYER;
        sprite::Manager::get(m_text_bar_bg_sprite_id)->transform_id = m_menu_down_transform_id;

        m_text_bar.transform_id = m_menu_down_transform_id;
        m_text_bar.set_offset({ 0.0F, 4.0F });
        m_text_bar.layer = MENU_LAYER;
        m_text_bar.texture_path = "res/texture/font_8_black.png";
        m_text_bar.set_is_hidden(false);
        //m_text_bar.clear_text();
        //m_text_bar.set_text(m_level_path.string());

        m_text_current_tile_set_bg_sprite_id = sprite::Manager::make("res/texture/editor_text_current_tile_set_bg.png");
        sprite::Manager::get(m_text_current_tile_set_bg_sprite_id)->offset = { 0.0F, 0.0F };
        sprite::Manager::get(m_text_current_tile_set_bg_sprite_id)->source_rect = { 0, 0, 32, 16 };
        sprite::Manager::get(m_text_current_tile_set_bg_sprite_id)->layer = MENU_LAYER;
        sprite::Manager::get(m_text_current_tile_set_bg_sprite_id)->transform_id = m_menu_right_transform_id;
        sprite::Manager::get(m_text_current_tile_set_bg_sprite_id)->is_hidden = true;

        m_text_current_tile_set.transform_id = m_menu_right_transform_id;
        m_text_current_tile_set.layer = MENU_LAYER;
        m_text_current_tile_set.texture_path = "res/texture/font_8_black.png";
        m_text_current_tile_set.set_is_hidden(true);
        m_text_current_tile_set.set_offset( {4.0F, 4.0F} );
        m_text_current_tile_set.set_text("0");

        m_info_message.transform_id = m_menu_up_transform_id;
        m_info_message.set_is_hidden(true);


        m_grid_transform_id = transform::Manager::make();
        m_grid_map_transform_id = transform::Manager::make();
        add_grid_at_offset({ 0.0F, 0.0F });


        m_position_on_grid_map_sprite_id = sprite::Manager::make("res/texture/tile_yellow.png");
        sprite::Manager::get(m_position_on_grid_map_sprite_id)->source_rect = { 0, 0, 1, 1 };
        sprite::Manager::get(m_position_on_grid_map_sprite_id)->offset = Vec2F{ 7.0F, 7.0F };
        sprite::Manager::get(m_position_on_grid_map_sprite_id)->transform_id = m_grid_map_transform_id;
        sprite::Manager::get(m_position_on_grid_map_sprite_id)->layer = GRID_LAYER + 1;

        sprite::Manager::get(m_position_on_grid_map_sprite_id)->is_hidden = m_is_hidden_grid_map;



        m_mouse_transform_id = transform::Manager::make();
        m_mouse_sprite_ids.emplace_back(sprite::Manager::make(m_mouse_texture_path));
        sprite::Manager::get(m_mouse_sprite_ids.back())->transform_id = m_mouse_transform_id;
        sprite::Manager::get(m_mouse_sprite_ids.back())->layer = MOUSE_LAYER;

        load_level_sprites(level_path);

        m_level_path = level_path;
    }
    Edit::~Edit() {
        Console::log("state::Edit::~Edit()\n");
        //input::Manager::clear();

        input::Manager::erase(m_input_id);

        transform::Manager::erase(m_mouse_transform_id);
        transform::Manager::erase(m_level_transform_id);
        transform::Manager::erase(m_tile_set_transform_id);
        transform::Manager::erase(m_tile_set_bg_transform_id);
        transform::Manager::erase(m_grid_transform_id);
        transform::Manager::erase(m_grid_map_transform_id);
        transform::Manager::erase(m_menu_up_transform_id);
        transform::Manager::erase(m_menu_down_transform_id);
        transform::Manager::erase(m_menu_right_transform_id);
        transform::Manager::erase(m_tile_set_scroll_transform_id);

        sprite::Manager::erase(m_menu_down_bg_sprite_id);
        sprite::Manager::erase(m_menu_right_bg_sprite_id);
        sprite::Manager::erase(m_position_on_grid_map_sprite_id);
        sprite::Manager::erase(m_save_sprite_id);
        sprite::Manager::erase(m_tile_set_sprite_id);
        sprite::Manager::erase(m_tile_set_bg_sprite_id);
        sprite::Manager::erase(m_current_tile_sprite_id);
        sprite::Manager::erase(m_grid_icon_sprite_id);
        sprite::Manager::erase(m_active_layer_sprite_id);
        sprite::Manager::erase(m_text_bar_bg_sprite_id);
        sprite::Manager::erase(m_text_current_tile_set_bg_sprite_id);

        for (auto& i : m_is_hidden_layer_sprite_ids)       sprite::Manager::erase(i);
        for (auto& i : m_mouse_sprite_ids)                 sprite::Manager::erase(i);
        for (auto& i : m_selection_on_tile_set_sprite_ids) sprite::Manager::erase(i);
        for (auto& i : m_selection_on_level_sprite_ids)    sprite::Manager::erase(i);
        for (auto& i : m_moving_sprite_ids)                sprite::Manager::erase(i);

        for (auto& i : m_level_sprite_ids)                    sprite::Manager::erase(i);
        
        for (auto& i : m_grid_sprite_ids)                  sprite::Manager::erase(i);
        for (auto& i : m_grid_map_sprite_ids)              sprite::Manager::erase(i);

        for (auto& i : m_menu_up_bar_bg_sprite_ids)        sprite::Manager::erase(i);
                
        m_menu_up_lists.clear();

        m_types.clear();
    }
    /*void Edit::clear_mouse_sprites() {
        Console::log("state::Edit::clear_mouse_sprites\n");
        for (auto& i : m_mouse_sprite_ids) {
            sprite::Manager::erase(i);
        }
        m_mouse_sprite_ids.clear();        
    }*/
    bool Edit::is_all_valid() {
        if (input::Manager::is_valid(m_input_id)                               &&
            transform::Manager::is_valid(m_mouse_transform_id)                 &&
            transform::Manager::is_valid(m_level_transform_id)                 &&
            transform::Manager::is_valid(m_tile_set_transform_id)              &&
            transform::Manager::is_valid(m_tile_set_bg_transform_id)           &&
            transform::Manager::is_valid(m_grid_transform_id)                  &&
            transform::Manager::is_valid(m_grid_map_transform_id)              &&
            transform::Manager::is_valid(m_menu_up_transform_id)               &&
            transform::Manager::is_valid(m_menu_down_transform_id)             &&
            transform::Manager::is_valid(m_tile_set_scroll_transform_id)       &&

            sprite::Manager::is_valid(m_menu_down_bg_sprite_id)                &&
            sprite::Manager::is_valid(m_menu_right_bg_sprite_id)               &&
            sprite::Manager::is_valid(m_position_on_grid_map_sprite_id)        &&
            sprite::Manager::is_valid(m_save_sprite_id)                        &&
            sprite::Manager::is_valid(m_tile_set_sprite_id)                    &&
            sprite::Manager::is_valid(m_tile_set_bg_sprite_id)                 &&
            sprite::Manager::is_valid(m_current_tile_sprite_id)                &&
            sprite::Manager::is_valid(m_grid_icon_sprite_id)                   &&
            sprite::Manager::is_valid(m_active_layer_sprite_id)                &&
            sprite::Manager::is_valid(m_text_bar_bg_sprite_id)                 &&
            sprite::Manager::is_valid(m_text_current_tile_set_bg_sprite_id)) {            
        } else {
            return false;
        }        
        for (auto& i : m_is_hidden_layer_sprite_ids)       { if (!sprite::Manager::is_valid(i)) return false; }
        for (auto& i : m_mouse_sprite_ids)                 { if (!sprite::Manager::is_valid(i)) return false; }
        for (auto& i : m_selection_on_tile_set_sprite_ids) { if (!sprite::Manager::is_valid(i)) return false; }
        for (auto& i : m_selection_on_level_sprite_ids)    { if (!sprite::Manager::is_valid(i)) return false; }
        for (auto& i : m_moving_sprite_ids)                { if (!sprite::Manager::is_valid(i)) return false; }        
        for (auto& i : m_grid_sprite_ids)                  { if (!sprite::Manager::is_valid(i)) return false; }
        for (auto& i : m_grid_map_sprite_ids)              { if (!sprite::Manager::is_valid(i)) return false; }
        for (auto& i : m_menu_up_bar_bg_sprite_ids)        { if (!sprite::Manager::is_valid(i)) return false; }
        return true;
    }    
}