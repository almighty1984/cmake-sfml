module;
#include <filesystem>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>

module entity;
import camera;
import collider.aabb;
import console;
import sound;
import sprite;

entity::Object::~Object() {    
    Camera::remove_transform_id(m_transform_id);
    sprite::Set::erase(m_sprite_id);
    transform::Set::erase(m_transform_id);
    m_sprite_id = -1;
    m_transform_id = -1;

    for (auto& i : m_aabb_ids) {
        aabb::Set::erase(i);
    }
    m_aabb_ids.clear();
}
bool entity::Object::load_config(const std::filesystem::path& path) {
    std::ifstream in_file(path);
    if (!in_file) {
        Console::error("config::load ", path, " not found\n");
        return false;
    }
    std::ostringstream oss{};
    oss << in_file.rdbuf();

    const std::string text = oss.str();



    const size_t transform_label = text.find("Transform", 0);
    if (transform_label != std::string::npos) {
        size_t transform_open = text.find("{", transform_label);
        if (transform_open != std::string::npos && m_transform_id == -1) {
            ++transform_open;

            m_transform_id = transform::Set::make();

            //transform::Set::at(m_transform_id)->position = m_start_offset;

            //Console::log("entity ", entity::type_to_string(m_type), " camera add transform: ", m_transform_id, "\n");
            Camera::add_transform_id(m_transform_id);


            const size_t transform_close = text.find("\n}", transform_open);
            if (transform_close != std::string::npos) {
                const size_t velocity_limit_label = text.find("velocity_limit", transform_open);
                size_t end_line = text.find("\n", velocity_limit_label);
                size_t velocity_limit_start = text.find("=", velocity_limit_label);
                if (velocity_limit_start < end_line && transform::Set::at(m_transform_id)) {
                    const size_t velocity_limit_open = text.find("{", velocity_limit_start);
                    const size_t velocity_limit_close = text.find("}", velocity_limit_start);

                    if (velocity_limit_open < end_line && velocity_limit_close < end_line) {
                        size_t value_0 = velocity_limit_open + 1;
                        while (text.at(value_0) == '	' || text.at(value_0) == ' ') ++value_0;
                        const size_t comma_0 = text.find(",", value_0);
                        if (comma_0 < end_line) {
                            transform::Set::at(m_transform_id)->velocity_limit.x = std::stof(text.substr(value_0, comma_0 - value_0));
                        }
                        size_t value_1 = comma_0 + 1;
                        while (text.at(value_1) == '	' || text.at(value_1) == ' ') ++value_1;
                        size_t value_1_end = velocity_limit_close;
                        while (text.at(value_1_end) == '	' || text.at(value_1_end) == ' ') --value_1_end;

                        transform::Set::at(m_transform_id)->velocity_limit.y = std::stof(text.substr(value_1, value_1_end - value_1));
                    }
                }


                const size_t acceleration_label = text.find("acceleration", transform_open);
                end_line = text.find("\n", acceleration_label);
                size_t acceleration_start = text.find("=", acceleration_label);
                if (acceleration_start < end_line && transform::Set::at(m_transform_id)) {
                    const size_t acceleration_open = text.find("{", acceleration_start);
                    const size_t acceleration_close = text.find("}", acceleration_start);

                    if (acceleration_open < end_line && acceleration_close < end_line) {
                        size_t value_0 = acceleration_open + 1;
                        while (text.at(value_0) == '	' || text.at(value_0) == ' ') ++value_0;
                        const size_t comma_0 = text.find(",", value_0);
                        if (comma_0 < end_line) {
                            transform::Set::at(m_transform_id)->acceleration.x = std::stof(text.substr(value_0, comma_0 - value_0));
                        }
                        size_t value_1 = comma_0 + 1;
                        while (text.at(value_1) == '	' || text.at(value_1) == ' ') ++value_1;
                        size_t value_1_end = acceleration_close;
                        while (text.at(value_1_end) == '	' || text.at(value_1_end) == ' ') --value_1_end;

                        transform::Set::at(m_transform_id)->acceleration.y = std::stof(text.substr(value_1, value_1_end - value_1));
                    }
                }

                const size_t deceleration_label = text.find("deceleration", transform_open);
                end_line = text.find("\n", deceleration_label);
                size_t deceleration_start = text.find("=", deceleration_label);
                if (deceleration_start < end_line && transform::Set::at(m_transform_id)) {
                    const size_t deceleration_open = text.find("{", deceleration_start);
                    const size_t deceleration_close = text.find("}", deceleration_start);

                    if (deceleration_open < end_line && deceleration_close < end_line) {
                        size_t value_0 = deceleration_open + 1;
                        while (text.at(value_0) == '	' || text.at(value_0) == ' ') ++value_0;
                        const size_t comma_0 = text.find(",", value_0);
                        if (comma_0 < end_line) {
                            transform::Set::at(m_transform_id)->deceleration.x = std::stof(text.substr(value_0, comma_0 - value_0));
                        }
                        size_t value_1 = comma_0 + 1;
                        while (text.at(value_1) == '	' || text.at(value_1) == ' ') ++value_1;
                        size_t value_1_end = deceleration_close;
                        while (text.at(value_1_end) == '	' || text.at(value_1_end) == ' ') --value_1_end;

                        transform::Set::at(m_transform_id)->deceleration.y = std::stof(text.substr(value_1, value_1_end - value_1));
                    }
                }

            }
        }
    }



    size_t aabb_set_label = text.find("AABB_Set", 0);
    if (aabb_set_label != std::string::npos) {

        size_t aabb_set_open = text.find("{", aabb_set_label);
        if (aabb_set_open != std::string::npos) {
            ++aabb_set_open;

            size_t aabb_set_close = text.find("\n}", aabb_set_open);
            if (aabb_set_close != std::string::npos) {
                std::vector<i32> aabb_ids{};
                                
                size_t aabb_open = text.find("{", aabb_set_open + 1);

                while (aabb_open < aabb_set_close) {

                    Rectf aabb_rect{};
                    aabb::Name aabb_name = aabb::Name::null;
                    
                    size_t end_line = text.find("\n", aabb_open);
                    size_t aabb_close = text.find("}", aabb_open);

                    size_t prev_endl = text.rfind("\n", aabb_open);
                    if (aabb_close < end_line) {

                        // See if there's a name in front
                        size_t name_end = text.rfind("=", aabb_open);

                        if (name_end > prev_endl && name_end != std::string::npos) {
                            //Console::log("\n\nname end: ", text.at(name_end), "\n\n");
                            //--name_end;
                            while (text.at(name_end - 1) == '	' || text.at(name_end - 1) == ' ') {
                                --name_end;
                                //Console::log("\nname end: ", text.at(name_end), "\n");
                                //Console::log("name_end: ", name_end, "\n");
                            }
                            if (name_end > prev_endl) {
                                size_t name_start = name_end;                                        
                                while (text.at(name_start - 1) != '	' && text.at(name_start - 1) != ' ' && name_start > prev_endl) {
                                    --name_start;
                                }

                                const std::string aabb_name_str = text.substr(name_start, name_end - name_start);

                                aabb_name = aabb::string_to_name(aabb_name_str);

                                //Console::log("\n\n", path, " aabb_name_str: ", aabb_name_str, "\n");
                            }
                        }

                        //Rectf rect;

                        size_t value_0 = aabb_open + 1;
                        while (text.at(value_0) == '	' || text.at(value_0) == ' ') ++value_0;
                        size_t comma_0 = text.find(",", value_0);
                        aabb_rect.x = std::stof(text.substr(value_0, comma_0 - value_0));

                        size_t value_1 = comma_0 + 1;
                        while (text.at(value_1) == '	' || text.at(value_1) == ' ') ++value_1;
                        size_t comma_1 = text.find(",", value_1);
                        aabb_rect.y = std::stof(text.substr(value_1, comma_1 - value_1));

                        size_t value_2 = comma_1 + 1;
                        while (text.at(value_2) == '	' || text.at(value_2) == ' ') ++value_2;
                        size_t comma_2 = text.find(",", value_2);
                        aabb_rect.w = std::stof(text.substr(value_2, comma_2 - value_2));

                        size_t value_3 = comma_2 + 1;
                        while (text.at(value_3) == '	' || text.at(value_3) == ' ') ++value_3;
                        size_t comma_3 = text.find(",", value_3);
                        aabb_rect.h = std::stof(text.substr(value_3, comma_3 - value_3));

                        //out_rects.push_back(rect);

                        aabb_open = text.find("{", end_line);
                    }

                    i32c aabb_id = aabb::Set::make(m_transform_id, { m_start_offset.x + aabb_rect.x,
                                                                     m_start_offset.y + aabb_rect.y,
                                                                     aabb_rect.w,
                                                                     aabb_rect.h });
                    aabb::Set::at(aabb_id)->id = aabb_id;
                    aabb::Set::at(aabb_id)->name = aabb_name;
                    aabb::Set::at(aabb_id)->owner = this;
                    aabb::Set::at(aabb_id)->update();
                    aabb_ids.push_back(aabb_id);

                    aabb_set_label = aabb_set_close;
                }
                m_aabb_ids = aabb_ids;
            }
        }
    }


    size_t aabb_colors_label = text.find("AABB_Colors", 0);
    if (aabb_colors_label != std::string::npos) {

        size_t aabb_colors_open = text.find("{", aabb_colors_label);
        if (aabb_colors_open != std::string::npos) {
            ++aabb_colors_open;

            size_t aabb_colors_close = text.find("\n}", aabb_colors_open);
            if (aabb_colors_close != std::string::npos) {
                std::vector<i32> aabb_ids{};

                size_t color_open = text.find("{", aabb_colors_open + 1);

                while (color_open < aabb_colors_close) {
                    aabb::Name aabb_name = aabb::Name::null;
                    Color aabb_color;

                    size_t end_line = text.find("\n", color_open);
                    size_t color_close = text.find("}", color_open);

                    size_t prev_endl = text.rfind("\n", color_open);
                    if (color_close < end_line) {
                        size_t name_end = text.rfind("=", color_open);

                        if (name_end > prev_endl && name_end != std::string::npos) {
                            //Console::log("\n\nname end: ", text.at(name_end), "\n\n");
                            //--name_end;
                            while (text.at(name_end - 1) == '	' || text.at(name_end - 1) == ' ') {
                                --name_end;
                                //Console::log("\nname end: ", text.at(name_end), "\n");
                                //Console::log("name_end: ", name_end, "\n");
                            }
                            if (name_end > prev_endl) {
                                size_t name_start = name_end;
                                while (text.at(name_start - 1) != '	' && text.at(name_start - 1) != ' ' && name_start > prev_endl) {
                                    --name_start;
                                }

                                const std::string aabb_name_str = text.substr(name_start, name_end - name_start);

                                aabb_name = aabb::string_to_name(aabb_name_str);
                            }
                        }

                        size_t value_0 = color_open + 1;
                        while (text.at(value_0) == '	' || text.at(value_0) == ' ') ++value_0;
                        size_t comma_0 = text.find(",", value_0);
                        aabb_color.r = std::stoi(text.substr(value_0, comma_0 - value_0));

                        size_t value_1 = comma_0 + 1;
                        while (text.at(value_1) == '	' || text.at(value_1) == ' ') ++value_1;
                        size_t comma_1 = text.find(",", value_1);
                        aabb_color.g = std::stoi(text.substr(value_1, comma_1 - value_1));

                        size_t value_2 = comma_1 + 1;
                        while (text.at(value_2) == '	' || text.at(value_2) == ' ') ++value_2;
                        size_t comma_2 = text.find(",", value_2);
                        aabb_color.b = std::stoi(text.substr(value_2, comma_2 - value_2));

                        color_open = text.find("{", end_line);
                    }

                    for (auto& aabb_id : m_aabb_ids) {
                        if (aabb::Set::at(aabb_id) && aabb::Set::at(aabb_id)->name == aabb_name) {
                            aabb::Set::at(aabb_id)->color(aabb_color);
                            aabb::Set::at(aabb_id)->start_color(aabb_color);
                        }
                    }

                    aabb_set_label = aabb_colors_close;
                }
            }
        }
    }




    const size_t config_label = text.find("Config", 0);
    if (config_label != std::string::npos) {
        size_t config_open = text.find("{", config_label);
        if (config_open != std::string::npos) {
            ++config_open;
            const size_t config_close = text.find("\n}", config_open);
            if (config_close != std::string::npos) {
                const size_t time_to_be_alive_label = text.find("time_to_be_alive", config_open);
                if (time_to_be_alive_label < config_close) {
                    const size_t end_line = text.find("\n", time_to_be_alive_label);
                    size_t value_start = text.find("=", time_to_be_alive_label);
                    if (value_start < end_line) {
                        ++value_start;
                        while (text.at(value_start) == '	' || text.at(value_start) == ' ') ++value_start;
                        m_time_to_be_alive = std::stoi(text.substr(value_start, end_line - value_start));
                    }
                }

                const size_t time_to_be_dead_label = text.find("time_to_be_dead", config_open);
                if (time_to_be_dead_label < config_close) {
                    const size_t end_line = text.find("\n", time_to_be_dead_label);
                    size_t value_start = text.find("=", time_to_be_dead_label);
                    if (value_start < end_line) {
                        ++value_start;
                        while (text.at(value_start) == '	' || text.at(value_start) == ' ') ++value_start;
                        m_time_to_be_dead = std::stoi(text.substr(value_start, end_line - value_start));
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

                    Console::log(path, " sound: ", sound_path, "\n\n");
                    if      (sound_label == "bounce") m_bounce_sound_id = sound::Set::make(sound_path);
                    else if (sound_label == "dead")   m_dead_sound_id   = sound::Set::make(sound_path);
                    else if (sound_label == "hurt")   m_hurt_sound_id   = sound::Set::make(sound_path);
                    else if (sound_label == "melee")  m_melee_sound_id  = sound::Set::make(sound_path);
                    label_start = end_line;
                }
            }
        }
    }


    const size_t sprite_label = text.find("Sprite", 0);
    if (sprite_label != std::string::npos) {
        size_t sprite_open = text.find("{", sprite_label);
        if (sprite_open != std::string::npos) {
            ++sprite_open;
            const size_t sprite_close = text.find("\n}", sprite_open);
            if (sprite_close != std::string::npos) {

                bool sprite_is_debug    = false,
                     sprite_is_hidden   = false,
                     sprite_is_leftward = false,
                     sprite_is_upended  = false;

                std::filesystem::path sprite_texture_path{};

                Recti sprite_source_rect = { 0, 0, 16, 16 };
                Vec2f sprite_offset{};

                const size_t texture_label = text.find("texture", sprite_open);
                if (texture_label < sprite_close) {
                    const size_t end_line = text.find("\n", texture_label);
                    size_t texture_start = text.find("=", texture_label);
                    if (texture_start < end_line) {
                        ++texture_start;
                        while (text.at(texture_start) == '	' || text.at(texture_start) == ' ') ++texture_start;
                        sprite_texture_path = "res/texture/" + text.substr(texture_start, end_line - texture_start);


                        //Console::log("texture: ", texture_path, "\n");
                    }
                }

                const size_t is_debug_label = text.find("is_debug", sprite_open);
                if (is_debug_label < sprite_close) {
                    const size_t end_line = text.find("\n", is_debug_label);
                    size_t is_debug_start = text.find("=", is_debug_label);
                    if (is_debug_start < end_line) {
                        ++is_debug_start;
                        while (text.at(is_debug_start) == '	' || text.at(is_debug_start) == ' ') ++is_debug_start;
                        sprite_is_debug = text.substr(is_debug_start, end_line - is_debug_start) == "true" ? true : false;
                    }
                }

                const size_t is_hidden_label = text.find("is_hidden", sprite_open);
                if (is_hidden_label < sprite_close) {
                    const size_t end_line = text.find("\n", is_hidden_label);
                    size_t is_hidden_start = text.find("=", is_hidden_label);
                    if (is_hidden_start < end_line) {
                        ++is_hidden_start;
                        while (text.at(is_hidden_start) == '	' || text.at(is_hidden_start) == ' ') ++is_hidden_start;
                        sprite_is_hidden = text.substr(is_hidden_start, end_line - is_hidden_start) == "true" ? true : false;
                    }
                }

                const size_t is_leftward_label = text.find("is_leftward", sprite_open);
                if (is_leftward_label < sprite_close) {
                    const size_t end_line = text.find("\n", is_leftward_label);
                    size_t is_leftward_start = text.find("=", is_leftward_label);
                    if (is_leftward_start < end_line) {
                        ++is_leftward_start;
                        while (text.at(is_leftward_start) == '	' || text.at(is_leftward_start) == ' ') ++is_leftward_start;
                        sprite_is_leftward = text.substr(is_leftward_start, end_line - is_leftward_start) == "true" ? true : false;
                    }
                }

                const size_t is_upended_label = text.find("is_upended", sprite_open);
                if (is_upended_label < sprite_close) {
                    const size_t end_line = text.find("\n", is_upended_label);
                    size_t is_upended_start = text.find("=", is_upended_label);
                    if (is_upended_start < end_line) {
                        ++is_upended_start;
                        while (text.at(is_upended_start) == '	' || text.at(is_upended_start) == ' ') ++is_upended_start;
                        sprite_is_upended = text.substr(is_upended_start, end_line - is_upended_start) == "true" ? true : false;
                    }
                }

                const size_t source_rect_label = text.find("source_rect", sprite_open);
                if (source_rect_label < sprite_close) {
                    const size_t end_line = text.find("\n", source_rect_label);
                    const size_t source_rect_start = text.find("=", source_rect_label);
                    if (source_rect_start < end_line) {
                        const size_t source_rect_open = text.find("{", source_rect_start);
                        const size_t source_rect_close = text.find("}", source_rect_start);

                        if (source_rect_open < end_line && source_rect_close < end_line) {                            
                            size_t value_0 = source_rect_open + 1;
                            while (text.at(value_0) == '	' || text.at(value_0) == ' ') ++value_0;
                            const size_t comma_0 = text.find(",", value_0);
                            if (comma_0 < end_line) {
                                sprite_source_rect.x = std::stoi(text.substr(value_0, comma_0 - value_0));
                            }
                            size_t value_1 = comma_0 + 1;
                            while (text.at(value_1) == '	' || text.at(value_1) == ' ') ++value_1;
                            const size_t comma_1 = text.find(",", value_1);
                            if (comma_1 < end_line) {
                                sprite_source_rect.y = std::stoi(text.substr(value_1, comma_1 - value_1));
                            }
                            size_t value_2 = comma_1 + 1;
                            while (text.at(value_2) == '	' || text.at(value_2) == ' ') ++value_2;
                            const size_t comma_2 = text.find(",", value_2);
                            if (comma_2 < end_line) {
                                sprite_source_rect.w = std::stoi(text.substr(value_2, comma_2 - value_2));
                            }

                            size_t value_3 = comma_2 + 1;
                            while (text.at(value_3) == '	' || text.at(value_3) == ' ') ++value_3;
                            size_t value_3_end = source_rect_close;
                            while (text.at(value_3_end) == '	' || text.at(value_3_end) == ' ') --value_3_end;

                            sprite_source_rect.h = std::stoi(text.substr(value_3, value_3_end - value_3));                            

                            //Console::log("entity::Object::load_config source_rect: ", sprite_source_rect.x, " ", sprite_source_rect.y, " ", sprite_source_rect.w, " ", sprite_source_rect.h, "\n");                            
                        }
                    }
                }

                const size_t offset_label = text.find("offset", sprite_open);
                if (offset_label < sprite_close) {
                    const size_t end_line = text.find("\n", offset_label);
                    size_t offset_start = text.find("=", offset_label);
                    if (offset_start < end_line) {
                        const size_t offset_open = text.find("{", offset_start);
                        const size_t offset_close = text.find("}", offset_start);

                        if (offset_open < end_line && offset_close < end_line) {
                            
                            size_t value_0 = offset_open + 1;
                            while (text.at(value_0) == '	' || text.at(value_0) == ' ') ++value_0;
                            const size_t comma_0 = text.find(",", value_0);
                            if (comma_0 < end_line) {
                                sprite_offset.x = std::stof(text.substr(value_0, comma_0 - value_0));
                            }
                            size_t value_1 = comma_0 + 1;
                            while (text.at(value_1) == '	' || text.at(value_1) == ' ') ++value_1;
                            size_t value_1_end = offset_close;
                            while (text.at(value_1_end) == '	' || text.at(value_1_end) == ' ') --value_1_end;

                            sprite_offset.y = std::stof(text.substr(value_1, value_1_end - value_1));

                            //Console::log("entity::Object::load_config offset: ", offset.x, " ", offset.y, "\n");
                        }
                    }
                }


                if (sprite_texture_path.empty()) {
                    sprite_texture_path = "res/texture/set_255.png";
                }

                if (!sprite_texture_path.empty()) {
                    /*if (m_sprite_id != -1) {
                        sprite::Set::erase(m_sprite_id);
                    }*/
                    m_sprite_id = sprite::Set::make(sprite_texture_path);

                    sprite::Set::at(m_sprite_id)->id = m_sprite_id;
                    sprite::Set::at(m_sprite_id)->layer = m_start_layer;
                    sprite::Set::at(m_sprite_id)->is_debug = sprite_is_debug;
                    sprite::Set::at(m_sprite_id)->is_hidden = sprite_is_hidden;
                    sprite::Set::at(m_sprite_id)->is_leftward = sprite_is_leftward;
                    sprite::Set::at(m_sprite_id)->is_upended = sprite_is_upended;
                    sprite::Set::at(m_sprite_id)->source_rect = sprite_source_rect;
                    sprite::Set::at(m_sprite_id)->origin = { sprite_source_rect.w / 2.0f, sprite_source_rect.h / 2.0f };

                    sprite::Set::at(m_sprite_id)->offset = m_start_offset + sprite_offset; // Add to already set level offset

                    sprite::Set::at(m_sprite_id)->transform_id = m_transform_id;


                    //Console::log(path, " is_hidden: ", sprite_is_hidden, "\n");
                }
            }
        }

    }

    

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

                        m_current_anim_id = -1;
                        if (current_label_str == "bounce") {
                            m_bounce_anim_id = anim::Set::make();
                            m_current_anim_id = m_bounce_anim_id;
                        }
                        else if (current_label_str == "dead") {
                            m_dead_anim_id = anim::Set::make();
                            m_current_anim_id = m_dead_anim_id;
                        }
                        else if (current_label_str == "hurt") {
                            m_hurt_anim_id = anim::Set::make();
                            m_current_anim_id = m_hurt_anim_id;
                        }
                        else if (current_label_str == "idle") {
                            m_idle_anim_id = anim::Set::make();
                            m_current_anim_id = m_idle_anim_id;
                        }
                        else if (current_label_str == "jump") {
                            m_jump_anim_id = anim::Set::make();
                            m_current_anim_id = m_jump_anim_id;
                        }
                        else if (current_label_str == "melee") {
                            m_melee_anim_id = anim::Set::make();
                            m_current_anim_id = m_melee_anim_id;
                        }
                        else if (current_label_str == "upended") {
                            m_upended_anim_id = anim::Set::make();
                            m_current_anim_id = m_upended_anim_id;
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

    return true;
}