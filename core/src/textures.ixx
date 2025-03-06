module;
#include <SFML/Graphics.hpp>
#include <map>
#include <filesystem>

export module textures;

export class Textures {
private:
    static inline std::map<std::filesystem::path, sf::Texture*> m_textures;
public:
    ~Textures() {
        unload_all();
    }
    static sf::Texture* load(const std::filesystem::path path) {
        if (path.empty()) {
            return nullptr;
        }
        auto it = m_textures.find(path.string());
        sf::Texture* temp_texture;
        if (it == m_textures.end()) {
            temp_texture = new sf::Texture;
            if (!temp_texture->loadFromFile(path)) {
                //Console::error("Error: " + path + " not found\n");
            }
            temp_texture->setSmooth(false);
            m_textures[path] = temp_texture;
            return temp_texture;
        }
        return it->second;
    }
    static void unload_all() {
        for (auto& i : m_textures) {
            if (!i.second) continue;
            delete i.second;
            i.second = nullptr;            
        }
        m_textures.clear();
    }
};