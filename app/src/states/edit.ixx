module;
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <vector>
#include <set>

export module state.edit;

export import state;
import config;
import console;
import inputs;
import sprites;
import transforms;
import types;

enum class Action {
    move,
    copy
};

export namespace state {    
    class Edit : public State {
        size_t m_input_id = -1;

        size_t m_tile_set_transform_id    = -1, m_tile_set_sprite_id     = -1,
               m_tile_set_bg_transform_id = -1, m_tile_set_bg_sprite_id  = -1,
               m_grid_transform_id        = -1, m_grid_sprite_id         = -1,               
               m_menu_transform_id        = -1, m_menu_down_bg_sprite_id = -1, m_menu_right_bg_sprite_id = -1,
               m_save_sprite_id           = -1,               
               m_level_transform_id       = -1;

        size_t m_active_layer_sprite_id = -1;
        std::vector<size_t> m_is_hidden_layer_sprite_ids;

        size_t m_mouse_transform_id = -1;
        std::vector<size_t> m_mouse_sprite_ids;

        size_t m_selected_on_set_transform_id = -1;
        std::vector<size_t> m_selection_on_set_sprite_ids;

        size_t m_selected_on_level_transform_id = -1;
        std::vector<size_t> m_selection_on_level_sprite_ids;

        std::vector<size_t> m_moving_sprite_ids;

        Vec2f  m_mouse_tile_position;

        std::filesystem::path m_grid_texture_path  = "res/planes/textures/editor_grid_block.png",
                              m_mouse_texture_path = "res/tiles/editor_selection.png";
                              //m_tile_set_texture_path = "res/tiles/tile_set_0.png";

        bool m_is_moving           = false,
             m_is_showing_menu    = false,
             m_is_showing_tile_set = false;

        u8 m_brush_size  = 1,
           m_tile_set    = 0,
           m_layer       = 0;

        u16 m_timer = 0;

        std::filesystem::path tile_set_texture_path(u8c number) {
            return "res/tiles/set_" + std::to_string((int)number) + ".png";
        }

