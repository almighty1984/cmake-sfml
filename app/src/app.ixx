module;
#include <chrono>
#include <vector>

export module app;

import console;
import inputs;
import lines;
import sprites;
import state;
import state.edit;
import state.game_init;
import transforms;
import types;
import window;

export class App {
    void init(u16 w, u16 h, const char* title) {        
        m_window.init(w, h, title);
    }
    Window        m_window;
    state::State* m_state = nullptr;
    state::Type   m_state_type;

    u16  m_frame_count = 0;
    u16c m_frames_until_update = 16667; // 1000000 / 60
    std::chrono::steady_clock::time_point m_prev_time = std::chrono::steady_clock::now();
    std::chrono::steady_clock::duration   m_delta_time;

public:
    App(u16c w, u16c h, const char* title) {
        init(w, h, title);
    }
    void set_state(state::Type next_type) {
        if (next_type == m_state_type || next_type == state::Type::null) return;
        m_state_type = next_type;
        if (m_state) {
            delete m_state;
            m_state = nullptr;
        }
        switch (next_type) {
        case state::Type::Edit:    m_state = new state::Edit; break;
        case state::Type::GameInit:    m_state = new state::GameInit; break;
        }
    }
    void run() {
        if (!m_state) {
            m_window.close();
        }
        while (m_window.is_open()) {
            m_prev_time = std::chrono::steady_clock::now();

            Inputs::handle_events(m_window);

            if (m_frame_count >= m_frames_until_update) {
                m_frame_count = 0;
                m_window.clear();
                m_state->update();
                Transforms::update();

                for (auto& i : m_state->visible_layers) {
                    Sprites::draw(i, m_window);
                }                
                Lines::draw(0, m_window);
                m_window.display();
                if (m_state->is_to_change()) {
                    Console::log("m_state->is_to_change\n");
                    set_state(m_state->next);
                }
            }

            m_delta_time = std::chrono::steady_clock::now() - m_prev_time;
            m_frame_count += std::chrono::duration_cast<std::chrono::microseconds>(m_delta_time).count();            
        }
    }
};