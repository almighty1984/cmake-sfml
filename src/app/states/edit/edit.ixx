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
import input;
import sprite;
import transform;
import types;

namespace undo {
    enum class Act { null = 0, placed, replaced, erased, moved };
    const std::string_view as_string(const Act& act) {
        switch (act) {
        case undo::Act::placed:   return "placed";
        case undo::Act::replaced: return "replaced";
        case undo::Act::erased:   return "erased";
        case undo::Act::moved:    return "moved";
        default: return "";
        }
    }
    struct Info {
        i32   transform_id = -1;
        u8    layer        =  0,
              tile_set     =  0;
        Recti source_rect  = { 0, 0, 0, 0 };
        Vec2f offset       = { 0.0f, 0.0f };

        bool operator ==(const Info& other) {
            return (layer == other.layer && tile_set == other.tile_set && source_rect == other.source_rect && offset == other.offset);
        }
        bool operator !=(const Info& other) {
            return !operator ==(other);
        }
    };
};

namespace entity {
    struct Info {
        u8  set    = 0;
        u16 number = 0;

        bool operator <(const Info& other)  const {
            return set < other.set ? true : number < other.number;
        }
    };
}

struct MenuList {
    i32    transform_id = -1,
           bg_sprite_id = -1;
    u16    bg_w         =  8,
           bg_h         =  8;
    std::vector<std::unique_ptr<BitmapText>> items;
};

export namespace state {
    constexpr u8 GRID_LAYER                  = 10,
                 GRID_MAP_LAYER              = 10,
                 SELECTION_ON_LEVEL_LAYER    = 10,
                 TILE_SET_BG_LAYER           = 11,
                 TILE_SET_LAYER              = 11,                 
                 SELECTION_ON_TILE_SET_LAYER = 11,
                 MENU_LAYER                  = 12,
                 MOUSE_LAYER                 = 13;

    class Edit : public State {
    public:
        Edit(u16c window_w, u16c window_h, const std::filesystem::path& path);
        /*Edit(u16c window_w, u16c window_h, const std::filesystem::path& path) {
            Edit(window_w, window_h);
            load_level_sprites(path);
        }*/
        ~Edit();

    private:
        i32 m_input_id = -1;

        i32 m_mouse_transform_id                 = -1,
            m_level_transform_id                 = -1,
            m_tile_set_transform_id              = -1,
            m_tile_set_bg_transform_id           = -1,
            m_grid_transform_id                  = -1, m_grid_map_transform_id   = -1,
            m_menu_up_transform_id               = -1,
            m_menu_down_transform_id             = -1, m_menu_right_transform_id = -1,
            m_selection_on_tile_set_transform_id = -1;

        i32 m_menu_down_bg_sprite_id             = -1, m_menu_right_bg_sprite_id = -1,
            m_position_on_grid_map_sprite_id     = -1,
            m_save_sprite_id                     = -1,
            m_tile_set_sprite_id                 = -1, m_tile_set_bg_sprite_id   = -1,
            m_active_layer_sprite_id             = -1,
            m_text_bar_bg_sprite_id              = -1,
            m_text_current_tile_set_bg_sprite_id = -1;

        std::string                     m_menu_up_labels[2] = { "Open ", "Import" };
        std::map<std::string, MenuList> m_menu_up_lists;

        size_t m_typing_pos = 0;

        std::vector<i32> m_is_hidden_layer_sprite_ids,
                         m_mouse_sprite_ids,
                         m_selection_on_tile_set_sprite_ids,
                         m_selection_on_level_sprite_ids,
                         m_moving_sprite_ids;

        std::vector<i32> m_grid_sprite_ids,
                         m_grid_map_sprite_ids;

        std::vector<i32> m_menu_up_bar_bg_sprite_ids;

        std::vector<std::unique_ptr<BitmapText>> m_menu_up_bar;

        BitmapText m_text_current_tile_set,
                   m_text_bar,
                   m_info_message;

        Vec2f  m_mouse_tile_position;
        
