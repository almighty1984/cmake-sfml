module;
#include <optional>
#include <vector>
//#include <map>

export module inputs;

export import input;
import config;
import console;
import types;
import window;

export class Inputs {
    static inline std::vector<Input*> m_inputs;
    static inline std::vector<size_t> m_unused_ids;
        
public:
    static inline Vec2f               mouse     { 0.0f,0.0f },
                                      mouse_prev{ 0.0f,0.0f };

    static inline f32                 scroll = 0.0f;

    static bool   is_valid(size_t id) { return (id >= 0 && id < m_inputs.size()) ? true : false; }    
    static Input* at(size_t i) { return (i >= 0 && i < m_inputs.size()) ? m_inputs.at(i) : nullptr; }
    
    static void handle_events(Window& w) {

        //Console::log("Inputs::handle_events\n");
        while (const std::optional event = w.poll_event()) {
            scroll = 0.0f;
            if (event->is<sf::Event::Closed>()) {
                w.close();
            }
            else if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
                input::Key key_num = (input::Key)key->scancode;
                //Console::log("key pressed ", (i32)key_num, "\n");
                for (auto& i : m_inputs) {
                    if (i) {
                        i->press(key_num);
                    }                    
                }
            } else if (const auto* key = event->getIf<sf::Event::KeyReleased>()) {
                input::Key key_num = (input::Key)key->scancode;
                //Console::log("key released ", (i32)key_num, "\n");
                for (auto& i : m_inputs) {
                    if (i) {
                        i->release(key_num);
                    }
                }
            } else if (const auto* mouse_moved = event->getIf<sf::Event::MouseMoved>()) {
                mouse = { (f32)(mouse_moved->position.x / Config::scale()), (f32)(mouse_moved->position.y / Config::scale()) };
                
            } else if (const auto* mouse_pressed = event->getIf<sf::Event::MouseButtonPressed>()) {                
                input::Button button = (input::Button)mouse_pressed->button;
                //Console::log("mouse pressed ", (i8)b, "\n");                
                for (auto& i : m_inputs) {
                    if (i) {
                        i->press(button);
                    }
                }
                if (button == input::Button::left) {
                    mouse_prev = { (f32)(mouse_pressed->position.x / Config::scale()), (f32)(mouse_pressed->position.y / Config::scale())};
                } else if (button == input::Button::right) {
                    //m_mouse_x = mouse_pressed->position.x;
                     //m_mouse_y = mouse_pressed->position.y;
                }
            } else if (const auto* mouse_released = event->getIf<sf::Event::MouseButtonReleased>()) {
                input::Button b = (input::Button)mouse_released->button;
                //Console::log("mouse released ", button_num, "\n");
                for (auto& i : m_inputs) {
                    if (i) {
                        i->release(b);
                    }
                }
            } else if (const auto* mouse_scrolled = event->getIf<sf::Event::MouseWheelScrolled>()) {
                scroll = mouse_scrolled->delta;
                //Console::log("Inputs::handle_events mouse delta: ", mouse_scrolled->delta, "\n");
            } else if (event->is<sf::Event::JoystickButtonPressed>()) {
                //Console::log("InputHandler JoystickButtonPressed ", sfEvent.joystickButton.button, "\n");
                //InputHandler::joyPress(sfEvent.joystickButton.button);
                //state->joystickButtonDown(sfEvent.joystickButton.button);
            }
        }
    }
    static size_t make() {
        Input* input = new Input;
        if (!m_unused_ids.empty()) {
            input->id = m_unused_ids.back();
            m_unused_ids.pop_back();
        } else {
            input->id = m_inputs.size();
            m_inputs.push_back(nullptr);
        }
        if (input->id >= 0 && input->id < m_inputs.size() && m_inputs.at(input->id)) {            
            delete m_inputs.at(input->id);
        }
        m_inputs.at(input->id) = input;
        return input->id;
    }    
    static void erase(size_t i) {
        Console::log("Inputs::erase ", i, "\n");
        if (m_inputs.empty() || i < 0 || i > m_inputs.size() - 1 || !m_inputs.at(i)) {
            Console::log("Inputs::erase ", i, "can't do it\n");
            return;
        }
        delete m_inputs.at(i);
        m_inputs.at(i) = nullptr;
        m_unused_ids.push_back(i);
    }
    static void clear() {
        for (size_t i = 0; i < m_inputs.size(); ++i) {
            Console::log("Inputs::clear erasing ", i, "\n");
            erase(i);
        }
        m_inputs.clear();
        m_unused_ids.clear();
    }
};