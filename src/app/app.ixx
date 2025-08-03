module;
#include <chrono>
#include <fstream>
#include <filesystem>
#include <map>
#include <vector>
#include <sstream>

export module app;
import anim;
import console;
import bitmap_text;
import input;
import level_config;
import line;
import music;
import sprite;
import state;
import state.edit;
import state.game;
import transform;
import types;
import window;

export class App {    
    Window                        m_window;
    state::Type                   m_state_type;
    std::unique_ptr<state::State> m_state;
    std::filesystem::path         m_level_path = "";
    u8 m_time_left_saving = 0,    m_time_to_save = 50;
    std::filesystem::path         m_level_path_to_save;

    void init(u16c w, u16c h, u8c scale, const char* title) {
        m_window.init(w, h, scale, title);
    }
public:
    App(u16c w, u16c h, u8c scale, const char* title) {
        init(w, h, scale, title);
    }
    //~App() {}

    void set_level_path_to_save(const std::filesystem::path& p) {
        m_level_path_to_save = p;
    }

    const auto read_save() {
        std::ifstream in_file("res/save/0.bin", std::ios::in | std::ios::binary);
        char saved_path[32];
        in_file.read((i8*)&saved_path, sizeof(char) * 32);

        return std::string(saved_path);
    }
    void write_save(const std::filesystem::path& level_path_to_save) {
        if (level_path_to_save.empty()) {
            Console::log("level_path_to_save empty\n");
            return;
        }
        std::ofstream out_file("res/save/0.bin", std::ios::out | std::ios::binary);
        //char save_path[32] = m_level_path_to_save;
        out_file.write((i8*)level_path_to_save.string().c_str(), sizeof(char) * 32);
        Console::log("write_save ", level_path_to_save.string(), "\n");
    }

    void set_state(state::Type next_state) {
        m_state_type = next_state;
        start::Info start_info;

        if (m_state) {
            m_level_path = LevelConfig::level_path(m_state->next_start());
            start_info = m_state->start_info();
            m_state.reset();
        }
        if (m_level_path.empty()) {
            m_level_path = read_save();
            Console::log("App::set_state() read_save(): ", m_level_path, "\n");
            if (m_level_path.empty()) {
                Console::log("App::set_state() level path empty!\n");
                m_level_path = "res/level/test.bin";
            }
        }
        
        //LevelConfig::clear_paths();
        LevelConfig::load(m_level_path);

        if (next_state == state::Type::edit) {
            LevelConfig::clear_music();
            m_state = std::make_unique<state::Edit>(m_window.w(), m_window.h(), m_level_path);            
            m_state->view({ 0.0f, 0.0f, 480.0f, 270.0f });
        } else if (next_state == state::Type::game) {

            //LevelConfig::stop_music();

            m_state = std::make_unique<state::Game>(m_window.w(), m_window.h(), m_level_path, start_info);
            m_state->view({ 0.0f, 0.0f, 320.0f, 180.0f });
        } else {
            m_window.close();
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
            m_state->update_unlocked();

            input::Set::handle_events(m_window);


            if (frame_count >= frames_until_update) {
                frame_count = 0;

                m_state->current_fps = 1000000 / std::chrono::duration_cast<std::chrono::microseconds>(delta_time).count();
                m_state->update();

                transform::Set::update();
                line::Set::update();
                anim::Set::update();
                sprite::Set::update();                

                /*if (m_state->current_fps > highest_fps) {
                    highest_fps = m_state->current_fps;
                    Console::log("highest fps: ", highest_fps, "\n");
                }*/
                
                if (m_state->is_to_change()) {
                    //Console::log("m_state->is_to_change to ", state::to_string(m_state->get_next()), "\n");                    
                    set_state(m_state->next_state());
                }
                if (m_state->view() != m_window.view()) {
                    m_window.view(m_state->view());
                }
                
                if (m_state->is_to_write_save()) {
                    if (m_time_left_saving == 0) {
                        m_time_left_saving = m_time_to_save;
                        write_save(m_state->level_path_to_save());
                    }
                }
                if (m_time_left_saving > 0) {
                    --m_time_left_saving;
                    m_state->is_to_write_save(false);
                }
            }

            //Console::log("current fps: ", m_state->current_fps, "\n");

            m_window.clear();
            for (auto& layer : m_state->get_visible_layers()) {
                for (auto& i : sprite::Set::objects_in_layer(layer)) {
                    if (!i) continue;
                    if ((!i->is_hidden && !i->is_debug) || (i->is_debug && m_state->is_drawing_debug_lines())) {
                        m_window.draw(i);
                    }
                }
                for (auto& i : line::Set::objects_in_layer(layer)) {
                    if (!i) continue;
                    if (!i->is_debug || m_state->is_drawing_debug_lines()) {
                        m_window.draw(i);
                    }
                }
            }
            m_window.display();
           
        }
    }
};