module;
#include <filesystem>
#include <fstream>

export module config;
import console;
import state;
import types;

export class Config {
    static inline u8          m_scale      = 1;
    static inline state::Type m_state_type = state::Type::Game;
public:
    static u8c         scale() { return m_scale; }
    static state::Type state_type() { return m_state_type; }

    static std::string file_to_string(std::filesystem::path path) {
        std::ifstream in_file(path);
        if (!in_file) {
            Console::error("Config::file_to_string ", path, " not found\n");
            return {};
        }
        std::ostringstream oss{};
        oss << in_file.rdbuf();
        in_file.close();
        return oss.str();
    }

    static bool load(std::filesystem::path path) {
        std::ifstream in_file(path);
        if (!in_file) {
            Console::error("config::load ", path, " not found\n");
            return false;
        }
        std::string text = file_to_string(path);
        std::string state_string = state::to_string(m_state_type);

        size_t start_tag = text.find("start_state", 0);
        size_t i = 0;
        if (start_tag != std::string::npos) {
            size_t end_line = text.find("\n", start_tag);
            size_t equals = text.find("=", start_tag);
            if (equals < end_line) {
                i = equals + 1;
                while (i < end_line && (text.at(i) == '=' || text.at(i) == ' ' || text.at(i) == '	')) {
                    ++i;
                }
                state_string = text.substr(i, end_line - i);
                Console::log("Config::load found start: ", state_string.c_str(), "\n");
            }
        }
        m_state_type = state::type_from(state_string);


        size_t scale_tag = text.find("scale", 0);
        i = 0;
        if (scale_tag != std::string::npos) {
            size_t end_line = text.find("\n", scale_tag);
            size_t equals = text.find("=", scale_tag);
            if (equals < end_line) {
                i = equals + 1;
                while (i < end_line && (text.at(i) == '=' || text.at(i) == ' ' || text.at(i) == '	')) {
                    ++i;
                }
                m_scale = std::stoi(text.substr(i, end_line - i));
                Console::log("Config::load found scale: ", (int)m_scale, "\n");
            }
        }

        /*const size_t tag_start = text.find(state_string, 0);
        if (tag_start == std::string::npos) {
            Console::error("config::load ", state_string.c_str(), " not found\n");
            return false;
        }
        const size_t bracket_open = text.find("{", tag_start);
        if (tag_start == std::string::npos) {
            Console::error("config::load ", state_string.c_str(), " { not found\n");
            return false;
        }
        const size_t body_end = text.find("}", bracket_open);
        if (body_end == std::string::npos) {
            Console::error("config::load ", state_string.c_str(), " } not found\n");
            return false;
        }

        i = bracket_open + 1;
        while (i < body_end && (text.at(i) == '{' || text.at(i) == ' ' || text.at(i) == '	' || text.at(i) == '\n')) {
            ++i;
        }
        const size_t body_start = i;

        size_t end_line = text.find("\n", i);

        Console::log("text:", text.substr(i, end_line - i).c_str(), "\n");

        size_t var_tag = text.find("scale", i);
        if (var_tag > body_end) {
            var_tag = body_start;
        } else {
            size_t equals = text.find("=", i);
            if (var_tag < end_line) {
                i = equals + 1;
                while (i < end_line && (text.at(i) == '=' || text.at(i) == ' ' || text.at(i) == '	')) {
                    ++i;
                }
                i32c scale = std::stoi(text.substr(i, end_line - i));
                Console::log("Config::load scale: ", scale, "\n");
                if (scale > 0 && scale < 5) {
                    m_scale = (u8)scale;
                }
            }
        }*/



        

        //m_scale = 4;
        //return Config{ .scale = 4 };

        return true;
    }
};