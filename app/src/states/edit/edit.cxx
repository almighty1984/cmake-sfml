module;
#include <sstream>

module state.edit;

import console;
import transforms;
import sprites;
import types;

namespace state {
    Edit::Edit() {
        Console::log("Edit()\n");
        m_input_id = -1;
        m_mouse_transform_id = m_level_transform_id = m_tile_set_transform_id = m_tile_set_bg_transform_id = m_grid_transform_id = m_grid_map_transform_id = m_menu_up_transform_id = m_menu_down_transform_id = m_selection_on_set_transform_id = m_selection_on_level_transform_id = -1;
        m_menu_up_bg_sprite_id = m_menu_down_bg_sprite_id = m_menu_right_bg_sprite_id = m_save_sprite_id = m_open_sprite_id = m_tile_set_sprite_id = m_tile_set_bg_sprite_id = m_active_layer_sprite_id = -1;
        
        for (u8 i = 0; i < num_visible_layers; ++i) {
            visible_layers.insert(i);
        }
        next = current = Type::Edit;

        m_input_id = Inputs::make();

        m_mouse_transform_id = Transforms::make();
        m_level_transform_id = Transforms::make();
        
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
        Sprites::at(m_grid_map_sprite_ids.back())->layer = GRID_LAYER;

        m_selection_on_set_transform_id = Transforms::make();
        Transforms::at(m_selection_on_set_transform_id)->position = { 0.0f, 512.0f };
        m_selection_on_set_sprite_ids.push_back(Sprites::make("res/tiles/editor_selection.png"));
        Sprites::at(m_selection_on_set_sprite_ids.back())->layer = SELECTION_ON_SET_LAYER;
        Sprites::at(m_selection_on_set_sprite_ids.back())->transform_id = m_selection_on_set_transform_id;

        m_tile_set_bg_transform_id = Transforms::make();
        Transforms::at(m_tile_set_bg_transform_id)->position = { 0.0f, 512.0f };
        m_tile_set_bg_sprite_id = Sprites::make("res/planes/textures/editor_tile_set_bg.png");
        Sprites::at(m_tile_set_bg_sprite_id)->source_rect = { 0, 0, 512, 512 };
        Sprites::at(m_tile_set_bg_sprite_id)->layer = SET_LAYER;
        Sprites::at(m_tile_set_bg_sprite_id)->transform_id = m_tile_set_bg_transform_id;

        m_tile_set_transform_id = Transforms::make();
        Transforms::at(m_tile_set_transform_id)->position = { 0.0f, 512.0f };
        m_tile_set_sprite_id = Sprites::make(tile_set_texture_path(0));
        Sprites::at(m_tile_set_sprite_id)->source_rect = { 0, 0, 512, 512 };
        Sprites::at(m_tile_set_sprite_id)->layer = SET_LAYER + 1;
        Sprites::at(m_tile_set_sprite_id)->transform_id = m_tile_set_transform_id;

        m_menu_up_transform_id = Transforms::make();
        m_menu_up_bg_sprite_id = Sprites::make("res/planes/textures/editor_menu_bg.png");
        Sprites::at(m_menu_up_bg_sprite_id)->source_rect = { 0, 0, 64, 16 };
        Sprites::at(m_menu_up_bg_sprite_id)->layer = MENU_LAYER;
        Sprites::at(m_menu_up_bg_sprite_id)->transform_id = m_menu_up_transform_id;

        m_menu_down_transform_id = Transforms::make();
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

        m_save_sprite_id = Sprites::make("res/tiles/editor_save.png");
        Sprites::at(m_save_sprite_id)->offset = { 32.0f, 192.0f };
        Sprites::at(m_save_sprite_id)->layer = 12;
        Sprites::at(m_save_sprite_id)->transform_id = m_menu_down_transform_id;

        m_open_sprite_id = Sprites::make("res/tiles/editor_open.png");
        Sprites::at(m_open_sprite_id)->offset = { 48.0f, 192.0f };
        Sprites::at(m_open_sprite_id)->layer = MENU_LAYER + 1;
        Sprites::at(m_open_sprite_id)->transform_id = m_menu_down_transform_id;

        m_active_layer_sprite_id = Sprites::make("res/tiles/editor_layer_active.png");
        Sprites::at(m_active_layer_sprite_id)->offset = { 320, 32 };
        Sprites::at(m_active_layer_sprite_id)->source_rect = { 0, 0, 16, 16 };
        Sprites::at(m_active_layer_sprite_id)->layer = MENU_LAYER + 1;
        Sprites::at(m_active_layer_sprite_id)->transform_id = m_menu_down_transform_id;

        for (u8 i = 0; i < 10; ++i) {
            m_is_hidden_layer_sprite_ids.push_back(Sprites::make("res/tiles/editor_layer_hidden.png"));
            Sprites::at(m_is_hidden_layer_sprite_ids.back())->offset = { 336.0f, i * 16.0f + 32.0f };
            Sprites::at(m_is_hidden_layer_sprite_ids.back())->source_rect = { 0, 0, 16, 16 };
            Sprites::at(m_is_hidden_layer_sprite_ids.back())->layer = MENU_LAYER + 1;
            Sprites::at(m_is_hidden_layer_sprite_ids.back())->transform_id = m_menu_down_transform_id;
        }

        add_to_level_list("res/levels/test.bin");
        add_to_level_list("res/levels/test2.bin");

        /*std::filesystem::path path("res/levels/");

        if (std::filesystem::exists(path)) {
            for (const auto& p : path) {
                Console::log(p, "\n");
            }

        }*/

        //add_to_level_list("list");

        /*std::vector<std::string> lis;

        for (const auto& entry : std::filesystem::directory_iterator(".")) {
            lis.push_back(entry.path().string());
        }*/

        //std::filesystem::path path = "./levels";

        //auto current_path = std::filesystem::current_path().string();

        //const auto it = std::filesystem::directory_iterator{ path };

        //for (const std::filesystem::directory_entry& i : std::filesystem::directory_iterator{ path }) {
        //    std::cout << " " << i.path() << '\n';
        //}
    }
    Edit::~Edit() {
        Console::log("~Edit()\n");
        //Inputs::erase(m_input_id);
        Inputs::clear();

        /*for (auto& i : m_mouse_sprite_ids) {
            Sprites::erase(i);
        }
        for (auto& i : m_selection_on_set_sprite_ids) {
            Sprites::erase(i);
        }
        for (auto& i : m_selection_on_level_sprite_ids) {
            Sprites::erase(i);
        }
        for (auto& i : m_is_hidden_layer_sprite_ids) {
            Sprites::erase(i);
        }
        for (auto& i : m_moving_sprite_ids) {
            Sprites::erase(i);
        }
        Sprites::erase(m_active_layer_sprite_id);
        Sprites::erase(m_grid_sprite_id);
        Sprites::erase(m_tile_set_bg_sprite_id);
        Sprites::erase(m_tile_set_sprite_id);
        Sprites::erase(m_menu_down_bg_sprite_id);
        Sprites::erase(m_menu_right_bg_sprite_id);
        Sprites::erase(m_save_sprite_id);
        Sprites::erase(m_open_sprite_id);*/
        Sprites::clear();


        /*Transforms::erase(m_level_transform_id);
        Transforms::erase(m_mouse_transform_id);
        Transforms::erase(m_selection_on_set_transform_id);
        Transforms::erase(m_grid_transform_id);
        Transforms::erase(m_tile_set_bg_transform_id);
        Transforms::erase(m_tile_set_transform_id);
        Transforms::erase(m_menu_down_transform_id);*/
        Transforms::clear();
    }
    void Edit::add_to_level_list(const std::string& level_path) {
        std::unique_ptr<BitmapText> bitmap_text = std::make_unique<BitmapText>();
        bitmap_text->transform_id = m_menu_up_transform_id;
        bitmap_text->layer = 12;
        bitmap_text->offset = { 0.0f, m_level_list.size() * (f32)bitmap_text->font_size };
        bitmap_text->set_text(level_path);

        u16c bg_w = level_path.size() * (int)bitmap_text->font_size;
        u16c bg_h = (int)(bitmap_text->font_size * (m_level_list.size() + 1));

        Console::log("add_to_level_list bg dimensions: ", bg_w, " ", bg_h, "\n");

        m_level_list.push_back(std::move(bitmap_text));

        Sprites::at(m_menu_up_bg_sprite_id)->source_rect = { 0, 0, bg_w, bg_h };
    }
    bool Edit::is_all_valid() {
        if (!Inputs::is_valid(m_input_id) ||
            !Transforms::is_valid(m_level_transform_id) ||
            !Transforms::is_valid(m_mouse_transform_id) ||
            !Transforms::is_valid(m_selection_on_set_transform_id) ||
            !Transforms::is_valid(m_grid_transform_id) ||
            !Transforms::is_valid(m_tile_set_bg_transform_id) || !Transforms::is_valid(m_tile_set_transform_id) ||
            !Transforms::is_valid(m_menu_down_transform_id)
            ) {
            return false;
        }
        for (auto& i : m_grid_sprite_ids)  { if (!Sprites::is_valid(i)) return false; }
        for (auto& i : m_mouse_sprite_ids) { if (!Sprites::is_valid(i)) return false; }
        return true;
    }
    void Edit::select_tool(Vec2f position) {
        Console::log("state::Edit::select_tool\n");
    }