    public:
        ~Edit() {
            Console::log("~Edit()\n");
            Inputs::clear();
            Sprites::clear();
            Transforms::clear();
        }
        Edit() {
            Console::log("Edit()\n");

            for (u8 i = 0; i < 14; ++i) {
                visible_layers.insert(i);
            }

            next = current = Type::Edit;
            m_input_id = Inputs::add(new Input);

            m_mouse_transform_id = Transforms::add(new Transform);
            m_mouse_sprite_ids.push_back(Sprites::add(new Sprite(m_mouse_texture_path)));
            Sprites::at(m_mouse_sprite_ids.back())->transform_id = m_mouse_transform_id;
            Sprites::at(m_mouse_sprite_ids.back())->layer = 13;

            m_selected_on_set_transform_id = Transforms::add(new Transform);
            Transforms::at(m_selected_on_set_transform_id)->position = { 0.0f, 512.0f };
            m_selection_on_set_sprite_ids.push_back(Sprites::add(new Sprite("res/tiles/editor_selection.png")));
            Sprites::at(m_selection_on_set_sprite_ids.back())->layer = 12;
            Sprites::at(m_selection_on_set_sprite_ids.back())->transform_id = m_selected_on_set_transform_id;

            m_grid_transform_id = Transforms::add(new Transform);
            m_grid_sprite_id = Sprites::add(new Sprite(m_grid_texture_path));
            Sprites::at(m_grid_sprite_id)->source_rect = { 0, 0, 320, 240 };
            Sprites::at(m_grid_sprite_id)->transform_id = m_grid_transform_id;
            Sprites::at(m_grid_sprite_id)->layer = 10;

            m_tile_set_bg_transform_id = Transforms::add(new Transform);
            Transforms::at(m_tile_set_bg_transform_id)->position = { 0.0f, 512.0f };
            m_tile_set_bg_sprite_id = Sprites::add(new Sprite("res/planes/textures/editor_tile_set_bg.png"));
            Sprites::at(m_tile_set_bg_sprite_id)->source_rect = { 0, 0, 512, 512 };
            Sprites::at(m_tile_set_bg_sprite_id)->layer = 10;
            Sprites::at(m_tile_set_bg_sprite_id)->transform_id = m_tile_set_bg_transform_id;

            m_tile_set_transform_id = Transforms::add(new Transform);
            Transforms::at(m_tile_set_transform_id)->position = { 0.0f, 512.0f };
            m_tile_set_sprite_id = Sprites::add(new Sprite(tile_set_texture_path(0)));
            Sprites::at(m_tile_set_sprite_id)->source_rect = { 0, 0, 512, 512 };
            Sprites::at(m_tile_set_sprite_id)->layer = 11;
            Sprites::at(m_tile_set_sprite_id)->transform_id = m_tile_set_transform_id;


            m_menu_transform_id = Transforms::add(new Transform);            
            m_menu_down_bg_sprite_id = Sprites::add(new Sprite("res/planes/textures/editor_menu_bg.png"));
            Sprites::at(m_menu_down_bg_sprite_id)->offset = { 32.0f, 192.0f };
            Sprites::at(m_menu_down_bg_sprite_id)->source_rect = { 0, 0, 512, 512 };
            Sprites::at(m_menu_down_bg_sprite_id)->layer = 12;
            Sprites::at(m_menu_down_bg_sprite_id)->transform_id = m_menu_transform_id;
            m_menu_right_bg_sprite_id = Sprites::add(new Sprite("res/planes/textures/editor_menu_bg.png"));
            Sprites::at(m_menu_right_bg_sprite_id)->offset = { 320, 32 };
            Sprites::at(m_menu_right_bg_sprite_id)->source_rect = { 0, 0, 512, 512 };
            Sprites::at(m_menu_right_bg_sprite_id)->layer = 12;
            Sprites::at(m_menu_right_bg_sprite_id)->transform_id = m_menu_transform_id;

            m_save_sprite_id = Sprites::add(new Sprite("res/tiles/editor_save.png"));
            Sprites::at(m_save_sprite_id)->offset = { 32.0f, 192.0f };
            Sprites::at(m_save_sprite_id)->layer = 13;
            Sprites::at(m_save_sprite_id)->transform_id = m_menu_transform_id;            

            m_active_layer_sprite_id = Sprites::add(new Sprite("res/tiles/editor_layer_active.png"));
            Sprites::at(m_active_layer_sprite_id)->offset = { 320, 32 };
            Sprites::at(m_active_layer_sprite_id)->source_rect = { 0, 0, 16, 16 };
            Sprites::at(m_active_layer_sprite_id)->layer = 13;
            Sprites::at(m_active_layer_sprite_id)->transform_id = m_menu_transform_id;

            for (u8 i = 0; i < 10; ++i) {
                m_is_hidden_layer_sprite_ids.push_back(Sprites::add(new Sprite("res/tiles/editor_layer_hidden.png")));
                Sprites::at(m_is_hidden_layer_sprite_ids.back())->offset = { 336.0f, i * 16.0f + 32.0f};
                Sprites::at(m_is_hidden_layer_sprite_ids.back())->source_rect = { 0, 0, 16, 16 };
                Sprites::at(m_is_hidden_layer_sprite_ids.back())->layer = 13;
                Sprites::at(m_is_hidden_layer_sprite_ids.back())->transform_id = m_menu_transform_id;
            }

            m_level_transform_id = Transforms::add(new Transform);
        }
        bool is_all_valid() {
            if (!Inputs::is_valid(m_input_id) ||
                !Transforms::is_valid(m_level_transform_id) || !Transforms::is_valid(m_grid_transform_id) ||
                !Transforms::is_valid(m_mouse_transform_id)
                ) {
                return false;
            }
            for (auto& i : m_mouse_sprite_ids) {
                if (!Sprites::is_valid(i)) {
                    return false;
                }
            }
            return true;
        }
        void select_on_level(Vec2f position) {
            //Console::log("state::Edit::select_on_level ", position.x, " ", position.y, "\n");
            for (auto& i : m_selection_on_level_sprite_ids) {
                if (Sprites::at(i)->offset == position) {
                    Console::log("state::Edit::select_on_level ", position.x, " ", position.y, " already selected\n");
                    return;
                }
            }
            m_selection_on_level_sprite_ids.push_back(Sprites::add(new Sprite(m_mouse_texture_path)));
            Sprites::at(m_selection_on_level_sprite_ids.back())->transform_id = m_level_transform_id;
            Sprites::at(m_selection_on_level_sprite_ids.back())->layer = 12;
            Sprites::at(m_selection_on_level_sprite_ids.back())->offset = position;
        }

