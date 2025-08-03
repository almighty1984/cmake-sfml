module;
#include <fstream>
#include <sstream>
module player;
import anim;
import collider.aabb;
import console;
import input;
import sound;
import sprite;
import transform;
import types;

namespace player {
    Object::Object() {
        Console::log("Player()\n");
        m_input_id = input::Set::make();
        
        m_type = entity::Type::player;
        m_state = entity::State::run;

        m_transform_id = transform::Set::make();
        transform::Set::at(m_transform_id)->acceleration = { 0.2f, 0.2f };
        transform::Set::at(m_transform_id)->deceleration = { 0.0f, 0.0f };
        transform::Set::at(m_transform_id)->velocity_limit = { 2.0f, 4.0f };

        load_config("res/entity/player/player.cfg");
    }

    bool Object::load_config(const std::filesystem::path& path) {
        if (!entity::Object::load_config(path)) {
            return false;
        }
        m_start_sprite_offset = sprite()->offset;

        Console::log("player::Object::load_config start_sprite_offset: ", m_start_sprite_offset.x, " ", m_start_sprite_offset.y, "\n");

        std::ifstream in_file(path);
        if (!in_file) {
            Console::error("config::load ", path, " not found\n");
            return false;
        }
        std::ostringstream oss{};
        oss << in_file.rdbuf();

        const std::string text = oss.str();

        const size_t animations_label = text.find("Animations", 0);
        if (animations_label != std::string::npos) {
            size_t animations_open = text.find("{", animations_label);
            if (animations_open != std::string::npos) {
                ++animations_open;
                const size_t animations_close = text.find("\n}", animations_open);
                if (animations_close != std::string::npos) {

                    size_t current_equals = animations_open;

                    while (1) {
                        current_equals = text.find("=", current_equals + 1);
                        if (current_equals > animations_close) {
                            break;
                        }
                        const size_t end_line = text.find("\n", current_equals);
                        const size_t current_open = text.find("{", current_equals);
                        const size_t current_close = text.find("}", current_equals);

                        if (current_open < end_line && current_close < end_line) {
                            u16 source_y = 0;
                            f32 speed = 0.0f;
                            u16 num_loops = 0;

                            size_t value_0 = current_open + 1;
                            while (text.at(value_0) == '	' || text.at(value_0) == ' ') ++value_0;
                            const size_t comma_0 = text.find(",", value_0);
                            if (comma_0 < end_line) {
                                source_y = std::stoi(text.substr(value_0, comma_0 - value_0));
                            }
                            size_t value_1 = comma_0 + 1;
                            while (text.at(value_1) == '	' || text.at(value_1) == ' ') ++value_1;
                            const size_t comma_1 = text.find(",", value_1);
                            if (comma_1 < end_line) {
                                speed = std::stof(text.substr(value_1, comma_1 - value_1));
                            }
                            size_t value_2 = comma_1 + 1;
                            while (text.at(value_2) == '	' || text.at(value_2) == ' ') ++value_2;
                            size_t value_2_end = current_close;
                            while (text.at(value_2_end) == '	' || text.at(value_2_end) == ' ') --value_2_end;
                            num_loops = std::stoi(text.substr(value_2, value_2_end - value_2));

                            size_t current_label_start = text.rfind("\n", current_equals);
                            while (text.at(current_label_start) == '	' || text.at(current_label_start) == ' ' || text.at(current_label_start) == '\n') ++current_label_start;
                            size_t current_label_end = current_equals;
                            while (text.at(current_label_end - 1) == '	' || text.at(current_label_end - 1) == ' ') --current_label_end;

                            std::string current_label_str = text.substr(current_label_start, current_label_end - current_label_start);
                            //Console::log("entity::Object::load_config anim: ", current_label_str, " ", source_y, " ", speed, " ", num_loops, "\n");

                            //Console::log("entity::Object::load_config anim: ", current_label_str, ".\n");

                            m_current_anim_id = -1;
                            if (current_label_str == "crawl") {
                                m_crawl_anim_id = anim::Set::make();
                                m_current_anim_id = m_crawl_anim_id;
                            } else if (current_label_str == "down_thrust") {
                                m_down_thrust_anim_id = anim::Set::make();
                                m_current_anim_id = m_down_thrust_anim_id;
                            } else if (current_label_str == "duck") {
                                m_duck_anim_id = anim::Set::make();
                                m_current_anim_id = m_duck_anim_id;
                            } else if (current_label_str == "fall") {
                                m_fall_anim_id = anim::Set::make();
                                m_current_anim_id = m_fall_anim_id;
                            } else if (current_label_str == "run") {
                                m_run_anim_id = anim::Set::make();
                                m_current_anim_id = m_run_anim_id;
                            } else if (current_label_str == "hit_ground") {
                                m_hit_ground_anim_id = anim::Set::make();
                                m_current_anim_id = m_hit_ground_anim_id;
                            } else if (current_label_str == "hover") {
                                m_hover_anim_id = anim::Set::make();
                                m_current_anim_id = m_hover_anim_id;
                            } else if (current_label_str == "idle") {
                                m_idle_anim_id = anim::Set::make();
                                m_current_anim_id = m_idle_anim_id;
                            } /*else if (current_label_str == "jump") {
                                m_jump_anim_id = anim::Set::make();
                                m_current_anim_id = m_jump_anim_id;
                            } */else if (current_label_str == "jump_spin") {
                                m_jump_spin_anim_id = anim::Set::make();
                                m_current_anim_id = m_jump_spin_anim_id;
                            } else if (current_label_str == "jump_skid") {
                                m_jump_skid_anim_id = anim::Set::make();
                                m_current_anim_id = m_jump_skid_anim_id;
                            } else if (current_label_str == "ledge_grab") {
                                m_ledge_grab_anim_id = anim::Set::make();
                                m_current_anim_id = m_ledge_grab_anim_id;
                            } else if (current_label_str == "ledge_climb") {
                                m_ledge_climb_anim_id = anim::Set::make();
                                m_current_anim_id = m_ledge_climb_anim_id;
                            } else if (current_label_str == "lever") {
                                m_lever_anim_id = anim::Set::make();
                                m_current_anim_id = m_lever_anim_id;
                            } /*else if (current_label_str == "melee") {
                                m_melee_anim_id = anim::Set::make();
                                m_current_anim_id = m_melee_anim_id;
                            }*/ else if (current_label_str == "rise") {
                                m_rise_anim_id = anim::Set::make();
                                m_current_anim_id = m_rise_anim_id;
                            } else if (current_label_str == "skid") {
                                m_skid_anim_id = anim::Set::make();
                                m_current_anim_id = m_skid_anim_id;
                            } else if (current_label_str == "slide_ground") {
                                m_slide_ground_anim_id = anim::Set::make();
                                m_current_anim_id = m_slide_ground_anim_id;
                            } else if (current_label_str == "slide_wall") {
                                m_slide_wall_anim_id = anim::Set::make();
                                m_current_anim_id = m_slide_wall_anim_id;
                            } else if (current_label_str == "swim") {
                                m_swim_anim_id = anim::Set::make();
                                m_current_anim_id = m_swim_anim_id;
                            } else if (current_label_str == "swing") {
                                m_swing_anim_id = anim::Set::make();
                                m_current_anim_id = m_swing_anim_id;
                            } else if (current_label_str == "walk") {
                                m_walk_anim_id = anim::Set::make();
                                m_current_anim_id = m_walk_anim_id;
                            }
                            if (m_current_anim_id != -1) {
                                anim::Set::at(m_current_anim_id)->texture_size = sprite::Set::at(m_sprite_id)->texture_size(),
                                    anim::Set::at(m_current_anim_id)->source = { 0,
                                                                                 source_y,
                                                                                 sprite::Set::at(m_sprite_id)->source_rect.w,
                                                                                 sprite::Set::at(m_sprite_id)->source_rect.h };
                                anim::Set::at(m_current_anim_id)->speed = speed;
                                anim::Set::at(m_current_anim_id)->loops = num_loops;
                            }

                        }


                    }
                }
            }
        }

        const size_t aabb_names_label = text.find("AABB_Names", 0);
        if (aabb_names_label != std::string::npos) {
            size_t aabb_names_open = text.find("{", aabb_names_label);
            if (aabb_names_open != std::string::npos) {
                ++aabb_names_open;
                const size_t aabb_ids_close = text.find("\n}", aabb_names_open);
                if (aabb_ids_close != std::string::npos) {
                    size_t current_equals = aabb_names_open;

                    size_t current_label_start = aabb_names_open + 1;
                    while (1) {                        
                        while ((text.at(current_label_start) == '	' || text.at(current_label_start) == ' ' || text.at(current_label_start) == '\n')) {
                            ++current_label_start;
                        }
                        size_t current_equals = text.find("=", current_label_start);
                        if (current_equals > aabb_ids_close) {
                            break;
                        }
                        size_t current_label_end = current_equals;
                        while ((text.at(current_label_end - 1) == '	' || text.at(current_label_end - 1) == ' ') && current_label_end > current_label_start) {
                            --current_label_end;
                        }
                        const std::string current_label_str = text.substr(current_label_start, current_label_end - current_label_start);
                        Console::log("player load_config aabb name: ", current_label_str, " ");

                        const size_t end_line = text.find("\n", current_equals);
                        const size_t current_open = text.find("{", current_equals);
                        const size_t current_close = text.find("}", current_equals);

                        if (current_open > end_line || current_close > end_line) {
                            break;
                        }
                        size_t value_list_start = current_open + 1;
                        while (text.at(value_list_start) == '	' || text.at(value_list_start) == ' ') ++value_list_start;

                        size_t current_value_start = value_list_start;

                        std::vector<i32> aabb_ids;
                        while (current_value_start < end_line) {
                            //Console::log("current value: ", text.at(current_value_start), "\n");
                            size_t current_value_end = text.find(",", current_value_start);
                            if (current_value_end > end_line) {
                                current_value_end = text.find("}", current_value_start);
                                if (current_value_end > end_line) {
                                    break;
                                }
                            }
                            //Console::log("current value end - 1: ", (int)text.at(current_value_end - 1), "\n");
                            while ((text.at(current_value_end) == '	' || text.at(current_value_end) == ' ') && current_value_end > current_value_start) {
                                --current_value_end;
                            }

                            u16c id = std::stoi(text.substr(current_value_start, current_value_end - current_value_start));
                            Console::log(id, " ");
                            aabb_ids.push_back(id);
                            current_value_start = current_value_end + 1;
                        }

                        if (current_label_str == "body") {
                            for (auto& i : aabb_ids) {
                                if (!aabb::Set::at(m_aabb_ids.at(i))) break;
                                aabb::Set::at(m_aabb_ids.at(i))->name = aabb::Name::body;                                
                            }
                        } else if (current_label_str == "body_swim") {
                            for (auto& i : aabb_ids) {
                                if (!aabb::Set::at(m_aabb_ids.at(i))) break;
                                aabb::Set::at(m_aabb_ids.at(i))->name = aabb::Name::body_swim;
                                aabb::Set::at(m_aabb_ids.at(i))->is_inactive(true);
                            }
                        } else if (current_label_str == "hit_ground") {                            
                            for (auto& i : aabb_ids) {
                                if (!aabb::Set::at(m_aabb_ids.at(i))) break;
                                aabb::Set::at(m_aabb_ids.at(i))->name = aabb::Name::hit_ground;
                                aabb::Set::at(m_aabb_ids.at(i))->is_inactive(true);
                            }
                        } else if (current_label_str == "interact") {
                            for (auto& i : aabb_ids) {
                                if (!aabb::Set::at(m_aabb_ids.at(i))) break;
                                aabb::Set::at(m_aabb_ids.at(i))->name = aabb::Name::interact;
                                aabb::Set::at(m_aabb_ids.at(i))->is_inactive(true);
                            }
                        } else if (current_label_str == "melee_L") {
                            for (auto& i : aabb_ids) {
                                if (!aabb::Set::at(m_aabb_ids.at(i))) break;
                                aabb::Set::at(m_aabb_ids.at(i))->name = aabb::Name::melee_L;
                                aabb::Set::at(m_aabb_ids.at(i))->is_inactive(true);
                            }
                        } else if (current_label_str == "melee_R") {
                            for (auto& i : aabb_ids) {
                                if (!aabb::Set::at(m_aabb_ids.at(i))) break;
                                aabb::Set::at(m_aabb_ids.at(i))->name = aabb::Name::melee_R;
                                aabb::Set::at(m_aabb_ids.at(i))->is_inactive(true);
                            }
                        }
                        current_label_start = end_line;
                        Console::log("\n");
                    }
                }
            }
        }
                        


         
    


        const size_t sounds_label = text.find("Sounds", 0);
        if (sounds_label != std::string::npos) {
            size_t sounds_open = text.find("{", sounds_label);
            if (sounds_open != std::string::npos) {
                ++sounds_open;
                const size_t sounds_close = text.find("\n}", sounds_open);
                if (sounds_close != std::string::npos) {
                    size_t label_start = sounds_open + 1;
                    while (1) {
                        const size_t end_line = text.find("\n", label_start + 1);
                        if (end_line > sounds_close) {
                            break;
                        }
                        while ((text.at(label_start) == '	' || text.at(label_start) == ' ' || text.at(label_start) == '\n') && label_start < end_line) {
                            ++label_start;
                        }
                        size_t current_equals = text.find("=", label_start);
                        if (current_equals > sounds_close) {
                            break;
                        }
                        size_t label_end = text.find("=", label_start);
                        while ((text.at(label_end - 1) == '	' || text.at(label_end - 1) == ' ') && label_end > label_start) {
                            --label_end;
                        }
                        const std::string sound_label = text.substr(label_start, label_end - label_start);

                        size_t value_start = current_equals + 1;
                        while ((text.at(value_start) == '	' || text.at(value_start) == ' ') && value_start < end_line) {
                            ++value_start;
                        }
                        size_t value_end = end_line;
                        while ((text.at(value_end - 1) == '	' || text.at(value_end - 1) == ' ') && value_end > value_start) {
                            --value_end;
                        }
                        const std::filesystem::path sound_path = "res/sound/" + text.substr(value_start, value_end - value_start);
                        Console::log("player::Object::load_config ", sound_label, "=", sound_path, "\n");
                        //Console::log("player::Object::load_config ", sound_label, "\n");

                        if      (sound_label == "bump_head")    m_bump_head_sound_id   = sound::Set::make(sound_path);
                        else if (sound_label == "down_thrust")  m_down_thrust_sound_id = sound::Set::make(sound_path);
                        else if (sound_label == "hit_ground")   m_hit_ground_sound_id  = sound::Set::make(sound_path);
                        else if (sound_label == "hover") {
                            m_hover_sound_id = sound::Set::make(sound_path);
                            sound::Set::at(m_hover_sound_id)->is_looped(true);
                        }
                        else if (sound_label == "jump")         m_jump_sound_id        = sound::Set::make(sound_path);
                        else if (sound_label == "ledge_climb")  m_ledge_climb_sound_id = sound::Set::make(sound_path);
                        else if (sound_label == "ledge_grab")   m_ledge_grab_sound_id  = sound::Set::make(sound_path);
                        else if (sound_label == "lever")        m_lever_sound_id       = sound::Set::make(sound_path);
                        else if (sound_label == "melee")        m_melee_sound_id       = sound::Set::make(sound_path);
                        else if (sound_label == "melee_hit")    m_melee_hit_sound_id   = sound::Set::make(sound_path);
                        else if (sound_label == "pick_up")      m_pick_up_sound_id     = sound::Set::make(sound_path);
                        else if (sound_label == "skid") {
                            m_skid_sound_id = sound::Set::make(sound_path);
                            sound::Set::at(m_skid_sound_id)->is_looped(false);
                        }
                        else if (sound_label == "slide_wall")   m_slide_wall_sound_id   = sound::Set::make(sound_path);
                        else if (sound_label == "step_grass")   m_step_grass_sound_id   = sound::Set::make(sound_path);
                        else if (sound_label == "swing_attach") m_swing_attach_sound_id = sound::Set::make(sound_path);
                        else if (sound_label == "swing_detach") m_swing_detach_sound_id = sound::Set::make(sound_path);
                        else if (sound_label == "toss")         m_toss_sound_id         = sound::Set::make(sound_path);
                        label_start = end_line;
                    }
                }
            }
        }
        return true;
    }

