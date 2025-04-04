module;
#include <chrono>
#include <vector>
#include <sstream>

export module app;

import console;
import bitmap_text;
import inputs;
import lines;
import sprites;
import state;
import state.edit;
import state.game;
import transforms;
import types;
import window;

export class App {    
    Window      m_window;
    state::Type m_state_type;
    std::unique_ptr<state::State> m_state;    

    i32  m_frame_count = 0;
    i32c m_frames_until_update = 16667; // 1000000 / 60
        
    std::chrono::steady_clock::duration   m_delta_time;

    i32 m_transform_id = -1;

    void init(u16 w, u16 h, const char* title) {
        m_window.init(w, h, title);
        m_transform_id = Transforms::make();
    }
public:
    App(u16c w, u16c h, const char* title) {
        init(w, h, title);
    }
    void set_state(state::Type next) {
        if (next == m_state_type || next == state::Type::null) return;
        m_state_type = next;
        
        m_state.reset();
        switch (next) {
        case state::Type::Edit: m_state = std::make_unique<state::Edit>(); break;
        case state::Type::Game: m_state = std::make_unique<state::Game>(); break;
        }
    }
    void run() {
        if (!m_state) {
            m_window.close();
        }
        //m_text_fps.layer = m_state->num_visible_layers - 1;
        //m_text_fps.transform_id = m_transform_id;

        while (m_window.is_open()) {
            auto start = std::chrono::steady_clock::now();

            Inputs::handle_events(m_window);

            if (m_frame_count >= m_frames_until_update) {
                m_frame_count = 0;
                m_window.clear();

                m_state->current_fps = 1000000 / std::chrono::duration_cast<std::chrono::microseconds>(m_delta_time).count();
                m_state->update();
                Transforms::update();

                //Console::log("current fps: ", m_state->current_fps, "\n");

                for (auto& i : m_state->get_visible_layers()) {
                    Sprites::draw(i, m_window);
                    Lines::draw(i, m_window);
                }                
                
                if (m_state->is_to_change()) {
                    //Console::log("m_state->is_to_change to ", state::to_string(m_state->get_next()), "\n");
                    set_state(m_state->get_next());
                }
                m_window.display();
                m_delta_time = std::chrono::steady_clock::now() - start;
            }            

            m_frame_count += std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count();
        }
    }
};