        void init_move_selected_on_level(Action action) {
            Console::log("state::Edit::init_move_selected_on_level\n");
            std::set<size_t> found_ids;
            for (auto& i : m_selection_on_level_sprite_ids) {
                if (!Sprites::at(i)) continue;
                for (size_t sprite_id = 0; sprite_id < Sprites::size(); ++sprite_id) {
                    if (sprite_id == i || !Sprites::at(sprite_id) ||
                        std::find(m_selection_on_level_sprite_ids.begin(),
                            m_selection_on_level_sprite_ids.end(), sprite_id) != m_selection_on_level_sprite_ids.end()) {
                        continue;
                    }
                    if (Sprites::at(sprite_id)->offset == Sprites::at(i)->offset) {
                        found_ids.insert(sprite_id);
                    }
                }
            }
            for (auto& i : found_ids) {
                m_moving_sprite_ids.push_back(Sprites::add(new Sprite(tile_set_texture_path(Sprites::at(i)->tile_set))));
                Sprites::at(m_moving_sprite_ids.back())->transform_id = Sprites::at(i)->transform_id;
                Sprites::at(m_moving_sprite_ids.back())->offset = Sprites::at(i)->offset;
                Sprites::at(m_moving_sprite_ids.back())->source_rect = Sprites::at(i)->source_rect;
                Sprites::at(m_moving_sprite_ids.back())->tile_set = Sprites::at(i)->tile_set;
                Sprites::at(m_moving_sprite_ids.back())->layer = Sprites::at(i)->layer;

                if (action == Action::move) {
                    Sprites::erase(i);
                }
            }
        }
        void finish_move_selected_on_level() {
            Console::log("state::Edit::finish_move_selected_on_level\n");
            for (auto& i : m_moving_sprite_ids) {
                if (!Sprites::at(i)) continue;
                size_t sprite_id = Sprites::add(new Sprite(tile_set_texture_path(Sprites::at(i)->tile_set)));
                Sprites::at(sprite_id)->offset = Sprites::at(i)->offset;
                Sprites::at(sprite_id)->source_rect = Sprites::at(i)->source_rect;
                Sprites::at(sprite_id)->tile_set = Sprites::at(i)->tile_set;
                Sprites::at(sprite_id)->layer = m_layer;
                Sprites::at(sprite_id)->transform_id = Sprites::at(i)->transform_id;

                Sprites::erase(i);
            }
            m_moving_sprite_ids.clear();            
        }
        void clear_selection_on_level() {
            for (auto& i : m_selection_on_level_sprite_ids) {
                if (!Sprites::at(i)) continue;
                Sprites::erase(i);
            }
            m_selection_on_level_sprite_ids.clear();
        }
        void move_selected_on_level(Vec2f move) {
            for (auto& i : m_selection_on_level_sprite_ids) {
                if (!Sprites::at(i)) continue;
                Sprites::at(i)->offset += move;
            }
            for (auto& i : m_moving_sprite_ids) {
                if (!Sprites::at(i)) continue;
                Sprites::at(i)->offset += move;
            }
        }
        void select_tool(Vec2f position) {
            Console::log("state::Edit::select_tool\n");
        }
        void select_tile(Vec2f position) {
            Console::log("state::Edit::select_tile mouse_sprite_ids size: ", m_mouse_sprite_ids.size(), "\n");
            if (!Inputs::at(m_input_id)->is_pressed(input::Key::ctrl)) {
                for (auto& i : m_selection_on_set_sprite_ids) {
                    if (!Sprites::at(i)) continue;
                    Sprites::erase(i);
                }
                m_selection_on_set_sprite_ids.clear();
            }

            for (auto& i : m_mouse_sprite_ids) {
                bool is_already_selected = false;
                for (auto& selected : m_selection_on_set_sprite_ids) {
                    if (Sprites::at(selected)->offset == (m_mouse_tile_position + Sprites::at(i)->offset - Transforms::at(m_selected_on_set_transform_id)->position)) {
                        is_already_selected = true;
                    }
                }
                if (!is_already_selected) {
                    Console::log("state::Edit::select_tile mouse ", i, " offset: ", Sprites::at(i)->offset.x, " ", Sprites::at(i)->offset.y, "\n");

                    m_selection_on_set_sprite_ids.push_back(Sprites::add(new Sprite("res/tiles/editor_selection.png")));
                    Sprites::at(m_selection_on_set_sprite_ids.back())->layer = 12;
                    Sprites::at(m_selection_on_set_sprite_ids.back())->transform_id = m_selected_on_set_transform_id;
                    Sprites::at(m_selection_on_set_sprite_ids.back())->offset = m_mouse_tile_position + Sprites::at(i)->offset - Transforms::at(m_selected_on_set_transform_id)->position;
                }
            }
        }
        void place_on_level(Vec2f position) {
            for (auto& sel : m_selection_on_set_sprite_ids) {
                Vec2f pos = position + Sprites::at(sel)->offset - Sprites::at(m_selection_on_set_sprite_ids.front())->offset;
                bool is_already_placed = false;
                for (size_t i = 0; i < Sprites::size(); ++i) {
                    if (!Sprites::at(i)) continue;
                    if (Sprites::at(i)->transform_id == m_level_transform_id && Sprites::at(i)->layer == m_layer && Sprites::at(i)->offset == pos) {
                        Sprites::at(i)->source_rect = { (i16)Sprites::at(sel)->offset.x,
                                                        (i16)Sprites::at(sel)->offset.y + Sprites::at(m_tile_set_sprite_id)->source_rect.y,
                                                         16, 16 };
                        Sprites::at(i)->offset = pos;
                        is_already_placed = true;
                    }
                }
                if (!is_already_placed) {
                    size_t sprite_id = Sprites::add(new Sprite(tile_set_texture_path(0)));
                    Sprites::at(sprite_id)->id = sprite_id;
                    Sprites::at(sprite_id)->transform_id = m_level_transform_id;
                    Sprites::at(sprite_id)->source_rect = { (i16)Sprites::at(sel)->offset.x,
                                                            (i16)Sprites::at(sel)->offset.y + Sprites::at(m_tile_set_sprite_id)->source_rect.y,
                                                            16, 16 };
                    Sprites::at(sprite_id)->offset = pos;
                    Sprites::at(sprite_id)->layer = m_layer;
                }
            }
        }
        void erase_on_level(Vec2f position) {
            for (size_t i = 0; i < Sprites::size(); ++i) {
                if (!Sprites::at(i)) continue;
                if (Sprites::at(i)->transform_id == m_level_transform_id && Sprites::at(i)->offset == position) {
                    Console::log("state::Edit::erase_on_level ", i, " ", " id: ", Sprites::at(i)->id, " at ", position.x, " ", position.y, "\n");
                    //Sprites::erase(Sprites::at(i)->id);
                    Sprites::erase(i);
                    return;
                }
            }
        }