    void player::Object::update() {
        if (!is_all_valid()) return;

        if (m_time_left_hurt > 0) {
            if (m_time_left_hurt == m_time_to_hurt) {
                if (sound::Set::at(m_hurt_sound_id)) {
                    sound::Set::at(m_hurt_sound_id)->position({ position().x / 160.0f, position().y / 90.0f });
                    sound::Set::at(m_hurt_sound_id)->play();
                }
            }
            --m_time_left_hurt;
        }

        switch (m_state) {
        case    entity::State::run:     update_run();   break;
        case    entity::State::swim:    update_swim();  break;
        case    entity::State::swing:   update_swing(); break; }

        //anim::Set::at(m_current_anim_id)->update();
        sprite()->source_rect.x = anim::Set::at(m_current_anim_id)->source.x;
        sprite()->source_rect.y = anim::Set::at(m_current_anim_id)->source.y;

        //Console::log("is_carrying: ", m_is_carrying, "\n");
        //Console::log("is to write save: ", m_is_to_write_save, "\n");
        //Console::log("sprite offset: ", sprite()->offset.x, " ", sprite()->offset.y, "\n");

        if (m_time_left_in_state > 0) {
            --m_time_left_in_state;
            //Console::log("time_left_in_state: ", (int)m_time_left_in_state, "\n");
        }
        //Console::log("level pos: ", position_difference(level_transform_id).x, "\n");
    }



    Object::~Object() {
        Console::log("player::Object::~Object() input\n");
        input::Set::erase(m_input_id);
        //Console::log("~Player() transform\n");
        //transform::Set::erase(m_transform_id);
        //Console::log("~Player() sprite\n");
        //sprite::Set::erase(m_sprite_id);

        sound::Set::erase(m_bump_head_sound_id);
        sound::Set::erase(m_hover_sound_id);
        sound::Set::erase(m_jump_sound_id);
        sound::Set::erase(m_ledge_climb_sound_id);
        sound::Set::erase(m_ledge_grab_sound_id);
        sound::Set::erase(m_melee_sound_id);
        sound::Set::erase(m_skid_sound_id);
        sound::Set::erase(m_slide_wall_sound_id);
        sound::Set::erase(m_step_grass_sound_id);

        m_input_id = -1;

        anim::Set::erase(m_idle_anim_id);
        anim::Set::erase(m_run_anim_id);
        anim::Set::erase(m_jump_anim_id);
        m_idle_anim_id = -1;
        m_run_anim_id = -1;
        m_jump_anim_id = -1;
        m_current_anim_id = -1;
    }
}