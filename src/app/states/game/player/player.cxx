module;
#include <fstream>
#include <sstream>
module player;
import anim;
import collider;
import console;
import input;
import sound;
import sprite;
import transform;
import types;

void player::Object::load_config(const std::filesystem::path& path) {
    tile::Object::load_config(path);

    std::ifstream in_file(path);
    if (!in_file) {
        Console::error("config::load ", path, " not found\n");
        return;
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
                        //Console::log("tile::Object::load_config anim: ", current_label_str, " ", source_y, " ", speed, " ", num_loops, "\n");
                        
                        //Console::log("tile::Object::load_config anim: ", current_label_str, ".\n");

                        m_current_anim_id = -1;
                        if (current_label_str == "crawl") {
                            m_crawl_anim_id = anim::Set::make();
                            m_current_anim_id = m_crawl_anim_id;
                        } else if (current_label_str == "duck") {
                            m_duck_anim_id = anim::Set::make();
                            m_current_anim_id = m_duck_anim_id;
                        } else if (current_label_str == "fall") {
                            m_fall_anim_id = anim::Set::make();
                            m_current_anim_id = m_fall_anim_id;
                        } else if (current_label_str == "run") {
                            m_run_anim_id = anim::Set::make();
                            m_current_anim_id = m_run_anim_id;
                        } else if (current_label_str == "jump") {
                            m_jump_anim_id = anim::Set::make();
                            m_current_anim_id = m_jump_anim_id;
                        } else if (current_label_str == "jump_spin") {
                            m_jump_spin_anim_id = anim::Set::make();
                            m_current_anim_id = m_jump_spin_anim_id;
                        } else if (current_label_str == "jump_skid") {
                            m_jump_skid_anim_id = anim::Set::make();
                            m_current_anim_id = m_jump_skid_anim_id;
                        } else if (current_label_str == "hover") {
                            m_hover_anim_id = anim::Set::make();
                            m_current_anim_id = m_hover_anim_id;
                        } else if (current_label_str == "idle") {
                            m_idle_anim_id = anim::Set::make();
                            m_current_anim_id = m_idle_anim_id;
                        } else if (current_label_str == "ledge_grab") {
                            m_ledge_grab_anim_id = anim::Set::make();
                            m_current_anim_id = m_ledge_grab_anim_id;
                        } else if (current_label_str == "ledge_climb") {
                            m_ledge_climb_anim_id = anim::Set::make();
                            m_current_anim_id = m_ledge_climb_anim_id;
                        } else if (current_label_str == "melee") {
                            m_melee_anim_id = anim::Set::make();
                            m_current_anim_id = m_melee_anim_id;
                        } else if (current_label_str == "rise") {
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
                    
                    const std::filesystem::path sound_path = "res/sounds/" + text.substr(value_start, value_end - value_start);

                    Console::log("player::Object::load_config ", sound_label, "=", sound_path, "\n");
                    
                    //Console::log("player::Object::load_config ", sound_label, "\n");
                    if (sound_label == "bump_head") {
                        m_bump_head_sound_id = sound::Set::make(sound_path);
                    } else if (sound_label == "hover") {
                        m_hover_sound_id = sound::Set::make(sound_path);
                        sound::Set::at(m_hover_sound_id)->is_looped(true);
                    } else if (sound_label == "jump") {
                        m_jump_sound_id = sound::Set::make(sound_path);
                    } else if (sound_label == "ledge_climb") {
                        m_ledge_climb_sound_id = sound::Set::make(sound_path);
                    } else if (sound_label == "ledge_grab") {
                        m_ledge_grab_sound_id = sound::Set::make(sound_path);
                    } else if (sound_label == "melee") {
                        m_melee_sound_id = sound::Set::make(sound_path);
                    } else if (sound_label == "skid") {
                        m_skid_sound_id = sound::Set::make(sound_path);
                        sound::Set::at(m_skid_sound_id)->is_looped(false);
                    } else if (sound_label == "slide_wall") {
                        m_slide_wall_sound_id = sound::Set::make(sound_path);
                    } else if (sound_label == "step_grass") {
                        m_step_grass_sound_id = sound::Set::make(sound_path);
                    }
                    
                    label_start = end_line;
                }
            }
        }
    }
}

player::Object::Object() {
    Console::log("Player()\n");

    m_input_id = input::Set::make();

    m_transform_id = transform::Set::make();
    

    m_sprite_id = sprite::Set::make("res/textures/tile_selection.png");
    sprite::Set::at(m_sprite_id)->transform_id = m_transform_id;
    sprite::Set::at(m_sprite_id)->type = sprite::Type::player;

    //m_id = NUM_ADDED_PLAYERS++;

    load_config("res/players/player.cfg");
    
}
player::Object::~Object() {
    Console::log("~player::Object()\n");
    Console::log("~player::Object() input\n");
    input::Set::erase(m_input_id);
    Console::log("~Player() transform\n");
    transform::Set::erase(m_transform_id);
    Console::log("~Player() sprite\n");
    sprite::Set::erase(m_sprite_id);

    for (auto& i : m_aabb_ids) {
        collider::aabb::Set::erase(i);
    }

    m_input_id = -1;
    m_transform_id = -1;
    m_sprite_id = -1;

    anim::Set::erase(m_idle_anim_id);
    anim::Set::erase(m_run_anim_id);
    anim::Set::erase(m_jump_anim_id);
    m_idle_anim_id = -1;
    m_run_anim_id = -1;
    m_jump_anim_id = -1;
    m_current_anim_id = -1;
}

void player::Object::update() {
    if (!is_all_valid()) return;

    switch (m_state) {
    case State::run:  update_run();  break;
    case State::swim: update_swim(); break;
    }

    anim::Set::at(m_current_anim_id)->update();
    sprite()->source_rect.x = anim::Set::at(m_current_anim_id)->source.x;
    sprite()->source_rect.y = anim::Set::at(m_current_anim_id)->source.y;
}