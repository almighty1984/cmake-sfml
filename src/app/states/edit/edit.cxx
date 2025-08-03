module;
#include <filesystem>
#include <sstream>

module state.edit;

import console;
import transform;
import sprite;
import types;

namespace state {
    Edit::Edit(u16c window_w, u16c window_h, const std::filesystem::path& path) {
        Console::log("state::Edit()\n");
        for (u8 i = 0; i < NUM_VISIBLE_LAYERS; ++i) {
            add_visible_layer(i);
        }
        current_state(state::Type::edit);


        //load_types_from_text_file("edit.cfg");

        load_types_from_bin();

        m_input_id = input::Set::make();
        m_level_transform_id = transform::Set::make();

        Console::log("state::Edit level transform id: ", m_level_transform_id, "\n");
        


        m_selection_on_tile_set_transform_id = transform::Set::make();
        transform::Set::at(m_selection_on_tile_set_transform_id)->position = { 0.0f, 512.0f };
        //m_selection_on_tile_set_sprite_ids.push_back(sprite::Set::make("res/texture/tile_selection.png"));
        //sprite::Set::at(m_selection_on_tile_set_sprite_ids.back())->layer = SELECTION_ON_TILE_SET_LAYER;
        //sprite::Set::at(m_selection_on_tile_set_sprite_ids.back())->transform_id = m_selection_on_tile_set_transform_id;

        m_tile_set_bg_transform_id = transform::Set::make();
        transform::Set::at(m_tile_set_bg_transform_id)->position = { 0.0f, 512.0f };
        m_tile_set_bg_sprite_id = sprite::Set::make("res/texture/editor_tile_set_bg.png");
        sprite::Set::at(m_tile_set_bg_sprite_id)->source_rect = { 0, 0, 512, 512 };
        sprite::Set::at(m_tile_set_bg_sprite_id)->layer = TILE_SET_BG_LAYER;
        sprite::Set::at(m_tile_set_bg_sprite_id)->transform_id = m_tile_set_bg_transform_id;

        m_tile_set_transform_id = transform::Set::make();
        transform::Set::at(m_tile_set_transform_id)->position = { 0.0f, 512.0f };
        m_tile_set_sprite_id = sprite::Set::make(tile_set_texture_path(0));
        sprite::Set::at(m_tile_set_sprite_id)->source_rect = { 0, 0, 512, 512 };
        sprite::Set::at(m_tile_set_sprite_id)->layer = TILE_SET_LAYER;
        sprite::Set::at(m_tile_set_sprite_id)->transform_id = m_tile_set_transform_id;
        sprite::Set::at(m_tile_set_sprite_id)->is_hidden = true;

        m_menu_up_transform_id = transform::Set::make();
        transform::Set::at(m_menu_up_transform_id)->position = { 0.0f, -8.0f };

        add_to_menu_up_bar(m_menu_up_labels[0]);
        add_to_menu_up_bar(m_menu_up_labels[1]);

        m_menu_up_lists[m_menu_up_labels[0]].transform_id = transform::Set::make();        
        m_menu_up_lists[m_menu_up_labels[0]].bg_sprite_id = sprite::Set::make("res/texture/editor_menu_bg.png");        

        Console::log("state::Edit::Edit bg_h: ", m_menu_up_lists[m_menu_up_labels[0]].bg_h, "\n");

        sprite::Set::at(m_menu_up_lists[m_menu_up_labels[0]].bg_sprite_id)->layer = MENU_LAYER;
        sprite::Set::at(m_menu_up_lists[m_menu_up_labels[0]].bg_sprite_id)->transform_id = m_menu_up_lists[m_menu_up_labels[0]].transform_id;

        m_menu_up_lists[m_menu_up_labels[1]].transform_id = transform::Set::make();
        m_menu_up_lists[m_menu_up_labels[1]].bg_sprite_id = sprite::Set::make("res/texture/editor_menu_bg.png");        
        sprite::Set::at(m_menu_up_lists[m_menu_up_labels[1]].bg_sprite_id)->layer = MENU_LAYER;
        sprite::Set::at(m_menu_up_lists[m_menu_up_labels[1]].bg_sprite_id)->transform_id = m_menu_up_lists[m_menu_up_labels[1]].transform_id;

        load_menu_up_list(m_menu_up_labels[0], std::filesystem::current_path() / "res" / "level");
        load_menu_up_list(m_menu_up_labels[1], std::filesystem::current_path() / "res" / "prefab");                

        m_menu_down_transform_id = transform::Set::make();
        transform::Set::at(m_menu_down_transform_id)->position = { 0.0f, 160.0f };
        m_menu_down_bg_sprite_id = sprite::Set::make("res/texture/editor_menu_bg.png");
        sprite::Set::at(m_menu_down_bg_sprite_id)->offset = { 0.0f, 0.0f };
        sprite::Set::at(m_menu_down_bg_sprite_id)->source_rect = { 0, 0, 512, 32 };
        sprite::Set::at(m_menu_down_bg_sprite_id)->layer = MENU_LAYER;
        sprite::Set::at(m_menu_down_bg_sprite_id)->transform_id = m_menu_down_transform_id;

        m_menu_right_transform_id = transform::Set::make();
        transform::Set::at(m_menu_right_transform_id)->position = { 288.0f, 0.0f };
        m_menu_right_bg_sprite_id = sprite::Set::make("res/texture/editor_menu_bg.png");
        sprite::Set::at(m_menu_right_bg_sprite_id)->offset = { 0, 0 };
        sprite::Set::at(m_menu_right_bg_sprite_id)->source_rect = { 0, 0, 32, 512 };
        sprite::Set::at(m_menu_right_bg_sprite_id)->layer = MENU_LAYER;
        sprite::Set::at(m_menu_right_bg_sprite_id)->transform_id = m_menu_right_transform_id;

        m_save_sprite_id = sprite::Set::make("res/texture/editor_save.png");
        sprite::Set::at(m_save_sprite_id)->offset = { 272.0f, 0.0f };
        sprite::Set::at(m_save_sprite_id)->layer = MENU_LAYER;
        sprite::Set::at(m_save_sprite_id)->transform_id = m_menu_down_transform_id;

        m_active_layer_sprite_id = sprite::Set::make("res/texture/editor_layer_active.png");
        sprite::Set::at(m_active_layer_sprite_id)->offset = { 0.0f, 0.0f };
        sprite::Set::at(m_active_layer_sprite_id)->source_rect = { 0, 0, 16, 16 };
        sprite::Set::at(m_active_layer_sprite_id)->layer = MENU_LAYER;
        sprite::Set::at(m_active_layer_sprite_id)->transform_id = m_menu_right_transform_id;

        for (u8 i = 0; i < 10; ++i) {
            m_is_hidden_layer_sprite_ids.push_back(sprite::Set::make("res/texture/editor_layer_hidden.png"));
            sprite::Set::at(m_is_hidden_layer_sprite_ids.back())->offset = { 16.0f, i * 16.0f };
            sprite::Set::at(m_is_hidden_layer_sprite_ids.back())->source_rect = { 0, 0, 16, 16 };
            sprite::Set::at(m_is_hidden_layer_sprite_ids.back())->layer = MENU_LAYER;
            sprite::Set::at(m_is_hidden_layer_sprite_ids.back())->transform_id = m_menu_right_transform_id;
        }

        m_text_bar_bg_sprite_id = sprite::Set::make("res/texture/editor_text_bar_bg.png");
        sprite::Set::at(m_text_bar_bg_sprite_id)->offset = { 0.0f, 0.0f };
        sprite::Set::at(m_text_bar_bg_sprite_id)->source_rect = { 0, 0, 272, 16 };
        sprite::Set::at(m_text_bar_bg_sprite_id)->layer = MENU_LAYER;
        sprite::Set::at(m_text_bar_bg_sprite_id)->transform_id = m_menu_down_transform_id;

        m_text_bar.transform_id = m_menu_down_transform_id;
        m_text_bar.set_offset({ 0.0f, 4.0f });
        m_text_bar.layer = MENU_LAYER;
        m_text_bar.texture_path = "res/texture/font_8_black.png";        

        m_text_current_tile_set_bg_sprite_id = sprite::Set::make("res/texture/editor_text_current_tile_set_bg.png");
        sprite::Set::at(m_text_current_tile_set_bg_sprite_id)->offset = { 0.0f, 0.0f };
        sprite::Set::at(m_text_current_tile_set_bg_sprite_id)->source_rect = { 0, 0, 32, 16 };
        sprite::Set::at(m_text_current_tile_set_bg_sprite_id)->layer = MENU_LAYER;
        sprite::Set::at(m_text_current_tile_set_bg_sprite_id)->transform_id = m_menu_right_transform_id;
        sprite::Set::at(m_text_current_tile_set_bg_sprite_id)->is_hidden = true;

        m_text_current_tile_set.transform_id = m_menu_right_transform_id;
        m_text_current_tile_set.layer = MENU_LAYER;
        m_text_current_tile_set.texture_path = "res/texture/font_8_black.png";
        m_text_current_tile_set.set_is_hidden(true);
        m_text_current_tile_set.set_offset( {4.0f, 4.0f} );
        m_text_current_tile_set.set_text("0");

        m_info_message.transform_id = m_menu_up_transform_id;
        m_info_message.set_is_hidden(true);



        // FIXME: causes sprites error when added earlier
        m_grid_transform_id = transform::Set::make();
        m_grid_map_transform_id = transform::Set::make();
        add_grid_at_offset({ 0.0f, 0.0f });


        m_position_on_grid_map_sprite_id = sprite::Set::make("res/texture/tile_yellow.png");
        sprite::Set::at(m_position_on_grid_map_sprite_id)->source_rect = { 0, 0, 1, 1 };
        sprite::Set::at(m_position_on_grid_map_sprite_id)->offset = Vec2f{ 7.0f, 7.0f };
        sprite::Set::at(m_position_on_grid_map_sprite_id)->transform_id = m_grid_map_transform_id;
        sprite::Set::at(m_position_on_grid_map_sprite_id)->layer = GRID_LAYER + 1;


        m_mouse_transform_id = transform::Set::make();
        m_mouse_sprite_ids.push_back(sprite::Set::make(m_mouse_texture_path));
        sprite::Set::at(m_mouse_sprite_ids.back())->transform_id = m_mouse_transform_id;
        sprite::Set::at(m_mouse_sprite_ids.back())->layer = MOUSE_LAYER;

        load_level_sprites(path);

        m_level_path = path;
    }
    Edit::~Edit() {
        Console::log("state::Edit::~Edit()\n");
        input::Set::clear();
        sprite::Set::clear();
        transform::Set::clear();
    }
    /*void Edit::clear_mouse_sprites() {
        Console::log("state::Edit::clear_mouse_sprites\n");
        for (auto& i : m_mouse_sprite_ids) {
            sprite::Set::erase(i);
        }
        m_mouse_sprite_ids.clear();        
    }*/
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
            sprite::Set::is_valid(m_text_current_tile_set_bg_sprite_id)) {            
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