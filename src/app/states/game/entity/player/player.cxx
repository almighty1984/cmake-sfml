module;
#include <fstream>
#include <sstream>
module player;
import anim;
import collider.aabb;
import console;
import health;
import input;
import particle;
import sound;
import sprite;
import transform;
import types;

player::Object::Object() {
    Console::log("Player()\n");
    m_input_id = input::Manager::make();
    input::Manager::get(m_input_id)->id = m_input_id;
        
    m_type = entity::Type::player;
    m_state = m_next_state = entity::State::run;

    //m_health_id = health::Manager::make(32.0f, m_transform_id);
    //health::Manager::get(m_health_id)->offset = { -8.0f, -16.0f };

    load_config("res/entity/player/player.cfg");
}

player::Object::~Object() {
    Console::log("player::Object::~Object() input\n");
    input::Manager::erase(m_input_id);
    //Console::log("~Player() transform\n");
    //transform::Manager::erase(m_transform_id);
    //Console::log("~Player() sprite\n");
    //sprite::Manager::erase(m_sprite_id);

    sound::Manager::erase(m_hover_sound_id);
    sound::Manager::erase(m_jump_sound_id);
    sound::Manager::erase(m_ledge_climb_sound_id);
    sound::Manager::erase(m_ledge_grab_sound_id);
    sound::Manager::erase(m_melee_sound_id);
    sound::Manager::erase(m_skid_sound_id);
    sound::Manager::erase(m_slide_wall_sound_id);
    sound::Manager::erase(m_step_grass_sound_id);

    anim::Manager::erase(m_idle_anim_id);
    anim::Manager::erase(m_run_anim_id);
    anim::Manager::erase(m_jump_anim_id);    
    m_current_anim_id = -1;
}

