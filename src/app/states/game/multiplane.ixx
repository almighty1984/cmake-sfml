module;
#include <fstream>
#include <filesystem>
#include <vector>
#include <map>
export module plane;
import console;
import sprite;
import transform;
import types;

export namespace multiplane {
    class Object {
        std::vector<I32>   m_sprite_ids;
        //std::vector<I32>   m_transform_ids;
        std::vector<Vec2F> m_scroll_amount;

        std::map<I32, I32> m_transform_ids_map;

        U8 m_num_layers  = 0,
           m_num_rows    = 0,
           m_num_columns = 0;
    public:
        Object() : m_sprite_ids(),
                   m_transform_ids_map(),
                   m_scroll_amount(),                   
                   m_num_layers(0),
                   m_num_columns(0) {            
        }                    
        ~Object() {
            Console::log("multimultiplane::Object::~Object()\n");
            clear();
        }
        void clear() {
            Console::log("multimultiplane::Object::clear()\n");
            for (auto& i : m_transform_ids_map) {
                transform::Manager::erase(i.second);
            }
            m_transform_ids_map.clear();
            for (auto& i : m_sprite_ids) {
                sprite::Manager::erase(i);
            }
            m_sprite_ids.clear();
            m_scroll_amount.clear();
        }
        std::vector<I32> get_sprite_ids() const { return m_sprite_ids; }
        //std::vector<I32> const get_transform_ids() { return m_transform_ids; }

        void set_velocity(cVec2F v) {
            if (m_transform_ids_map.empty()) return;

            for (U8 layer = 0; layer < m_num_layers; layer += 1) {
                if (layer < m_scroll_amount.size()) {
                    if (transform::Manager::get(m_transform_ids_map.at(layer))) {
                        transform::Manager::get(m_transform_ids_map.at(layer))->velocity.x = v.x * (m_scroll_amount.at(layer)).x;
                        //transform::Manager::get(m_transform_ids.front() + layer)->velocity.y = v.y;
                    }
                }
            }
        }
        void create(cU8 columns, cU8 rows) {
            Console::log("multiplane::Object::create ", (int)columns, "x", (int)rows, "\n");
            load_config("res/multiplane/forest.cfg", columns, rows);
        }
        bool load_config(const std::filesystem::path& path, cU8 columns, cU8 rows) {
            std::ifstream in_file(path);
            if (!in_file) {
                Console::error("multiplane::Object::load_config() ", path, " not found!\n");
                return false;
            }
            std::ostringstream oss{};
            oss << in_file.rdbuf();

            in_file.close();

            const std::string text = oss.str();

            size_t start_find_pos = 0;

            //clear();

            while (1) {
                const size_t plane_label = text.find("Plane", start_find_pos);
                if (plane_label == std::string::npos) {
                    break;
                }
                Console::log("multiplane::Object::load_config plane_label: ", plane_label, "\n");
                start_find_pos = plane_label + 5;                                
                size_t plane_open = text.find("{", plane_label);
                //Console::log("multiplane::Object::load_config start_find_pos: ", start_find_pos, "\n");
                std::filesystem::path texture_path{};
                RectI source_rect = {};
                Vec2F velocity = {};

                U8 layer = 0;

                if (plane_open == std::string::npos) {
                    Console::log("plane_open == std::string::npos\n");
                    break;
                }

                ++plane_open;
                size_t plane_close = plane_open;

                while (1) {
                    plane_close = text.find("}", plane_close);
                    if (plane_close == std::string::npos) return false;

                    ++plane_close;
                    while (text.at(plane_close) == '	' || text.at(plane_close) == ' ' || text.at(plane_close) == '\n') ++plane_close;
                    if (text.at(plane_close) == '}') {
                        break;
                    }
                    
                }

                /*if (plane_close == std::string::npos) {
                    Console::log("plane_close == std::string::npos\n");
                    break;
                }*/

                const size_t texture_label = text.find("texture", plane_open);
                if (texture_label < plane_close) {
                    const size_t end_line = text.find("\n", texture_label);
                    size_t texture_start = text.find("=", texture_label);
                    if (texture_start < end_line) {
                        ++texture_start;
                        while (text.at(texture_start) == '	' || text.at(texture_start) == ' ') ++texture_start;
                        texture_path = "res/texture/" + text.substr(texture_start, end_line - texture_start);
                        //Console::log("texture: ", texture_path, "\n");

                        start_find_pos = end_line;
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

                            start_find_pos = end_line;
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
                            //Console::log("multiplane::Object::load_config velocity : ", velocity.x, " ", velocity.y, "\n");
                        }
                        start_find_pos = end_line;
                    }
                }

                if (!texture_path.empty()) {
                    //m_transform_ids.emplace_back(transform::Manager::make());

                    m_transform_ids_map.emplace(m_num_layers, transform::Manager::make());

                    m_scroll_amount.emplace_back(velocity);
                    //Console::log("\nplane layer: ", (int)m_num_layers, " transform id: ", m_transform_ids.back(), "\n\n");
                    
                    for (U8 y = 0; y < rows; ++y) {
                        for (U8 x = 0; x < columns; ++x) {
                            m_sprite_ids.emplace_back(sprite::Manager::make(texture_path));
                            sprite::Manager::get(m_sprite_ids.back())->transform_id = m_transform_ids_map.at(m_num_layers);
                            sprite::Manager::get(m_sprite_ids.back())->offset = Vec2F{ (F32)(x * source_rect.w), (F32)(y * source_rect.h) };

                            //Console::log("multiplane sprite offset: ", sprite::Manager::get(m_sprite_ids.back())->offset.x, "\n\n");
                            sprite::Manager::get(m_sprite_ids.back())->source_rect = source_rect;
                            sprite::Manager::get(m_sprite_ids.back())->layer = m_num_layers;
                            sprite::Manager::get(m_sprite_ids.back())->is_bg = true;
                        }
                    }
                    m_num_rows = rows;
                    m_num_columns = columns;
                    ++m_num_layers;
                }
            }
            //Console::log("plane m_num_layers: ", (int)m_num_layers, "\n");

