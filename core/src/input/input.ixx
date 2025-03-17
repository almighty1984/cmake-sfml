module;
#include <map>

export module input;

import types;

export namespace input {
    enum class Key : i32 {
        null = -1,
        a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z,
        num_1 = 26, num_2, num_3, num_4, num_5, num_6, num_7, num_8, num_9, num_0,
        enter = 36, esc, backspace, tab, space, subtract,
        grave = 48,
        f1 = 52, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12,
        del = 83,
        right = 86, left, down, up,
        ctrl = 127, shift, alt        
    };
    enum class Button : i32 {
        null = -1,
        left, right, middle
    };    
}
export struct Input {
    Input() : id(-1),
              key_map() {
    }
    ~Input() {
        id = -1; 
    }
    void press(input::Key k)         { key_map[k] = true;         }  void release(input::Key k)    { key_map[k] = false; }
    bool is_pressed(input::Key k)    { return key_map[k];         }
    
    void press(input::Button b)      { button_map[b] = true; }       void release(input::Button b) { button_map[b] = false; }
    bool is_pressed(input::Button b) { return button_map[b]; }

    size_t id;
    std::map<input::Key, bool>      key_map;
    std::map<input::Button, bool>   button_map;    
};