        Vec2f find_tile(Vec2f position) {
            for (size_t i = 0; i < Sprites::size(); ++i) {
                if (!Sprites::at(i)) continue;
                if (Sprites::at(i)->transform_id == m_level_transform_id && Sprites::at(i)->offset == position) {
                    Console::log("state::Edit::find_at ", Sprites::at(i)->source_rect.x, " ", Sprites::at(i)->source_rect.y, "\n");
                    return { (f32)Sprites::at(i)->source_rect.x, (f32)Sprites::at(i)->source_rect.y };
                }
            }
            return {};
        }       

        void update() override {
            if (!is_all_valid()) return;

            m_is_showing_tile_set = Transforms::at(m_tile_set_bg_transform_id)->position.y == 0.0f ? true : false;
                        
            if (Inputs::at(m_input_id)->is_pressed(input::Key::alt)) {
                Inputs::at(m_input_id)->release(input::Key::alt);
                m_is_showing_menu = !m_is_showing_menu;
            }
            Transforms::at(m_menu_transform_id)->position = { 0.0f, 0.0f };
            if (m_is_showing_menu) {
                Transforms::at(m_menu_transform_id)->position = { -32.0f, -32.0f };
            }            
            

            m_timer += 1;
            if (m_timer > 2) {
                m_timer = 0;
                return;
            }

            /*if (Inputs::scroll < 0.0f) {
                Inputs::scroll = 0.0f;
                if (Sprites::at(m_tile_set_sprite_id)->source_rect.y < 320.0f) {
                    Sprites::at(m_tile_set_sprite_id)->source_rect.y += 16.0f;
                    Sprites::at(m_tile_set_sprite_id)->source_rect.h -= 16.0f;
                }
                Console::log("state::Edit::update tile_set y: ", Sprites::at(m_tile_set_sprite_id)->source_rect.y, "\n");
            } else if (Inputs::scroll > 0.0f) {
                Inputs::scroll = 0.0f;
                if (Sprites::at(m_tile_set_sprite_id)->source_rect.y > 0.0f) {
                    Sprites::at(m_tile_set_sprite_id)->source_rect.y -= 16.0f;
                    Sprites::at(m_tile_set_sprite_id)->source_rect.h += 16.0f;
                }
                Console::log("state::Edit::update tile_set y: ", Sprites::at(m_tile_set_sprite_id)->source_rect.y, "\n");
            }*/

            if (Inputs::at(m_input_id)->is_pressed(input::Key::c)) {
                Inputs::at(m_input_id)->release(input::Key::c);
                if (!m_selection_on_level_sprite_ids.empty()) {
                    if (!m_is_moving) {
                        m_is_moving = true;
                        init_move_selected_on_level(Action::copy);
                    } else {
                        finish_move_selected_on_level();
                        init_move_selected_on_level(Action::copy);
                    }
                }
            }
            if (Inputs::at(m_input_id)->is_pressed(input::Key::x)) {
                Inputs::at(m_input_id)->release(input::Key::x);
                if (!m_selection_on_level_sprite_ids.empty()) {
                    if (!m_is_moving) {
                        m_is_moving = true;
                        init_move_selected_on_level(Action::move);
                    }
                }
            }

            Vec2f remainder = { std::fmodf(Inputs::mouse.x, 16.0f), std::fmodf(Inputs::mouse.y, 16.0f) };

            m_mouse_tile_position = Inputs::mouse - remainder;

            Transforms::at(m_mouse_transform_id)->position = m_mouse_tile_position;

            //Console::log("state::Edit::update tile_set position: ", Transforms::at(m_tile_set_transform_id)->position.x, " ", Transforms::at(m_tile_set_transform_id)->position.y, "\n");
            if (Inputs::at(m_input_id)->is_pressed(input::Key::a)) {                
                if (!m_selection_on_level_sprite_ids.empty()) {
                    Inputs::at(m_input_id)->release(input::Key::a);
                    move_selected_on_level({ -16.0f, 0.0f });
                    return;
                }
                if (Inputs::at(m_input_id)->is_pressed(input::Key::ctrl)) {
                    Inputs::at(m_input_id)->release(input::Key::a);
                    bool is_out_of_bounds = false;
                    for (auto& i : m_selection_on_set_sprite_ids) {
                        if (Sprites::at(i)->offset.x < 16) {
                            is_out_of_bounds = true;
                            break;
                        }
                    }
                    if (!is_out_of_bounds) {
                        for (auto& i : m_selection_on_set_sprite_ids) {
                            Sprites::at(i)->offset.x -= 16;
                        }
                    }
                } else {
                    if (m_is_showing_tile_set) {
                        if (Transforms::at(m_tile_set_transform_id)->position.x < 0.0f) {
                            Transforms::at(m_tile_set_transform_id)->position.x += 16;
                            Transforms::at(m_selected_on_set_transform_id)->position.x += 16;
                        }
                    } else {
                        Transforms::at(m_level_transform_id)->position.x += 16;
                        Transforms::at(m_grid_transform_id)->position.x += 16;
                    }
                }
            }
            if (Inputs::at(m_input_id)->is_pressed(input::Key::d)) {
                if (!m_selection_on_level_sprite_ids.empty()) {
                    Inputs::at(m_input_id)->release(input::Key::d);
                    move_selected_on_level({ 16.0f, 0.0f });
                    return;
                }
                if (Inputs::at(m_input_id)->is_pressed(input::Key::ctrl)) {
                    Inputs::at(m_input_id)->release(input::Key::d);
                    bool is_out_of_bounds = false;
                    for (auto& i : m_selection_on_set_sprite_ids) {
                        if (Sprites::at(i)->offset.x > 480) {
                            is_out_of_bounds = true;
                            break;
                        }
                    }
                    if (!is_out_of_bounds) {
                        for (auto& i : m_selection_on_set_sprite_ids) {
                            Sprites::at(i)->offset.x += 16;
                        }
                    }
                } else {
                    if (m_is_showing_tile_set) {
                        if (Transforms::at(m_tile_set_transform_id)->position.x > -192) {
                            Transforms::at(m_tile_set_transform_id)->position.x -= 16;
                            Transforms::at(m_selected_on_set_transform_id)->position.x -= 16;
                        }
                    } else {
                        Transforms::at(m_level_transform_id)->position.x -= 16;
                        Transforms::at(m_grid_transform_id)->position.x -= 16;
                    }
                }
            }
            if (Inputs::at(m_input_id)->is_pressed(input::Key::w)) {
                if (!m_selection_on_level_sprite_ids.empty()) {
                    Inputs::at(m_input_id)->release(input::Key::w);
                    move_selected_on_level({ 0.0f, -16.0f });
                    return;
                }
                if (Inputs::at(m_input_id)->is_pressed(input::Key::ctrl)) {
                    Inputs::at(m_input_id)->release(input::Key::w);
                    bool is_out_of_bounds = false;
                    for (auto& i : m_selection_on_set_sprite_ids) {
                        if (Sprites::at(i)->offset.y < 0) {
                            is_out_of_bounds = true;
                            break;
                        }
                    }
                    if (!is_out_of_bounds) {
                        for (auto& i : m_selection_on_set_sprite_ids) {
                            Sprites::at(i)->offset.y -= 16;
                        }
                    }
                } else {
                    if (m_is_showing_tile_set) {
                        if (Transforms::at(m_tile_set_transform_id)->position.y < 0) {
                            Transforms::at(m_tile_set_transform_id)->position.y += 16;
                            Transforms::at(m_selected_on_set_transform_id)->position.y += 16;
                        }
                    } else {
                        Transforms::at(m_level_transform_id)->position.y += 16;
                        Transforms::at(m_grid_transform_id)->position.y += 16;
                    }
                }
            }
            if (Inputs::at(m_input_id)->is_pressed(input::Key::s)) {
                if (!m_selection_on_level_sprite_ids.empty()) {
                    Inputs::at(m_input_id)->release(input::Key::s);
                    move_selected_on_level({ 0.0f, 16.0f });
                    return;
                }
                if (Inputs::at(m_input_id)->is_pressed(input::Key::ctrl)) {
                    Inputs::at(m_input_id)->release(input::Key::s);
                    bool is_out_of_bounds = false;
                    for (auto& i : m_selection_on_set_sprite_ids) {
                        if (Sprites::at(i)->offset.y > 480) {
                            is_out_of_bounds = true;
                            break;
                        }
                    }
                    if (!is_out_of_bounds) {
                        for (auto& i : m_selection_on_set_sprite_ids) {
                            Sprites::at(i)->offset.y += 16;
                        }
                    }
                } else {
                    if (m_is_showing_tile_set) {
                        if (Transforms::at(m_tile_set_transform_id)->position.y > -336) {
                            Transforms::at(m_tile_set_transform_id)->position.y -= 16;
                            Transforms::at(m_selected_on_set_transform_id)->position.y -= 16;
                        }
                    } else {
                        Transforms::at(m_level_transform_id)->position.y -= 16;
                        Transforms::at(m_grid_transform_id)->position.y -= 16;
                    }
                }
            }
            if (Inputs::at(m_input_id)->is_pressed(input::Key::num_1) && m_brush_size != 1) {
                Inputs::at(m_input_id)->release(input::Key::num_1);
                m_brush_size = 1;                
                for (auto& i : m_mouse_sprite_ids) {
                    Sprites::erase(i);
                }
                m_mouse_sprite_ids.clear();
                
                m_mouse_sprite_ids.push_back(Sprites::add(new Sprite(m_mouse_texture_path)));
                Sprites::at(m_mouse_sprite_ids.back())->transform_id = m_mouse_transform_id;
                Sprites::at(m_mouse_sprite_ids.back())->layer = 3;
            }
            if (Inputs::at(m_input_id)->is_pressed(input::Key::num_2) && m_brush_size != 3) {
                Inputs::at(m_input_id)->release(input::Key::num_2);
                m_brush_size = 3;
                for (auto& i : m_mouse_sprite_ids) {
                    Sprites::erase(i);
                }
                m_mouse_sprite_ids.clear();
                
                for (f32 x = -16.0f; x < 32.0f; x += 16.0f) {
                    for (f32 y = -16.0f; y < 32.0f; y += 16.0f) {
                        m_mouse_sprite_ids.push_back(Sprites::add(new Sprite(m_mouse_texture_path)));
                        Sprites::at(m_mouse_sprite_ids.back())->transform_id = m_mouse_transform_id;
                        Sprites::at(m_mouse_sprite_ids.back())->layer = 3;
                        Sprites::at(m_mouse_sprite_ids.back())->offset.x = x;
                        Sprites::at(m_mouse_sprite_ids.back())->offset.y = y;
                    }
                }
            }

            if (Inputs::at(m_input_id)->is_pressed(input::Key::tab)) {
                Inputs::at(m_input_id)->release(input::Key::tab);                
                if (Inputs::at(m_input_id)->is_pressed(input::Key::shift) && m_layer > 0) {
                    Sprites::at(m_active_layer_sprite_id)->offset.y -= 16.0f;
                    m_layer -= 1;
                } else if (m_layer < 9) {
                    Sprites::at(m_active_layer_sprite_id)->offset.y += 16.0f;
                    m_layer += 1;                    
                }
            }

            if (Inputs::at(m_input_id)->is_pressed(input::Button::left)) {
                Vec2f position = Transforms::at(m_mouse_transform_id)->position - Transforms::at(m_level_transform_id)->position;
                if (m_is_showing_tile_set && (!m_is_showing_menu || (m_mouse_tile_position.x < 288 && m_mouse_tile_position.y < 160))) {
                    select_tile(position);
                } else if (m_is_showing_menu && (m_mouse_tile_position.x >= 288 || m_mouse_tile_position.y >= 160)) {
                    Inputs::at(m_input_id)->release(input::Button::left);
                    Console::log("state::Edit::update mouse pos: ", m_mouse_tile_position.x, " ", m_mouse_tile_position.y, "\n");                    
                    if (m_mouse_tile_position.x == 288 && m_mouse_tile_position.y < 160) {
                        Sprites::at(m_active_layer_sprite_id)->offset.y = m_mouse_tile_position.y + 32.0f;
                        m_layer = (u8)(m_mouse_tile_position.y / 16.0f);
                    } else if (m_mouse_tile_position.x == 304 && m_mouse_tile_position.y < 160) {
                        u8 layer = (u8)(m_mouse_tile_position.y / 16.0f);
                        if (layer < m_is_hidden_layer_sprite_ids.size() - 1 && m_is_hidden_layer_sprite_ids.at(layer)) {
                            Sprites::at(m_is_hidden_layer_sprite_ids.at(layer))->is_hidden = !Sprites::at(m_is_hidden_layer_sprite_ids.at(layer))->is_hidden;
                        }
                        if (Sprites::at(m_is_hidden_layer_sprite_ids.at(layer))->is_hidden) {
                            visible_layers.erase(layer);
                        } else {
                            visible_layers.insert(layer);
                        }
                    }
                    //select_tool(position);
                } else {
                    if (Inputs::at(m_input_id)->is_pressed(input::Key::ctrl)) {
                        select_on_level(position);
                    } else {
                        for (auto& i : m_selection_on_level_sprite_ids) {
                            Sprites::erase(i);
                        }
                        m_selection_on_level_sprite_ids.clear();

                        for (auto& i : m_mouse_sprite_ids) {
                            place_on_level(position + Sprites::at(i)->offset);
                        }
                    }
                }
            }
            if (Inputs::at(m_input_id)->is_pressed(input::Button::right)) {
                for (auto& i : m_mouse_sprite_ids) {
                    Vec2fc position = Transforms::at(m_mouse_transform_id)->position + Sprites::at(i)->offset - Transforms::at(m_level_transform_id)->position;
                    erase_on_level(position);
                }
            }
            if (Inputs::at(m_input_id)->is_pressed(input::Button::middle)) {
                Inputs::at(m_input_id)->release(input::Button::middle);
                Vec2fc position = Transforms::at(m_mouse_transform_id)->position - Transforms::at(m_level_transform_id)->position;
                Vec2fc tile_offset = find_tile(position);
                
                for (auto& i : m_selection_on_set_sprite_ids) {
                    Sprites::erase(i);
                }
                m_selection_on_set_sprite_ids.clear();
                m_selection_on_set_sprite_ids.push_back(Sprites::add(new Sprite("res/tiles/editor_selection.png")));
                Sprites::at(m_selection_on_set_sprite_ids.back())->layer = 12;
                Sprites::at(m_selection_on_set_sprite_ids.back())->transform_id = m_selected_on_set_transform_id;
                Sprites::at(m_selection_on_set_sprite_ids.back())->offset = tile_offset;
            }

            if (Inputs::at(m_input_id)->is_pressed(input::Key::space)) {
                Inputs::at(m_input_id)->release(input::Key::space);
                if (Transforms::at(m_tile_set_bg_transform_id)->position.y == 0.0f) {
                    Transforms::at(m_tile_set_bg_transform_id)->position.y = 512.0f;
                    Transforms::at(m_tile_set_transform_id)->position.y += 512.0f;
                } else {
                    Transforms::at(m_tile_set_bg_transform_id)->position.y = 0.0f;
                    Transforms::at(m_tile_set_transform_id)->position.y -= 512.0f;
                }

                Transforms::at(m_selected_on_set_transform_id)->position = Transforms::at(m_tile_set_transform_id)->position;
                //Transforms::at(m_selected_on_set_transform_id)->position.y - Transforms::at(m_tile_set_transform_id)->position.y;
            }

            if (Inputs::at(m_input_id)->is_pressed(input::Key::esc)) {
                Inputs::at(m_input_id)->release(input::Key::esc);
                if (m_is_moving) {
                    m_is_moving = false;
                    finish_move_selected_on_level();
                    if (!m_selection_on_level_sprite_ids.empty()) {
                        clear_selection_on_level();
                        return;
                    }
                }
                
                set_next(Type::GameInit);
            }

        }
    };
}