module;
#include <filesystem>
#include <vector>
#include <sstream>

export module player;

import console;
import input;
import inputs;
import sprites;
import transform;
import transforms;
import types;

export class Player {
private:
    i32  m_input_id     = -1,
         m_transform_id = -1,
         m_sprite_id    = -1;
public:
    input::Key key_up    = input::Key::w,
               key_down  = input::Key::s,
               key_left  = input::Key::a,
               key_right = input::Key::d;

    Player();
    ~Player();

    void set_layer(u8c layer) { Sprites::at(m_sprite_id)->layer = layer; }

    size_t get_transform_id() const { return m_transform_id; }
    size_t get_sprite_id() const { return m_sprite_id; }

    Rectic source_rect() const { return Sprites::at(m_sprite_id) ? Sprites::at(m_sprite_id)->source_rect : Rect{0, 0, 0, 0}; }

    bool sprite_texture(const std::filesystem::path& path) { if (Sprites::at(m_sprite_id)) { Sprites::at(m_sprite_id)->texture(path); return true; } else return false; }

    Vec2fc position() const { return Transforms::at(m_transform_id) ? Transforms::at(m_transform_id)->position : Vec2f{ 0.0f,0.0f }; }  void position(Vec2fc p) { if (Transforms::at(m_transform_id)) Transforms::at(m_transform_id)->position = p; }
    f32c   x()        const { return Transforms::at(m_transform_id) ? Transforms::at(m_transform_id)->position.x : 0.0f; }              void x(f32c in_x) { if (Transforms::at(m_transform_id)) Transforms::at(m_transform_id)->position.x = in_x; }
    f32c   y()        const { return Transforms::at(m_transform_id) ? Transforms::at(m_transform_id)->position.y : 0.0f; }              void y(f32c in_y) { if (Transforms::at(m_transform_id)) Transforms::at(m_transform_id)->position.y = in_y; }
    Vec2fc velocity() const { return Transforms::at(m_transform_id) ? Transforms::at(m_transform_id)->velocity : Vec2f{ 0.0f, 0.0f }; } void velocity(Vec2fc v) { if (Transforms::at(m_transform_id)) Transforms::at(m_transform_id)->velocity = v; }

    void acceleration(Vec2fc a) { if (Transforms::at(m_transform_id)) Transforms::at(m_transform_id)->acceleration = a; }
    void deceleration(Vec2fc d) { if (Transforms::at(m_transform_id)) Transforms::at(m_transform_id)->deceleration = d; }

    void velocity_limit(Vec2fc l) { if (Transforms::at(m_transform_id)) Transforms::at(m_transform_id)->velocity_limit = l; }
    
    Transform* transform()        { return Transforms::is_valid(m_transform_id) ? Transforms::at(m_transform_id) : nullptr; }
    bool is_pressed(input::Key k) { return Inputs::is_valid(m_input_id) ? Inputs::at(m_input_id)->is_pressed(k) : false; }
    void release(input::Key k)    { if (Inputs::is_valid(m_input_id))     Inputs::at(m_input_id)->release(k); }

    void update();
};