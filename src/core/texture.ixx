module;
#include <SFML/Graphics.hpp>
#include <map>
#include <filesystem>

export module texture;
import console;

export namespace texture {
    class Manager {
    private:
        static inline std::map<std::filesystem::path, sf::Texture*> s_textures;
    public:
        ~Manager() {
            unload_all();
        }
        static sf::Texture* load(const std::filesystem::path& path) {
            if (path.empty()) {
                return nullptr;
            }
            auto it = s_textures.find(path);
            sf::Texture* temp_texture;
            if (it == s_textures.end()) {
                temp_texture = new sf::Texture;
                if (!temp_texture->loadFromFile(path)) {
                    Console::error("texture::Manager::load: ", path, " not found\n");
                    delete temp_texture;
                    return nullptr;
                }
                temp_texture->setSmooth(false);
                s_textures.emplace(path, temp_texture);
                return s_textures.at(path);
            }
            return it->second;
        }
        static void unload_all() {
            for (auto& i : s_textures) {
                if (!i.second) continue;
                delete i.second;
                i.second = nullptr;
            }
            s_textures.clear();
        }
    };
}