bool player::Object::load_config(const std::filesystem::path& path) {
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
                        U16 source_y = 0;
                        F32 speed = 0.0F;
                        U16 num_loops = 0;

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
                            m_crawl_anim_id = anim::Manager::make();
                            m_current_anim_id = m_crawl_anim_id;
                        } else if (current_label_str == "down_thrust") {
                            m_down_thrust_anim_id = anim::Manager::make();
                            m_current_anim_id = m_down_thrust_anim_id;
                        } else if (current_label_str == "duck") {
                            m_duck_anim_id = anim::Manager::make();
                            m_current_anim_id = m_duck_anim_id;
                        } else if (current_label_str == "fall") {
                            m_fall_anim_id = anim::Manager::make();
                            m_current_anim_id = m_fall_anim_id;
                        } else if (current_label_str == "run") {
                            m_run_anim_id = anim::Manager::make();
                            m_current_anim_id = m_run_anim_id;
                        } else if (current_label_str == "hit_ground") {
                            m_hit_ground_anim_id = anim::Manager::make();
                            m_current_anim_id = m_hit_ground_anim_id;
                        } else if (current_label_str == "hover") {
                            m_hover_anim_id = anim::Manager::make();
                            m_current_anim_id = m_hover_anim_id;
                        } else if (current_label_str == "jump_spin") {
                            m_jump_spin_anim_id = anim::Manager::make();
                            m_current_anim_id = m_jump_spin_anim_id;
                        } else if (current_label_str == "jump_skid") {
                            m_jump_skid_anim_id = anim::Manager::make();
                            m_current_anim_id = m_jump_skid_anim_id;
                        } else if (current_label_str == "ledge_grab") {
                            m_ledge_grab_anim_id = anim::Manager::make();
                            m_current_anim_id = m_ledge_grab_anim_id;
                        } else if (current_label_str == "ledge_climb") {
                            m_ledge_climb_anim_id = anim::Manager::make();
                            m_current_anim_id = m_ledge_climb_anim_id;
                        } else if (current_label_str == "lever") {
                            m_lever_anim_id = anim::Manager::make();
                            m_current_anim_id = m_lever_anim_id;
                        } else if (current_label_str == "rise") {
                            m_rise_anim_id = anim::Manager::make();
                            m_current_anim_id = m_rise_anim_id;
                        } else if (current_label_str == "skid") {
                            m_skid_anim_id = anim::Manager::make();
                            m_current_anim_id = m_skid_anim_id;
                        } else if (current_label_str == "slide_ground") {
                            m_slide_ground_anim_id = anim::Manager::make();
                            m_current_anim_id = m_slide_ground_anim_id;
                        } else if (current_label_str == "slide_wall") {
                            m_slide_wall_anim_id = anim::Manager::make();
                            m_current_anim_id = m_slide_wall_anim_id;
                        } else if (current_label_str == "swim") {
                            m_swim_anim_id = anim::Manager::make();
                            m_current_anim_id = m_swim_anim_id;
                        } else if (current_label_str == "swing") {
                            m_swing_anim_id = anim::Manager::make();
                            m_current_anim_id = m_swing_anim_id;
                        } else if (current_label_str == "walk") {
                            m_walk_anim_id = anim::Manager::make();
                            m_current_anim_id = m_walk_anim_id;
                        }
                        if (m_current_anim_id != -1) {
                            anim::Manager::get(m_current_anim_id)->texture_size = sprite::Manager::get(m_sprite_id)->texture_size(),
                                anim::Manager::get(m_current_anim_id)->source = { 0,
                                                                                source_y,
                                                                                sprite::Manager::get(m_sprite_id)->source_rect.w,
                                                                                sprite::Manager::get(m_sprite_id)->source_rect.h };
                            anim::Manager::get(m_current_anim_id)->speed = speed;
                            anim::Manager::get(m_current_anim_id)->loops = num_loops;
                            anim::Manager::get(m_current_anim_id)->type  = anim::string_to_type(current_label_str);;
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
                    const std::filesystem::path sound_path = "res/sound/" + text.substr(value_start, value_end - value_start);
                    //Console::log("player::Object::load_config ", sound_label, "=", sound_path, "\n");
                    //Console::log("player::Object::load_config ", sound_label, "\n");

                    if      (sound_label == "down_thrust")  m_down_thrust_sound_id = sound::Manager::make(sound_path);
                    else if (sound_label == "hit_ground")   m_hit_ground_sound_id  = sound::Manager::make(sound_path);
                    else if (sound_label == "hover") {
                        m_hover_sound_id = sound::Manager::make(sound_path);
                        sound::Manager::get(m_hover_sound_id)->is_looped(true);
                    }
                    else if (sound_label == "jump")         m_jump_sound_id        = sound::Manager::make(sound_path);
                    else if (sound_label == "ledge_climb")  m_ledge_climb_sound_id = sound::Manager::make(sound_path);
                    else if (sound_label == "ledge_grab")   m_ledge_grab_sound_id  = sound::Manager::make(sound_path);
                    else if (sound_label == "lever")        m_lever_sound_id       = sound::Manager::make(sound_path);                    
                    else if (sound_label == "pick_up")      m_pick_up_sound_id     = sound::Manager::make(sound_path);
                    else if (sound_label == "skid") {
                        m_skid_sound_id = sound::Manager::make(sound_path);
                        sound::Manager::get(m_skid_sound_id)->is_looped(false);
                    }
                    else if (sound_label == "slide_wall")   m_slide_wall_sound_id   = sound::Manager::make(sound_path);
                    else if (sound_label == "step_grass")   m_step_grass_sound_id   = sound::Manager::make(sound_path);                    
                    else if (sound_label == "swing_detach") m_swing_detach_sound_id = sound::Manager::make(sound_path);
                    else if (sound_label == "toss")         m_toss_sound_id         = sound::Manager::make(sound_path);                    
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
        --m_time_left_hurt;
    }
    if (m_time_left_interacting > 0) {
        --m_time_left_interacting;
    }
    if (m_is_sliding_wall) {
        ++m_time_sliding_wall;
    } else {
        m_time_sliding_wall = 0;
    }

    health()->layer = m_start_layer;

    if (m_next_state != m_state) {
        m_prev_state = m_state;
        m_state = m_next_state;
        m_is_first_state_update = true;
    }
    switch (m_state) {
    case    entity::State::dead:    update_dead();  break;
    case    entity::State::run:     update_run();   break;
    case    entity::State::swim:    update_swim();  break;
    case    entity::State::swing:   update_swing(); break; }

    //anim::Manager::get(m_current_anim_id)->update();
    sprite()->source_rect.x = anim::Manager::get(m_current_anim_id)->source.x;
    sprite()->source_rect.y = anim::Manager::get(m_current_anim_id)->source.y;

    //Console::log("is_carrying: ", m_is_carrying, "\n");
    //Console::log("is to write save: ", m_is_to_write_save, "\n");
    //Console::log("sprite offset: ", sprite()->offset.x, " ", sprite()->offset.y, "\n");

    if (m_time_left_in_state > 0) {
        --m_time_left_in_state;
        //Console::log("time_left_in_state: ", (int)m_time_left_in_state, "\n");
    }
    //Console::log("level pos: ", position_difference(level_transform_id).x, "\n");
}