    void Edit::handle_menu_click(Vec2f position) {
        //Console::log("state::Edit::handle_menu_click position: ", position.x, " ", position.y, "\n");
        if (position.x == 0.0f && position.y == 160.0f) {
            Console::log("state::Edit::handle_menu_click save\n");
            //Sprites::erase_null();                
            m_time_left_saving = 10;
            save_level_sprites_to(m_level_path);
        } else if (position.x == 16.0f && position.y == 160.0f) {
            Console::log("state::Edit::handle_menu_click open\n");
            m_time_left_opening = 10;
            m_is_showing_menu_up = !m_is_showing_menu_up;
        } else if (position.x == 288 && position.y < 160) {
            Sprites::at(m_active_layer_sprite_id)->offset.y = position.y + 32.0f;
            m_layer = (u8)(position.y / 16.0f);
        } else if (position.x == 304 && position.y < 160) {
            u8 layer = (u8)(position.y / 16.0f);
            if (layer < m_is_hidden_layer_sprite_ids.size() - 1 && m_is_hidden_layer_sprite_ids.at(layer)) {
                Sprites::at(m_is_hidden_layer_sprite_ids.at(layer))->is_hidden = !Sprites::at(m_is_hidden_layer_sprite_ids.at(layer))->is_hidden;
            }
            if (Sprites::at(m_is_hidden_layer_sprite_ids.at(layer))->is_hidden) {
                visible_layers.erase(layer);
            } else {
                visible_layers.insert(layer);
            }
        }
    }
    void Edit::init_brush(u8 size) {
        //if (m_level_brush_size == size) return;
        if (size != 1 && size != 3) {
            size = 1;
        }
        Vec2fc start = size == 1 ? Vec2f{ 0.0f,  0.0f } : Vec2f{ -16.0f,-16.0f };
        Vec2fc end = size == 1 ? Vec2f{ 16.0f, 16.0f } : Vec2f{ 32.0f, 32.0f };

        for (auto& i : m_mouse_sprite_ids) {
            Sprites::erase(i);
        }
        m_mouse_sprite_ids.clear();

        for (f32 x = start.x; x < end.x; x += 16.0f) {
            for (f32 y = start.y; y < end.y; y += 16.0f) {
                m_mouse_sprite_ids.push_back(Sprites::make(m_mouse_texture_path));
                Sprites::at(m_mouse_sprite_ids.back())->transform_id = m_mouse_transform_id;
                Sprites::at(m_mouse_sprite_ids.back())->layer = 13;
                Sprites::at(m_mouse_sprite_ids.back())->offset.x = x;
                Sprites::at(m_mouse_sprite_ids.back())->offset.y = y;
                //Sprites::at(m_mouse_sprite_ids.back())->id = m_mouse_sprite_ids.back();
            }
        }
    }
}