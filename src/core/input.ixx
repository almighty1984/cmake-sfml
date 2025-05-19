module;
#include <map>
#include <string>
#include <sstream>
#include <optional>
#include <vector>

export module input;

import console;
import config;
import types;
import window;

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
    i32c alphabet_begin() { return (i32)Key::a;         }
    i32c alphabet_end()   { return (i32)Key::z + 1;     }
    i32c numbers_begin()  { return (i32)Key::num_1;     }
    i32c numbers_end()    { return (i32)Key::num_0 + 1; }
    i32c inputs_begin()   { return (i32)Key::a;         }
    i32c inputs_end()     { return (i32)Key::alt + 1;   }
        
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

    struct Object {
        void press(Key k)      { key_map[k] = true;  }
        void release(Key k)    { key_map[k] = false; }
        bool is_pressed(Key k) { return key_map[k];  }

        void press(Button b)      { button_map[b] = true; }  void release(Button b) { button_map[b] = false; }
        bool is_pressed(Button b) { return button_map[b]; }

        i32 id = -1;
        std::map<Key, bool>       key_map;
        std::map<Button, bool>    button_map;
    };

    class Set {
        static inline std::vector<Object*> m_objects;
        static inline std::vector<size_t>  m_unused_ids;

    public:
        static inline Vec2f                mouse{ 0.0f, 0.0f }, mouse_prev{ 0.0f, 0.0f };
        static inline f32                  scroll = 0.0f;

        static bool    is_valid(size_t i) { return (i < m_objects.size() && m_objects.at(i)) ? true : false; }
        static Object* at(size_t i)       { return (i < m_objects.size()) ? m_objects.at(i) : nullptr;       }

        static void handle_events(Window& window) {
            while (const std::optional sf_event = window.poll_event()) {
                scroll = 0.0f;
                if (sf_event->is<sf::Event::Closed>()) {
                    window.close();
                } else if (const auto* sf_key = sf_event->getIf<sf::Event::KeyPressed>()) {
                    input::Key key_num = (input::Key)sf_key->scancode;
                    //Console::log("key pressed ", (i32)key_num, "\n");
                    for (auto& i : m_objects) {
                        if (i) {
                            i->press(key_num);
                        }
                    }
                } else if (const auto* sf_key = sf_event->getIf<sf::Event::KeyReleased>()) {
                    input::Key key_num = (input::Key)sf_key->scancode;
                    //Console::log("key released ", (i32)key_num, "\n");
                    for (auto& i : m_objects) {
                        if (i) {
                            i->release(key_num);
                        }
                    }
                } else if (const auto* sf_mouse_moved = sf_event->getIf<sf::Event::MouseMoved>()) {
                    Vec2fc view_scale = { 320.0f / window.view().w, 180.0f / window.view().h };                    
                    mouse = { (f32)(sf_mouse_moved->position.x / view_scale.x / Config::scale()), (f32)(sf_mouse_moved->position.y / view_scale.y / Config::scale()) };

                } else if (const auto* sf_mouse_pressed = sf_event->getIf<sf::Event::MouseButtonPressed>()) {
                    Button button = (Button)sf_mouse_pressed->button;
                    //Console::log("mouse pressed ", (i8)b, "\n");                
                    for (auto& i : m_objects) {
                        if (i) {
                            i->press(button);
                        }
                    }
                    if (button == Button::left) {
                        mouse_prev = { (f32)(sf_mouse_pressed->position.x / Config::scale()), (f32)(sf_mouse_pressed->position.y / Config::scale()) };
                    } else if (button == Button::right) {
                        //m_mouse_x = mouse_pressed->position.x;
                         //m_mouse_y = mouse_pressed->position.y;
                    }
                } else if (const auto* sf_mouse_released = sf_event->getIf<sf::Event::MouseButtonReleased>()) {
                    Button b = (Button)sf_mouse_released->button;
                    //Console::log("mouse released ", button_num, "\n");
                    for (auto& i : m_objects) {
                        if (i) {
                            i->release(b);
                        }
                    }
                } else if (const auto* sf_mouse_scrolled = sf_event->getIf<sf::Event::MouseWheelScrolled>()) {
                    scroll = sf_mouse_scrolled->delta;
                    //Console::log("input::Set::handle_events mouse delta: ", mouse_scrolled->delta, "\n");
                } else if (sf_event->is<sf::Event::JoystickButtonPressed>()) {
                    //Console::log("ObjectHandler JoystickButtonPressed ", sfEvent.joystickButton.button, "\n");
                    //state->joystickButtonDown(sfEvent.joystickButton.button);
                }
            }
        }
        static i32c make() {
            Object* object = new Object;
            if (!m_unused_ids.empty()) {
                object->id = m_unused_ids.back();
                m_unused_ids.pop_back();
                if (object->id >= 0 && object->id < m_objects.size() && m_objects.at(object->id)) {
                    delete m_objects.at(object->id);
                }
            } else {
                object->id = m_objects.size();
                m_objects.push_back(nullptr);
            }            
            m_objects.at(object->id) = object;
            return object->id;
        }
        static bool erase(size_t i) {
            if (!(i < m_objects.size()) || !m_objects.at(i)) {
                Console::log("input::Set::erase ", i, " can't do it\n");
                return false;
            }
            delete m_objects.at(i);
            m_objects.at(i) = nullptr;
            m_unused_ids.push_back(i);
            return true;
        }
        static void clear() {
            for (size_t i = 0; i < m_objects.size(); ++i) {
                Console::log("input::Set::clear erasing ", i, "\n");
                erase(i);
            }
            m_objects.clear();
            m_unused_ids.clear();
        }
    };
}