            Console::log("multiplane::Object transforms size: ", m_transform_ids_map.size(), "\n");

            Console::log("multiplane::Object sprite_ids size: ", m_sprite_ids.size(), "\n");
            return true;
        }

        void update() {
            if (m_transform_ids_map.empty()) return;

            //Console::log("plane transforms: ", m_transform_ids.size(), "\n");

            /*Console::log("plane transform ids: ");
            for (auto& i : m_transform_ids) {
                Console::log(i, " ");
            }
            Console::log("\n");*/

            
            /*for (U8 i = 0; i < m_num_layers; i += 1) {
                if (!sprite::Manager::get(m_sprite_ids.front()) || !transform::Manager::get(m_transform_id_map.at(i))) continue;

                if (transform::Manager::get(m_transform_id_map.at(i))->position.x < -sprite::Manager::get(m_sprite_ids.front())->source_rect.w) {
                    transform::Manager::get(m_transform_id_map.at(i))->position.x += sprite::Manager::get(m_sprite_ids.front())->source_rect.w;
                }
                if (transform::Manager::get(m_transform_id_map.at(i))->position.x > 0) {
                    transform::Manager::get(m_transform_id_map.at(i))->position.x -= sprite::Manager::get(m_sprite_ids.front())->source_rect.w;
                }
            }*/

            for (auto& i : m_transform_ids_map) {
                transform::Manager::get(i.second)->position.y = 0.0F;
                if (transform::Manager::get(i.second)->position.x < -256.0F) {
                    transform::Manager::get(i.second)->position.x += 256.0F;
                }
                if (transform::Manager::get(i.second)->position.x > 0.0F) {
                    transform::Manager::get(i.second)->position.x -= 256.0F;
                }
            }



            //Console::log("hello plane ", m_id_stride, "\n");
        }
    };
}