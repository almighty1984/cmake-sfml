module;
#include <fstream>
#include <sstream>
module player;
import anim;
import collider;
import console;
import input;
import sprite;
import transform;
import types;

void Player::load_config(const std::filesystem::path& path) {
    Object::load_config(path);

    std::ifstream in_file(path);
    if (!in_file) {
        Console::error("config::load ", path, " not found\n");
        return;
    }
    std::ostringstream oss{};
    oss << in_file.rdbuf();

    std::string text = oss.str();

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
                        Console::log("tile::Object::load_config anim: ", current_label_str, ".\n");

                        m_current_anim_id = -1;
                        if (current_label_str == "idle") {
                            m_idle_anim_id = anim::Set::make();
                            m_current_anim_id = m_idle_anim_id;
                        } else if (current_label_str == "walk") {
                            m_walk_anim_id = anim::Set::make();
                            m_current_anim_id = m_walk_anim_id;
                        } else if (current_label_str == "run") {
                            m_run_anim_id = anim::Set::make();
                            m_current_anim_id = m_run_anim_id;
                        } else if (current_label_str == "turn") {
                            m_turn_anim_id = anim::Set::make();
                            m_current_anim_id = m_turn_anim_id;
                        } else if (current_label_str == "jump") {
                            m_jump_anim_id = anim::Set::make();
                            m_current_anim_id = m_jump_anim_id;
                        } else if (current_label_str == "jump_spin") {
                            m_jump_spin_anim_id = anim::Set::make();
                            m_current_anim_id = m_jump_spin_anim_id;
                        } else if (current_label_str == "fall") {
                            m_fall_anim_id = anim::Set::make();
                            m_current_anim_id = m_fall_anim_id;
                        } else if (current_label_str == "slide_wall") {
                            m_slide_wall_anim_id = anim::Set::make();
                            m_current_anim_id = m_slide_wall_anim_id;
                        } else if (current_label_str == "ledge_grab") {
                            m_ledge_grab_anim_id = anim::Set::make();
                            m_current_anim_id = m_ledge_grab_anim_id;
                        } else if (current_label_str == "ledge_climb") {
                            m_ledge_climb_anim_id = anim::Set::make();
                            m_current_anim_id = m_ledge_climb_anim_id;
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
}

Player::Player() {
    Console::log("Player()\n");

    m_input_id = input::Set::make();

    m_transform_id = transform::Set::make();
    transform::Set::at(m_transform_id)->acceleration = { 0.2f, 0.2f };
    transform::Set::at(m_transform_id)->deceleration = { 0.04f, 0.04f };
    transform::Set::at(m_transform_id)->velocity_limit = { 2.0f, 4.0f };

    m_sprite_id = sprite::Set::make("res/textures/tile_selection.png");
    sprite::Set::at(m_sprite_id)->transform_id = m_transform_id;
    sprite::Set::at(m_sprite_id)->type = sprite::Type::player;

    //m_id = NUM_ADDED_PLAYERS++;

    load_config("res/players/player.cfg");

    m_start_velocity_limit = transform()->velocity_limit;

}
Player::~Player() {
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
    
    m_input_id     = -1;
    m_transform_id = -1;
    m_sprite_id    = -1;

    anim::Set::erase(m_idle_anim_id);
    anim::Set::erase(m_run_anim_id);
    anim::Set::erase(m_jump_anim_id);
    m_idle_anim_id    = -1;
    m_run_anim_id     = -1;
    m_jump_anim_id    = -1;
    m_current_anim_id = -1;
}