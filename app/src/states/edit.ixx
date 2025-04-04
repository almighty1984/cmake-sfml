module;
#include <filesystem>
#include <vector>
#include <string>
#include <map>

export module state.edit;

export import state;
import bitmap_text;
import config;
import console;
import inputs;
import sprite;
import sprites;
import transforms;
import types;

enum class PreviousAct { null = 0, placed, replaced, erased, moved };

std::string_view as_string(PreviousAct act) {
    switch (act) {
    case PreviousAct::placed:   return "placed";
    case PreviousAct::replaced: return "replaced";
    case PreviousAct::erased:   return "erased";
    case PreviousAct::moved:    return "moved";
    default: return "";
    }
}

struct MenuList {
    i32    transform_id = -1,
           bg_sprite_id = -1;
    u16    bg_w = 8,
           bg_h = 8;
    std::vector<std::unique_ptr<BitmapText>> items;
};

struct UndoInfo {
    i32   transform_id  = -1;
    u8    layer         =  0,
          tile_set      =  0;
    Recti source_rect   = { 0, 0, 0, 0 };
    Vec2f offset        = { 0.0f, 0.0f };

    bool operator ==(const UndoInfo& other) {
        return (layer == other.layer && tile_set == other.tile_set && source_rect == other.source_rect && offset == other.offset);
    }
    bool operator !=(const UndoInfo& other) {
        return !operator ==(other);
    }
};

export namespace state {
    constexpr u8 GRID_LAYER                  = 10,
                 GRID_MAP_LAYER              = 10,
                 MENU_LAYER                  = 12,                 
                 TILE_SET_BG_LAYER           = 11,
                 TILE_SET_LAYER              = 11,
                 SELECTION_ON_LEVEL_LAYER    = 10,
                 SELECTION_ON_TILE_SET_LAYER = 12,
                 MOUSE_LAYER                 = 13;

    class Edit : public State {
    public:
        Edit();
        ~Edit();

    private:
        i32    m_input_id = -1;

        i32    m_mouse_transform_id                 = -1,
               m_level_transform_id                 = -1,
               m_tile_set_transform_id              = -1,
               m_tile_set_bg_transform_id           = -1,
               m_grid_transform_id                  = -1, m_grid_map_transform_id = -1,
               m_menu_up_transform_id               = -1,
               m_menu_down_transform_id             = -1,
               m_selection_on_tile_set_transform_id = -1;

        i32    m_menu_down_bg_sprite_id           = -1, m_menu_right_bg_sprite_id = -1,
               m_position_on_grid_map_sprite_id   = -1,
               m_save_sprite_id                   = -1,
               m_tile_set_sprite_id               = -1, m_tile_set_bg_sprite_id   = -1,
               m_active_layer_sprite_id           = -1,
               m_current_level_bg_sprite_id       = -1,
               m_current_tile_set_bg_sprite_id    = -1;

        std::string                     m_menu_up_labels[2] = { "Open ", "Import" };
        std::map<std::string, MenuList> m_menu_up_lists;

        size_t m_typing_pos_current_level = 12;

        std::vector<i32> m_is_hidden_layer_sprite_ids,
                         m_mouse_sprite_ids,
                         m_selection_on_tile_set_sprite_ids,
                         m_selection_on_level_sprite_ids,
                         m_moving_sprite_ids;

        std::vector<i32> m_grid_sprite_ids,
                         m_grid_map_sprite_ids;

        std::vector<i32> m_menu_up_bar_bg_sprite_ids;

        std::vector<std::unique_ptr<BitmapText>> m_menu_up_bar;

        BitmapText m_current_level,
                   m_current_tile_set,
                   m_info_message;

        Vec2f  m_mouse_tile_position;
        
        bool m_is_moving               = false,
             m_is_mouse_on_menu_up_bar = false, m_is_mouse_on_menu_up_list = false, m_is_mouse_on_menu_down = false,
             m_is_showing_menu_down    = true,
             m_is_showing_grid_map     = true,
             m_is_showing_tile_set     = false,
             m_is_typing_current_level = false;

        std::pair<bool, size_t> m_is_asked_to_remove_level = { false, 0 };

        u8   m_level_brush_size    = 1,
             m_tile_set_brush_size = 1,
             m_tile_set            = 0,
             m_layer               = 0;

        u16 m_menu_up_w = 0;

        u16  m_timer            = 0,
             m_time_left_saving = 0;

        std::vector<UndoInfo> m_undo_placed,
                              m_undo_replaced,
                              m_undo_erased,
                              m_undo_moved;
        
        std::vector<u16>      m_undo_counts;
        u16                   m_last_undo_count = 0;

        std::vector<PreviousAct> m_previous_acts;

        std::filesystem::path m_grid_texture_path     = "res/planes/textures/editor_grid_block.png",
                              m_grid_map_texture_path = "res/textures/tile_blue.png",
                              m_mouse_texture_path    = "res/textures/tile_selection.png",
                              m_level_path            = "res/levels/test.bin";
        
        bool is_all_valid();
        void handle_menu_up();
        bool add_to_menu_up_bar(const std::string& text);
        void load_menu_up_list(const std::string& menu, const std::filesystem::path& levels_dir_path);
        bool add_to_menu_up_list(const std::string& menu, const std::filesystem::path& level_path);
        void handle_menu_down(Vec2f position, input::Button button);
        bool init_typing_current_level();
        void handle_typing_current_level();
        bool shift_typing_pos_left();
        bool shift_typing_pos_right();
        bool insert_at_typing_pos_current_level(input::Key key, bool is_shift);
        bool erase_at_typing_pos_current_level();
        bool finish_typing_current_level_and_save();
        bool save_current_level();
        bool remove_level(const std::filesystem::path& path);
        bool quit_typing_current_level();
        void clear_typed_current_level();
        void select_tool(Vec2f position);
        void init_brush(u8 size);

        i32  grid_sprite_id_at_offset(Vec2f offset);
        i32  grid_map_sprite_id_at_offset(Vec2f offset);
        bool add_grid_sprite_id_at_offset(Vec2f offset);
        bool erase_grid_sprite_id_at_offset(Vec2f offset);
        bool clear_grid_sprites();

        void clear_mouse_sprites();

        void init_tile_set(u8c layer);
        void enter_tile_set();
        void exit_tile_set();
        bool move_tile_set(Vec2fc amount);
        bool select_on_tile_set(Vec2fc offset);
        bool deselect_on_tile_set(Vec2fc offset);
        bool move_selection_on_tile_set(Vec2fc amount);
        bool deselect_all_on_tile_set();

        void move_level(Vec2fc amount);
        void move_level_to(Vec2fc position);
        bool select_on_level(Vec2fc offset);
        void deselect_on_level(Vec2fc offset);

        std::vector<i32> find_sprites_in_selection_on_level();

        void copy_selected_start_moving_on_level();
        void start_moving_selected_on_level();
        void clear_selected_on_level();
        void move_selected_on_level(Vec2fc amount);
        void finish_moving_selected_on_level();
        void deselect_all_on_level();
        bool level_eyedropper_at_offset(Vec2fc offset);
        Sprite* level_sprite_at_offset(u8c layer, Vec2fc offset);

        PreviousAct place_sprite_on_level(u8c layer, u8c tile_set, Rectic source_rect, Vec2fc offset);
        PreviousAct erase_sprite_on_level(u8c layer, Vec2fc offset);
        
        void undo_previous_act();
        void clear_level_sprites();
        void load_level_sprites_from(const std::filesystem::path& path);
        void save_level_sprites_to(const std::filesystem::path& path);

        void update() override;
    };
}