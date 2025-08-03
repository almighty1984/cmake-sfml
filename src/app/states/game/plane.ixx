module;
#include <fstream>
#include <filesystem>
#include <vector>
export module plane;
import console;
import sprite;
import transform;
import types;

export namespace plane {
    class Object {
        std::vector<i32>   m_sprite_ids;
        std::vector<i32>   m_transform_ids;
        std::vector<Vec2f> m_scroll_amount;

        i32 m_id_stride = 0;

        u8 m_num_layers  = 0,
           m_num_rows    = 0,
           m_num_columns = 0;
    public:
        Object() {}

        std::vector<i32> const get_transform_ids() { return m_transform_ids; }

        void set_velocity(Vec2fc v) {
            for (u8 layer = 0; layer < m_num_layers; layer += 1) {
                transform::Set::at(m_transform_ids.front() + layer)->velocity.x = v.x * (m_scroll_amount.at(layer)).x;
                //transform::Set::at(m_transform_ids.front() + layer)->velocity.y = v.y;
            }
        }
        void create(u8c columns, u8c rows) {
            load_config("res/plane/forest.cfg", columns, rows);
        }
        bool load_config(const std::filesystem::path& path, u8c columns, u8c rows) {
            std::ifstream in_file(path);
            if (!in_file) {
                Console::error("plane::Object::load_config() ", path, " not found!\n");
                return false;
            }
            std::ostringstream oss{};
            oss << in_file.rdbuf();

            const std::string text = oss.str();

            size_t start_find_pos = 0;

            while (start_find_pos != std::string::npos) {
                const size_t plane_label = text.find("Plane", start_find_pos);
                if (plane_label == std::string::npos) {
                    break;
                }
                //Console::log("plane::Object::load_config plane_label: ", plane_label, "\n");
                start_find_pos = plane_label + 5;                                
                size_t plane_open = text.find("{", plane_label);
                //Console::log("plane::Object::load_config start_find_pos: ", start_find_pos, "\n");
                std::filesystem::path texture_path;
                Recti source_rect = {};
                Vec2f velocity = {};

                if (plane_open != std::string::npos) {
                    ++plane_open;
                    const size_t plane_close = text.find("\n}", plane_open);
                    if (plane_close != std::string::npos) {
                        const size_t texture_label = text.find("texture", plane_open);
                        if (texture_label < plane_close) {
                            const size_t end_line = text.find("\n", texture_label);
                            size_t texture_start = text.find("=", texture_label);
                            if (texture_start < end_line) {
                                ++texture_start;
                                while (text.at(texture_start) == '	' || text.at(texture_start) == ' ') ++texture_start;
                                texture_path = "res/texture/" + text.substr(texture_start, end_line - texture_start);
                                //Console::log("texture: ", texture_path, "\n");
                            }
                        }
                        const size_t source_rect_label = text.find("source_rect", plane_open);
                        if (source_rect_label < plane_close) {
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
                                    //Console::log("entity::Object::load_config source_rect: ", source_rect.x, " ", source_rect.y, " ", source_rect.w, " ", source_rect.h, "\n");
                                }
                            }
                        }
                        const size_t velocity_label = text.find("velocity", plane_open);
                        if (velocity_label < plane_close) {
                            const size_t end_line = text.find("\n", velocity_label);
                            const size_t velocity_start = text.find("=", velocity_label);
                            if (velocity_start < end_line) {
                                const size_t velocity_open = text.find("{", velocity_start);
                                const size_t velocity_close = text.find("}", velocity_start);

                                if (velocity_open < end_line && velocity_close < end_line) {

                                    size_t value_0 = velocity_open + 1;
                                    while (text.at(value_0) == '	' || text.at(value_0) == ' ') ++value_0;
                                    const size_t comma_0 = text.find(",", value_0);
                                    if (comma_0 < end_line) {
                                        velocity.x = std::stof(text.substr(value_0, comma_0 - value_0));
                                    }
                                    size_t value_1 = comma_0 + 1;
                                    while (text.at(value_1) == '	' || text.at(value_1) == ' ') ++value_1;
                                    size_t value_1_end = velocity_close;
                                    while (text.at(value_1_end) == '	' || text.at(value_1_end) == ' ') --value_1_end;

                                    velocity.y = std::stof(text.substr(value_1, value_1_end - value_1));
                                    //Console::log("plane::Object::load_config velocity : ", velocity.x, " ", velocity.y, "\n");
                                }
                            }
                        }
                    }
                }
                if (!texture_path.empty()) {
                    m_transform_ids.push_back(transform::Set::make());
                    //Console::log("\nplane layer: ", (int)m_num_layers, " transform id: ", m_transform_ids.back(), "\n\n");
                    m_id_stride = rows * columns;
                    m_scroll_amount.push_back(velocity);
                    for (u8 y = 0; y < rows; ++y) {
                        for (u8 x = 0; x < columns; ++x) {
                            m_sprite_ids.push_back(sprite::Set::make(texture_path));
                            sprite::Set::at(m_sprite_ids.back())->transform_id = m_transform_ids.back();
                            sprite::Set::at(m_sprite_ids.back())->offset = Vec2f{ (f32)x * source_rect.w, (f32)y * source_rect.h };
                            sprite::Set::at(m_sprite_ids.back())->source_rect = source_rect;
                            sprite::Set::at(m_sprite_ids.back())->layer = 0;
                        }
                    }
                    m_num_rows = rows;
                    m_num_columns = columns;
                    ++m_num_layers;
                }
            }
            //Console::log("plane m_num_layers: ", (int)m_num_layers, "\n");
            return true;
        }

        void update() {
            for (auto& i : m_transform_ids) {
                //transform::Set::at(i)->velocity = m_velocity;
                transform::Set::at(i)->position.y = 0.0f;
            }            
            for (u8 i = 0; i < m_num_layers; i += 1) {
                if (transform::Set::at(m_transform_ids.front() + i)->position.x < -sprite::Set::at(m_sprite_ids.front())->source_rect.w) {
                    transform::Set::at(m_transform_ids.front() + i)->position.x += sprite::Set::at(m_sprite_ids.front())->source_rect.w;
                }
                if (transform::Set::at(m_transform_ids.front() + i)->position.x > 0) {
                    transform::Set::at(m_transform_ids.front() + i)->position.x -= sprite::Set::at(m_sprite_ids.front())->source_rect.w;
                }
            }
            //Console::log("hello plane ", m_id_stride, "\n");
        }
    };
}