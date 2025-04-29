module;
#include <chrono>
#include <vector>
#include <sstream>

export module app;

import console;
import bitmap_text;
import input;
import line;
import sprite;
import state;
import state.edit;
import state.game;
import transform;
import types;
import window;

export class App {    
    Window      m_window;

    state::Type m_state_type;

    std::unique_ptr<state::State> m_state;


    i32 m_transform_id = -1;

    void init(u16c w, u16c h, u8c scale, const char* title) {
        m_window.init(w, h, scale, title);
        m_transform_id = transform::Set::make();
    }
public:
    App(u16c w, u16c h, u8c scale, const char* title) {
        init(w, h, scale, title);
    }
    void set_state(state::Type next) {
        if (next == m_state_type || next == state::Type::null) return;
        m_state_type = next;
        
        m_state.reset();
        switch (next) {
        case state::Type::Edit: m_state = std::make_unique<state::Edit>(m_window.w(), m_window.h()); break;
        case state::Type::Game: m_state = std::make_unique<state::Game>(m_window.w(), m_window.h()); break;
        }
    }
    void run() {
        if (!m_state) {
            m_window.close();
        }
        //u32 highest_fps = 0;
                
        //u64 seconds_timer = 0;
        i32  frame_count = 0;
        i32c frames_until_update = 16667; // 1000000 / 60

        auto last_time = std::chrono::steady_clock::now();
        std::chrono::steady_clock::duration   delta_time;

        while (m_window.is_open()) {
            auto current_time = std::chrono::steady_clock::now();
            delta_time = current_time - last_time;
            last_time = current_time;

            frame_count += std::chrono::duration_cast<std::chrono::microseconds>(delta_time).count();

            //seconds_timer += std::chrono::duration_cast<std::chrono::microseconds>(delta_time).count();

            //if (seconds_timer >= 1000000) {
                //Console::log("hello ", std::chrono::duration_cast<std::chrono::microseconds>(delta_time).count(), "\n");
                //seconds_timer = 0;
            //}

            input::Set::handle_events(m_window);

            if (frame_count >= frames_until_update) {
                frame_count = 0;                
                transform::Set::update();                
                
                m_state->current_fps = 1000000 / std::chrono::duration_cast<std::chrono::microseconds>(delta_time).count();                

                m_state->update();

                /*if (m_state->current_fps > highest_fps) {
                    highest_fps = m_state->current_fps;
                    Console::log("highest fps: ", highest_fps, "\n");
                }*/
                
                if (m_state->is_to_change()) {
                    //Console::log("m_state->is_to_change to ", state::to_string(m_state->get_next()), "\n");
                    set_state(m_state->get_next());
                }
            }

            m_window.clear();
            //Console::log("current fps: ", m_state->current_fps, "\n");
            

            //line::Set::update();
            //sprite::Set::update();
            for (auto& layer : m_state->get_visible_layers()) {
                for (auto& i : sprite::Set::get_objects(layer)) {
                    i->update();
                    if (!i->is_hidden) m_window.draw(i);
                }
                for (auto& i : line::Set::get_objects(layer)) {
                    i->update();
                    if (!i->is_debug || m_state->is_drawing_debug_lines) {
                        m_window.draw(i);
                    }
                }
            }
            m_window.display();
           
        }
    }
};