        bool m_is_moving               = false,
             m_is_mouse_on_menu_up_bar = false, m_is_mouse_on_menu_up_list = false, m_is_mouse_on_menu_down = false,
             m_is_showing_menu_down    = true,  m_is_showing_menu_right = true,
             m_is_showing_grid_map     = true,
             m_is_showing_tile_set     = false,
             m_is_typing_text_bar      = false;

        std::pair<bool, size_t> m_is_asked_to_remove_level = { false, 0 };

        u8   m_level_brush_size    = 1,
             m_tile_set_brush_size = 1,
             m_tile_set            = 0, m_prev_tile_set = 255,
             m_layer               = 0;

        u16  m_tile_number = 0;
            
        u16  m_menu_up_w        = 0,
             m_timer            = 0,
             m_time_left_saving = 0, m_time_to_save = 10;

        std::vector<undo::Info> m_undo_info_placed,
                                m_undo_info_replaced,
                                m_undo_info_erased,
                                m_undo_info_moved;
        
        std::vector<u16>        m_undo_counts;
        u16                     m_last_undo_count = 0;

        std::vector<undo::Act>  m_undo_acts;

        std::filesystem::path m_grid_texture_path     = "res/texture/editor_grid_block.png",
                              m_grid_map_texture_path = "res/texture/tile_blue.png",
                              m_mouse_texture_path    = "res/texture/tile_selection.png",
                              m_level_path            = "";

        std::map<entity::Info, std::string> m_types;
        
        bool is_all_valid();
        void handle_menu_up();
        bool add_to_menu_up_bar(const std::string& text);
        void load_menu_up_list(const std::string& menu, const std::filesystem::path& level_dir_path);
        bool add_to_menu_up_list(const std::string& menu, const std::filesystem::path& level_path);
        bool remove_level(const std::filesystem::path& path);
        
        void handle_menu_down();
        void handle_menu_right();

        bool init_brush(u8 size);

        i32c grid_sprite_id_at_offset(Vec2fc offset);
        i32c grid_map_sprite_id_at_offset(Vec2fc offset);
        bool add_grid_at_offset(Vec2fc offset);
        bool erase_grid_at_offset(Vec2fc offset);
        bool clear_grid_sprites();

        //void clear_mouse_sprites();
                
        void init_tile_set(u8c layer);
        void enter_tile_set();
        void exit_tile_set();
        void move_tile_set(Vec2fc amount);
        bool select_on_tile_set(Vec2fc offset);
        bool deselect_on_tile_set(Vec2fc offset);
        bool move_selection_on_tile_set(Vec2fc amount);
        bool deselect_all_on_tile_set();

        bool init_typing_text_bar();
        bool quit_typing_text_bar();
        void handle_typing_text_bar();
        bool shift_text_bar_typing_pos_left();
        bool shift_text_bar_typing_pos_right();
        bool insert_at_text_bar_typing_pos(input::Key key, bool is_shift);
        bool erase_at_text_bar_typing_pos();
        bool save_typed_text_bar();

        bool save_types_to_bin();
        bool load_types_from_bin();
        void load_types_from_text_file(const std::filesystem::path& path);

        void move_level(Vec2fc amount);
        void move_level_to(Vec2fc position);
        bool select_on_level(Vec2fc offset);
        bool deselect_on_level(Vec2fc offset);

        std::vector<i32> find_sprites_in_selection_on_level();

        void copy_selected_start_moving_on_level();
        void start_moving_selected_on_level();
        void clear_selected_on_level();
        void move_selected_on_level(Vec2fc amount);
        void finish_moving_selected_on_level();
        void deselect_all_on_level();
        bool eyedropper_on_level(Vec2fc offset);
        sprite::Object* level_sprite_at_offset(u8c layer, Vec2fc offset);

        undo::Act place_sprite_on_level(u8c layer, u8c tile_set, Rectic source_rect, Vec2fc offset);
        undo::Act erase_sprite_on_level(u8c layer, Vec2fc offset);
        
        void undo_last_act();
        void clear_level_sprites();
        void load_level_sprites(const std::filesystem::path& path);

        void update() override;
    };
}