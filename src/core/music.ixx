module;
#include <SFML/Audio.hpp>
#include <filesystem>

export module music;
import console;
import types;

class Object {
    sf::Music m_sf_music;

public:
    Object(const std::filesystem::path& path) { open(path); }
    bool open(const std::filesystem::path& path) { return m_sf_music.openFromFile(path); }
    bool is_looped() const { return m_sf_music.isLooping(); } void is_looped(bool is) { m_sf_music.setLooping(is); }
    bool is_playing() const { return m_sf_music.getStatus() == sf::Music::Status::Playing; }
    cF32 volume()    const { return m_sf_music.getVolume(); } void volume(cF32 v) { m_sf_music.setVolume(v); }
    void play()  { m_sf_music.play();  }
    void stop()  { m_sf_music.stop();  }
    void pause() { m_sf_music.pause(); }
};

export namespace music {
    class Manager {
        static inline std::vector<std::unique_ptr<Object>> s_objects;
    public:
        static std::unique_ptr<Object>& get(size_t i) { return s_objects.at(i); }
        static bool    is_valid(size_t i) { return (i >= 0 && i < s_objects.size() && s_objects.at(i)) ? true : false; }
        static size_t  size() { return s_objects.size(); }

        static cI32 make(const std::filesystem::path& path) {
            s_objects.emplace_back(std::make_unique<Object>(path));
            return s_objects.size() - 1;
        }

        static bool erase(cI32 i) {
            if (i < 0) return false;
            Console::log("music::Set::erase ", i, "\n");
            if (s_objects.empty() || i > s_objects.size() - 1 || !s_objects.at(i)) {
                return false;
            }
            if (s_objects.at(i) && s_objects.at(i)->is_playing()) {
                s_objects.at(i)->stop();
            }
            s_objects.at(i).release();
            return true;
        }
        static void clear() {  
            for (auto& i : s_objects) {
                if (i && i->is_playing()) {
                    i->stop();
                }
                i.release();
            }
            s_objects.clear();
        }
    };
}