module;
#include <filesystem>
#include <vector>
#include <string>

export module state.edit;

export import state;
import bitmap_text;
import config;
import console;
import inputs;
import sprites;
import transforms;
import types;

enum class SelectionAct { null = 0, clear, copy, move };

export namespace state {
    constexpr u8 GRID_LAYER               = 10,
                 GRID_MAP_LAYER           = 11,
                 MENU_LAYER               = 12,
                 SET_LAYER                = 11,
                 SELECTION_ON_LEVEL_LAYER = 10,
                 SELECTION_ON_SET_LAYER   = 13,
                 MOUSE_LAYER              = 13;

    class Edit : public State {
        size_t m_input_id;

        size_t m_level_transform_id,
               m_tile_set_transform_id,
               m_tile_set_bg_transform_id,
               m_grid_transform_id, m_grid_map_transform_id,
               m_menu_up_transform_id, m_menu_down_transform_id;

        size_t m_selection_on_set_transform_id, m_selection_on_level_transform_id;

        size_t m_mouse_transform_id;

        size_t m_menu_up_bg_sprite_id,
               m_menu_down_bg_sprite_id, m_menu_right_bg_sprite_id,
               m_save_sprite_id, m_open_sprite_id,               
               m_tile_set_sprite_id, m_tile_set_bg_sprite_id,
               m_active_layer_sprite_id;

        std::vector<size_t> m_is_hidden_layer_sprite_ids,
                            m_mouse_sprite_ids,
                            m_selection_on_set_sprite_ids,
                            m_selection_on_level_sprite_ids,
                            m_moving_sprite_ids;

        std::vector<size_t> m_grid_sprite_ids,
                            m_grid_map_sprite_ids;

        std::vector<std::unique_ptr<BitmapText>> m_level_list;

        Vec2f  m_mouse_tile_position;

        std::filesystem::path m_grid_texture_path     = "res/planes/textures/editor_grid_block.png",
                              m_grid_map_texture_path = "res/tiles/editor_tool.png",
                              m_mouse_texture_path    = "res/tiles/editor_selection.png",
                              m_level_path            = "res/levels/test.bin";

        bool m_is_moving             = false,
             m_is_mouse_on_menu_up   = false, m_is_mouse_on_menu_down = false,
             m_is_showing_menu_up    = false, m_is_showing_menu_down  = true,
             m_is_showing_map        = true,
             m_is_showing_tile_set   = false;

        u8   m_level_brush_size = 1,
             m_set_brush_size   = 1,
             m_tile_set         = 0,
             m_layer            = 0;

        u16  m_timer = 0;

        u16  m_time_left_opening = 0,
             m_time_left_saving  = 0;

        std::filesystem::path tile_set_texture_path(u8c number) { return "res/tiles/set_" + std::to_string((int)number) + ".png"; }

    public:
        ~Edit();
        Edit();
        bool   is_all_valid();
        void   add_to_level_list(const std::string& level_path);
        void   handle_menu_click(Vec2f position);
        void   select_tool(Vec2f position);
        void   init_brush(u8 size);

        size_t grid_sprite_id_at_offset(Vec2f offset);
        size_t grid_map_sprite_id_at_offset(Vec2f offset);
        bool   add_grid_sprite_id_at_offset(Vec2f offset);
        bool   erase_grid_sprite_id_at_offset(Vec2f offset);

        void   select_on_set(Vec2f position);
        void   clear_selection_on_set();
        
        void   select_on_level(Vec2f offset);
        void   deselect_on_level(Vec2f offset);
        void   act_on_selection_on_level(SelectionAct act);
        void   move_selected_on_level(Vec2f move);
        void   finish_moving_on_level();
        void   clear_selection_on_level();
        size_t level_sprite_id_at_offset(u8c layer, Vec2fc offset);
        Vec2f  level_sprite_source_at_offset(u8c layer, Vec2f offset);
        void   place_sprite_on_level_at_offset(Vec2f offset);
        void   erase_sprite_on_level_at_offset(Vec2f offset);
        void   load_level_sprites_from(const std::filesystem::path& path);
        void   save_level_sprites_to(const std::filesystem::path& path);

        void update() override;
    };
}