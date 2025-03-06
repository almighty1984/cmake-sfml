module;
#include <vector>

export module player;

import console;
import input;
import inputs;
import sprites;
import transform;
import transforms;
import types;

export struct Player {
    size_t input_id     = -1,
           transform_id = -1,
           sprite_id = -1;
    input::Key key_up    = input::Key::up,
               key_down  = input::Key::down,
               key_left  = input::Key::left,
               key_right = input::Key::right;

    Player() {
        Console::log("Player()\n");
        input_id = Inputs::add(new Input);

        transform_id = Transforms::add(new Transform);
        Transforms::at(transform_id)->acceleration.x = 0.2f;
        Transforms::at(transform_id)->deceleration.x = 0.1f;
        Transforms::at(transform_id)->velocity_limit = { 5.0f, 0.0f };

        sprite_id = Sprites::add(new Sprite("res/tiles/editor_selection.png"));
        Sprites::at(sprite_id)->transform_id = transform_id;
    }
    ~Player() {
        Console::log("~Player()\n");
        Inputs::erase(input_id);
        Transforms::erase(transform_id);
        Sprites::erase(sprite_id);
    }
    
    Transform* transform() { return Transforms::is_valid(transform_id) ? Transforms::at(transform_id) : nullptr; }
    bool is_pressed(input::Key k) { return Inputs::is_valid(input_id) ? Inputs::at(input_id)->is_pressed(k) : false; }
    void release(input::Key k)    { if (Inputs::is_valid(input_id))     Inputs::at(input_id)->release(k); }

    void update() {
        if (!Inputs::is_valid(input_id)
            ||
            !Transforms::is_valid(transform_id)
            )
            return;

        //if      (transform()->velocity.x < 0.0f)    transform()->velocity.x += 0.01f;
        //else if (transform()->velocity.x > 0.0f)    transform()->velocity.x -= 0.01f;        

        if (is_pressed(input::Key::a)) {
            release(input::Key::a);
            //Console::log("input ", input_id, " pressed A\n");
        }
        if (is_pressed(key_up)) {
            release(key_up);
            //Console::log("input ", input_id, " pressed up\n");
        }
        if (is_pressed(key_down)) {
            //release(key_down);
            //Console::log("input ", input_id, " pressed down\n");
        }
        if (is_pressed(key_left)) {
            //release(key_left);
            //Console::log("input ", input_id, " pressed left\n");
            transform()->velocity.x -= transform()->acceleration.x;
        }
        if (is_pressed(key_right)) {
            //release(key_right);
            //Console::log("input ", input_id, " pressed right\n");
            transform()->velocity.x += transform()->acceleration.x;
        }
        //transform()->update();

        //Transforms::at(transform_id)->update();
        
        //Sprites::at(sprite_id)->position = Transforms::at(transform_id)->position;
        //Sprites::at(sprite_id)->update();        
    }
};