module;
#include <chrono>
#include <fstream>
#include <filesystem>
#include <vector>
#include <sstream>

export module app;

import console;
import bitmap_text;
import input;
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
    Window      m_window;

    state::Type m_state_type;

    std::unique_ptr<state::State> m_state;

    std::filesystem::path m_level_path = "",
                          m_level_path_L_0 = m_level_path,
                          m_level_path_R_0 = m_level_path,
                          m_level_music_path,
                          m_prev_level_music_path;    

    i32 m_music_id = -1;

    u8 m_time_left_saving = 0, m_time_to_save = 50;

    std::filesystem::path m_level_path_to_save;

    void init(u16c w, u16c h, u8c scale, const char* title) {
        m_window.init(w, h, scale, title);
    }
public:
    App(u16c w, u16c h, u8c scale, const char* title) {
        init(w, h, scale, title);
    }
    ~App() {
        music::Set::erase(m_music_id);
        music::Set::clear();
    }

    void set_level_path_to_save(const std::filesystem::path& p) {
        m_level_path_to_save = p;
    }

    const auto read_save() {
        std::ifstream in_file("res/saves/0.bin", std::ios::in | std::ios::binary);
        char saved_path[32];
        in_file.read((i8*)&saved_path, sizeof(char) * 32);

        return std::string(saved_path);
    }
    void write_save(const std::filesystem::path& level_path_to_save) {
        if (level_path_to_save.empty()) {
            Console::log("level_path_to_save empty\n");
            return;
        }
        std::ofstream out_file("res/saves/0.bin", std::ios::out | std::ios::binary);
        //char save_path[32] = m_level_path_to_save;
        out_file.write((i8*)level_path_to_save.string().c_str(), sizeof(char) * 32);
        Console::log("write_save ", level_path_to_save.string(), "\n");
    }

    void set_state(state::Type next) {
        m_state_type = next;
        start::Info start_info;
        if (m_state) {
            switch (m_state->next_start()) {
            case start::Type::L_0:    m_level_path = m_level_path_L_0;      break;
            case start::Type::R_0:    m_level_path = m_level_path_R_0;      break; }

            start_info = m_state->start_info();

            //Console::log("next start: ", start::to_string(m_state->next_start()), "\n");
        }

        //Console::log("m level_path: ", m_level_path, "\n");

        if (m_level_path.empty()) {
            Console::log("App::set_state level path empty!\n");
            Console::log("read save: ", read_save(), "\n");
            m_level_path = read_save();
        }
        
        //Console::log("App::set_state ", state::to_string(next), " ", sprite::to_string(m_state->next_level_start()), "\n");

        /*if (m_state->next_level_start() == sprite::Type::level_L_0 ||
            m_state->next_level_start() == sprite::Type::level_R_0 ||
            m_state->next_level_start() == sprite::Type::level_start) {
            m_level_start = m_state->next_level_start();
        }*/
        
        m_state.reset();

        load_level_config(m_level_path);

        if (next == state::Type::edit) {
            music::Set::erase(m_music_id);
            m_prev_level_music_path.clear();
            m_state = std::make_unique<state::Edit>(m_window.w(), m_window.h(), m_level_path);
        } else if (next == state::Type::game) {
            m_state = std::make_unique<state::Game>(m_window.w(), m_window.h(), m_level_path, start_info);
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
                    set_state(m_state->next());
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

            m_window.clear();
            //Console::log("current fps: ", m_state->current_fps, "\n");

            //line::Set::update();
            //sprite::Set::update();
            for (auto& layer : m_state->get_visible_layers()) {
                for (auto& i : sprite::Set::get_objects(layer)) {
                    i->update();
                    if ((!i->is_hidden && !i->is_debug) || (i->is_debug && m_state->is_drawing_debug_lines())) {
                        m_window.draw(i);
                    }
                }
                for (auto& i : line::Set::get_objects(layer)) {
                    i->update();
                    if (!i->is_debug || m_state->is_drawing_debug_lines()) {
                        m_window.draw(i);
                    }
                }
            }
            m_window.display();
           
        }
    }





    void load_level_config(const std::filesystem::path& path) {
        std::string path_str = path.string();

        path_str.pop_back();
        path_str.pop_back();
        path_str.pop_back();

        path_str.append("cfg");

        Console::log("state::Game::load_level_config: ", path_str, "\n");

        std::ifstream in_file(path_str);
        if (!in_file) {
            Console::error("state::Game::load_level_config ", path_str, " not found\n");
            return;
        }
        std::ostringstream oss{};
        oss << in_file.rdbuf();

        const std::string text = oss.str();

        size_t levels_label = text.find("Levels", 0);

        if (levels_label != std::string::npos) {
            size_t levels_open = text.find("{", levels_label);
            if (levels_open != std::string::npos) {
                ++levels_open;
                size_t levels_close = text.find("\n}", levels_open);
                if (levels_close != std::string::npos) {
                    const size_t L_0_label = text.find("L_0", levels_open);
                    if (L_0_label < levels_close) {
                        const size_t end_line = text.find("\n", L_0_label);
                        size_t L_0_start = text.find("=", L_0_label);
                        if (L_0_start < end_line) {
                            ++L_0_start;
                            while (text.at(L_0_start) == '	' || text.at(L_0_start) == ' ') ++L_0_start;

                            m_level_path_L_0 = "res/levels/" + text.substr(L_0_start, end_line - L_0_start);

                            Console::log("state::Game::load_level_config L_0: ", m_level_path_L_0, "\n");
                        }
                    }
                    const size_t R_0_label = text.find("R_0", levels_open);
                    if (R_0_label < levels_close) {
                        const size_t end_line = text.find("\n", R_0_label);
                        size_t R_0_start = text.find("=", R_0_label);
                        if (R_0_start < end_line) {
                            ++R_0_start;
                            while (text.at(R_0_start) == '	' || text.at(R_0_start) == ' ') ++R_0_start;
                            m_level_path_R_0 = "res/levels/" + text.substr(R_0_start, end_line - R_0_start);

                            Console::log("state::Game::load_level_config R_0: ", m_level_path_R_0, "\n");
                        }
                    }
                    const size_t music_label = text.find("music", levels_open);
                    if (music_label < levels_close) {
                        const size_t end_line = text.find("\n", music_label);
                        size_t music_start = text.find("=", music_label);
                        if (music_start < end_line) {
                            ++music_start;
                            while (text.at(music_start) == '	' || text.at(music_start) == ' ') ++music_start;
                            m_level_music_path = "res/music/" + text.substr(music_start, end_line - music_start);
                            Console::log("state::Game::load_level_config music path: ", m_level_music_path, " prev: ", m_prev_level_music_path, "\n");

                            if (m_level_music_path != m_prev_level_music_path) {
                                music::Set::erase(m_music_id);
                                m_prev_level_music_path = m_level_music_path;

                                m_music_id = music::Set::make(m_level_music_path);

                                music::Set::at(m_music_id)->is_looped(true);
                                music::Set::at(m_music_id)->volume(5.0f);
                                music::Set::at(m_music_id)->play();
                            }
                        }
                    }
                }
            }
        }

    }
};