void player::Object::hurt(entity::Object* culprit) {
    if (!culprit) return;    

    if (culprit->type() == entity::Type::particle_melee) {
        if (!culprit->parent()) {
            return;
        }
        if (culprit->parent()->type() == entity::Type::frog) {
            health()->amount -= 16.0f;            
            Console::log("player hurt by: ", entity::to_string(culprit->parent()->type()), " health: ", health()->amount, "\n");
        }
    }
}

void player::Object::interact(entity::Object* object) {
    if (!object || !object->transform() || !sprite() || !transform() || m_time_left_interacting > 0) return;
    
    m_time_left_interacting = m_time_to_interact;

    if (object->type() == entity::Type::brick || object->type() == entity::Type::bug) {
        if (!object->sprite() || object->state() == entity::State::swim) return;
                
        if (!m_is_carrying && !object->parent()) {
            m_is_carrying = true;
            object->parent(this);
            object->sprite()->layer = sprite()->layer + 1;
            //Console::log("player::Object::interact pick up bug layer: ", (int)object->sprite()->layer, "\n");
            if (sound::Manager::get(m_pick_up_sound_id)) {
                sound::Manager::get(m_pick_up_sound_id)->position({ position().x / 160.0F, position().y / 90.0F });
                sound::Manager::get(m_pick_up_sound_id)->play();
            }
        }
        else if (object->parent() && m_is_carrying) {
            if (sprite()->is_leftward && object->is_near_wall_L()) {
                return;
            }
            if (!sprite()->is_leftward && object->is_near_wall_R()) {
                return;
            }


            m_is_carrying = false;
            object->sprite()->layer = object->start_layer();            
            object->parent(nullptr);
            object->transform()->velocity = (transform()->velocity + transform()->moved_velocity);
            //other.owner->transform()->position.x += other.owner->transform()->velocity.x;
            if (sprite()->is_leftward) {
                object->transform()->velocity.x -= 2.0F;
                object->transform()->position.x -= 8.0F;
            } else {
                object->transform()->velocity.x += 2.0F;
                object->transform()->position.x += 8.0F;
            }
            //Console::log("object->transform()->velocity.x: ", object->transform()->velocity.x, "\n");

            object->transform()->velocity.y -= 1.5F;
            object->next_state(entity::State::tossed);
            object->time_left_interacting(10);

            if (sound::Manager::get(m_toss_sound_id)) {
                sound::Manager::get(m_toss_sound_id)->position({ position().x / 160.0F, position().y / 90.0F });
                sound::Manager::get(m_toss_sound_id)->play();
            }
        }
    }
    else if (object->type() == entity::Type::clip_ledge) {        
        //Console::log("player::Object::interact clip_ledge ", sprite()->is_leftward, "\n");
        m_is_on_ledge = true;
        m_is_climbing_ledge = true;
        m_is_sliding_wall = false;
        press(key_up);
        m_is_wall_to_left = sprite()->is_leftward;
        //transform()->position.y = object->other_rect.y - 8;
        transform()->velocity.y = 0.0F;
        reset_anim(m_ledge_climb_anim_id);
    }
    else if (object->type() == entity::Type::trigger) {
        
        m_time_left_lever = m_time_to_lever;
        reset_anim(m_lever_anim_id);
        sprite()->is_leftward = object->is_dead();
        
        if (object->is_dead()) {
            object->time_left_alive(U16_MAX);
            object->time_left_dead(0);
        } else {
            object->time_left_dead(U16_MAX);
            object->time_left_alive(0);
        }

        if (sound::Manager::get(m_lever_sound_id)) {
            sound::Manager::get(m_lever_sound_id)->position({ position().x / 160.0F, position().y / 90.0F });
            sound::Manager::get(m_lever_sound_id)->play();
        }
    }

}

void player::Object::spawn_down_thrust(cVec2F position) {
    cVec2F dimensions = { 32.0F, 32.0F };
    cF32 speed = 3.0F;

    particle::cType type = particle::Type::down_thrust;

    cVec2F p = { position.x - dimensions.x / 2.0F, position.y - dimensions.y / 2.0F };

    // L
    particle::Manager::spawn({ .parent = this,
                               .type = type,
                               .position = p,
                               .velocity = { -speed, 0.0F } });
    // R
    particle::Manager::spawn({ .parent = this,
                               .type = type,
                               .position = p,
                               .velocity = {  speed, 0.0F } });

}