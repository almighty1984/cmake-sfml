module;
#include <map>
#include <string>

export module input;

import types;
import console;

export namespace input {
    enum class Key : i32 {
        null = -1, 
        a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z,
        num_1, num_2, num_3, num_4, num_5, num_6, num_7, num_8, num_9, num_0,
        enter = 36, esc, backspace, tab, space, subtract,
        grave = 48, comma, period, slash,
        f1 = 52, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12,
        del = 83,
        right = 86, left, down, up,
        ctrl = 127, shift, alt        
    };
    enum class Button : i32 {
        null = -1,
        left, right, middle
    };
    const size_t alphabet_begin() { return (size_t)Key::a;         }
    const size_t alphabet_end()   { return (size_t)Key::z + 1;     }
    const size_t numbers_begin()  { return (size_t)Key::num_1;     }
    const size_t numbers_end()    { return (size_t)Key::num_0 + 1; }
    const size_t inputs_begin()   { return (size_t)Key::a;         }
    const size_t inputs_end()     { return (size_t)Key::alt; +1;   }
        
    const std::string string_from(Key key, bool is_shift) {
        switch (key) {
        case Key::a:        return is_shift ? "A" : "a"; break;
        case Key::b:        return is_shift ? "B" : "b"; break;
        case Key::c:        return is_shift ? "C" : "c"; break;
        case Key::d:        return is_shift ? "D" : "d"; break;
        case Key::e:        return is_shift ? "E" : "e"; break;
        case Key::f:        return is_shift ? "F" : "f"; break;
        case Key::g:        return is_shift ? "G" : "g"; break;
        case Key::h:        return is_shift ? "H" : "h"; break;
        case Key::i:        return is_shift ? "I" : "i"; break;
        case Key::j:        return is_shift ? "J" : "j"; break;
        case Key::k:        return is_shift ? "K" : "k"; break;
        case Key::l:        return is_shift ? "L" : "l"; break;
        case Key::m:        return is_shift ? "M" : "m"; break;
        case Key::n:        return is_shift ? "N" : "n"; break;
        case Key::o:        return is_shift ? "O" : "o"; break;
        case Key::p:        return is_shift ? "P" : "p"; break;
        case Key::q:        return is_shift ? "Q" : "q"; break;
        case Key::r:        return is_shift ? "R" : "r"; break;
        case Key::s:        return is_shift ? "S" : "s"; break;
        case Key::t:        return is_shift ? "T" : "t"; break;
        case Key::u:        return is_shift ? "U" : "u"; break;
        case Key::v:        return is_shift ? "V" : "v"; break;
        case Key::w:        return is_shift ? "W" : "w"; break;
        case Key::x:        return is_shift ? "X" : "x"; break;
        case Key::y:        return is_shift ? "Y" : "y"; break;
        case Key::z:        return is_shift ? "Z" : "z"; break;
        case Key::num_0:    return is_shift ? ")" : "0"; break;
        case Key::num_1:    return is_shift ? "!" : "1"; break;
        case Key::num_2:    return is_shift ? "@" : "2"; break;
        case Key::num_3:    return is_shift ? "#" : "3"; break;
        case Key::num_4:    return is_shift ? "$" : "4"; break;
        case Key::num_5:    return is_shift ? "%" : "5"; break;
        case Key::num_6:    return is_shift ? "^" : "6"; break;
        case Key::num_7:    return is_shift ? "&" : "7"; break;
        case Key::num_8:    return is_shift ? "*" : "8"; break;
        case Key::num_9:    return is_shift ? "(" : "9"; break;
        case Key::subtract: return is_shift ? "_" : "-"; break;
        case Key::comma:    return is_shift ? "<" : ","; break;
        case Key::period:   return is_shift ? ">" : "."; break;
        case Key::slash:    return is_shift ? "?" : "/"; break;
        default:            return "";  break;
        }
    }
    const std::string string_from(Key key) {
        return string_from(key, false);
    }
}
export struct Input {
    Input() : id(-1),
              key_map() {
    }
    ~Input() {
        id = -1; 
    }
    void press(input::Key k)      { key_map[k] = true;  }
    void release(input::Key k)    { key_map[k] = false; }
    bool is_pressed(input::Key k)    { return key_map[k];         }
    
    void press(input::Button b)      { button_map[b] = true; }       void release(input::Button b) { button_map[b] = false; }
    bool is_pressed(input::Button b) { return button_map[b]; }

    size_t id;
    std::map<input::Key, bool>      key_map;
    std::map<input::Button, bool>   button_map;    
};