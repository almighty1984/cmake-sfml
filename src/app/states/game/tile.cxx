module;
#include <filesystem>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>

module tile;
import collider;
import console;
import sprite;

void tile::Object::load_config(const std::filesystem::path& path) {
    std::ifstream in_file(path);
    if (!in_file) {
        Console::error("config::load ", path, " not found\n");
        return;
    }
    std::ostringstream oss{};
    oss << in_file.rdbuf();

    const std::string text = oss.str();

    size_t aabb_set = text.find("AABB_Set", 0);
    if (aabb_set != std::string::npos) {

        size_t aabb_set_open = text.find("{", aabb_set);
        if (aabb_set_open != std::string::npos) {
            ++aabb_set_open;

            size_t aabb_set_close = text.find("\n}", aabb_set_open);
            if (aabb_set_close != std::string::npos) {
                Color color = { 255, 255, 255 };
                size_t color_label = text.find("color", aabb_set_open);

                size_t position_after_color = aabb_set_open;
                if (color_label < aabb_set_close) {
                    size_t end_line = text.find("\n", color_label);

                    position_after_color = end_line;

                    size_t color_open = text.find("{", color_label);
                    size_t color_close = text.find("}", color_open);

                    if (color_open < end_line && color_close < end_line) {
                        size_t comma_0 = text.find(",", color_open);
                        size_t comma_1 = text.find(",", comma_0 + 1);

                        size_t red = color_open + 1;
                        while (text.at(red) == '	' || text.at(red) == ' ') ++red;

                        color.r = (u8)std::stoi(text.substr(red, comma_0 - red));

                        size_t green = comma_0 + 1;
                        while (text.at(green) == '	' || text.at(green) == ' ') ++green;
                        color.g = (u8)std::stoi(text.substr(green, comma_1 - green));

                        size_t blue = comma_1 + 1;
                        while (text.at(blue) == '	' || text.at(blue) == ' ') ++blue;

                        color.b = (u8)std::stoi(text.substr(blue, color_close - green));

                        //Console::log("color: ", (int)color.r, " ", (int)color.g, " ", (int)color.b, "\n");
                    }
                }

                std::vector<Rectf> out_rects;

                while (aabb_set != std::string::npos) {
                    size_t aabb_open = text.find("{", position_after_color);
                    if (aabb_open != std::string::npos) {

                        while (aabb_open < aabb_set_close) {
                            size_t end_line = text.find("\n", aabb_open);
                            size_t aabb_close = text.find("}", aabb_open);
                            if (aabb_close < end_line) {

                                Rectf rect;

                                size_t value_0 = aabb_open + 1;
                                while (text.at(value_0) == '	' || text.at(value_0) == ' ') ++value_0;
                                size_t comma_0 = text.find(",", value_0);
                                rect.x = std::stof(text.substr(value_0, comma_0 - value_0));

                                size_t value_1 = comma_0 + 1;
                                while (text.at(value_1) == '	' || text.at(value_1) == ' ') ++value_1;
                                size_t comma_1 = text.find(",", value_1);
                                rect.y = std::stof(text.substr(value_1, comma_1 - value_1));

                                size_t value_2 = comma_1 + 1;
                                while (text.at(value_2) == '	' || text.at(value_2) == ' ') ++value_2;
                                size_t comma_2 = text.find(",", value_2);
                                rect.w = std::stof(text.substr(value_2, comma_2 - value_2));

                                size_t value_3 = comma_2 + 1;
                                while (text.at(value_3) == '	' || text.at(value_3) == ' ') ++value_3;
                                size_t comma_3 = text.find(",", value_3);
                                rect.h = std::stof(text.substr(value_3, comma_3 - value_3));

                                out_rects.push_back(rect);

                                aabb_open = text.find("{", end_line);
                            }
                        }
                        aabb_set = text.find("AABB_Set", aabb_set + 1);
                    }
                }

                std::vector<i32> aabb_ids{};
                for (auto& i : out_rects) {
                    //Console::log("Tile::load_config, ", i.x, " ", i.y, " ", i.w, " ", i.h, "\n");

                    Vec2fc offset = sprite::Set::at(m_sprite_id) ? sprite::Set::at(m_sprite_id)->offset : Vec2f{ 0.0f, 0.0f };

                    i32c id = collider::aabb::Set::make(m_transform_id, { offset.x + i.x, offset.y + i.y, i.w, i.h });

                    //Console::log("Tile::load_config, ", offset.x + i.x, " ", offset.y + i.y, " ", i.w, " ", i.h, "\n");

                    //i32c id = collider::aabb::Set::make(transform_id, { transform()->position.x + i.x, transform()->position.y + i.y, i.w, i.h });


                    collider::aabb::Set::at(id)->color(color);
                    collider::aabb::Set::at(id)->start_color(color);
                    collider::aabb::Set::at(id)->parent = this;
                    //collider::aabb::Set::at(id)->sprite_type = sprite_type();
                    collider::aabb::Set::at(id)->update();
                    aabb_ids.push_back(id);
                }
                //m_aabb_id = collider::aabb::Set::make(m_transform_id, { 8,8,8,8 });

                //info.color = out_color;
                m_aabb_ids = aabb_ids;
            }
        }
    }











    bool is_hidden = false;
    const size_t sprite_label = text.find("Sprite", 0);
    if (sprite_label != std::string::npos) {
        size_t sprite_open = text.find("{", sprite_label);
        if (sprite_open != std::string::npos) {
            ++sprite_open;
            const size_t sprite_close = text.find("\n}", sprite_open);
            if (sprite_close != std::string::npos) {
                const size_t is_hidden_label = text.find("is_hidden", sprite_open);
                if (is_hidden_label < sprite_close) {
                    const size_t end_line = text.find("\n", is_hidden_label);
                    size_t is_hidden_start = text.find("=", is_hidden_label);
                    if (is_hidden_start < end_line) {
                        ++is_hidden_start;
                        while (text.at(is_hidden_start) == '	' || text.at(is_hidden_start) == ' ') ++is_hidden_start;
                        is_hidden = text.substr(is_hidden_start, end_line - is_hidden_start) == "true" ? true : false;
                    }
                }
                const size_t texture_label = text.find("texture", sprite_open);
                if (texture_label < sprite_close) {
                    const size_t end_line = text.find("\n", texture_label);
                    size_t texture_start = text.find("=", texture_label);
                    if (texture_start < end_line) {
                        ++texture_start;
                        while (text.at(texture_start) == '	' || text.at(texture_start) == ' ') ++texture_start;
                        std::filesystem::path texture_path = "res/textures/" + text.substr(texture_start, end_line - texture_start);
                        sprite()->texture(texture_path);

                        //Console::log("texture: ", texture_path, "\n");
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
                            Recti source_rect = {};
                            size_t value_0 = source_rect_open + 1;
                            while (text.at(value_0) == '	' || text.at(value_0) == ' ') ++value_0;
                            const size_t comma_0 = text.find(",", value_0);
                            if (comma_0 < end_line) {
                                source_rect.x = std::stoi(text.substr(value_0, comma_0 - value_0));
                            }
                            size_t value_1 = comma_0 + 1;
                            while (text.at(value_1) == '	' || text.at(value_1) == ' ') ++value_1;
                            const size_t comma_1 = text.find(",", value_1);
                            if (comma_1 < end_line) {
                                source_rect.y = std::stoi(text.substr(value_1, comma_1 - value_1));
                            }
                            size_t value_2 = comma_1 + 1;
                            while (text.at(value_2) == '	' || text.at(value_2) == ' ') ++value_2;
                            const size_t comma_2 = text.find(",", value_2);
                            if (comma_2 < end_line) {
                                source_rect.w = std::stoi(text.substr(value_2, comma_2 - value_2));
                            }

                            size_t value_3 = comma_2 + 1;
                            while (text.at(value_3) == '	' || text.at(value_3) == ' ') ++value_3;
                            size_t value_3_end = source_rect_close;
                            while (text.at(value_3_end) == '	' || text.at(value_3_end) == ' ') --value_3_end;
                                
                            source_rect.h = std::stoi(text.substr(value_3, value_3_end - value_3));                         
                            sprite()->source_rect = source_rect;

                            Console::log("tile::Object::load_config source_rect: ", source_rect.x, " ", source_rect.y, " ", source_rect.w, " ", source_rect.h, "\n");
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
                            Vec2f offset = {};
                            size_t value_0 = offset_open + 1;
                            while (text.at(value_0) == '	' || text.at(value_0) == ' ') ++value_0;
                            const size_t comma_0 = text.find(",", value_0);
                            if (comma_0 < end_line) {
                                offset.x = std::stof(text.substr(value_0, comma_0 - value_0));
                            }

                            size_t value_1 = comma_0 + 1;
                            while (text.at(value_1) == '	' || text.at(value_1) == ' ') ++value_1;
                            size_t value_1_end = offset_close;
                            while (text.at(value_1_end) == '	' || text.at(value_1_end) == ' ') --value_1_end;

                            offset.y = std::stoi(text.substr(value_1, value_1_end - value_1));

                            sprite()->offset = offset;
                            Console::log("tile::Object::load_config offset: ", offset.x, " ", offset.y, "\n");
                        }
                    }
                }
            }
        }
    }







    //sprite()->is_hidden = is_hidden;
    sprite::Set::at(m_sprite_id)->is_hidden = is_hidden;
}