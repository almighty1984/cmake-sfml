module;
#include <SFML/Audio.hpp>
#include <filesystem>
#include <unordered_map>
#include <vector>
#include <memory>

export module sound;
import console;
import types;

std::unordered_map<std::filesystem::path, std::unique_ptr<sf::SoundBuffer>> sound_buffers;
static sf::SoundBuffer& load_sound_buffer(const std::filesystem::path& path) {
    if (sound_buffers.find(path) == sound_buffers.end()) {
        sound_buffers[path] = std::make_unique<sf::SoundBuffer>(path);
    }
    return *sound_buffers[path];
}

class Sound {
    std::unique_ptr<sf::Sound> m_sound;
    std::filesystem::path m_path;
    Vec2f m_position;
public:
    i32 id = -1;
    //Sound() {}
    Sound(const std::filesystem::path& path) {
        load(path);
    }
    void load(const std::filesystem::path& path) {
        m_path = path;
        m_sound = std::make_unique<sf::Sound>(load_sound_buffer(path));        
    }
    ~Sound() {
        m_sound.release();
    }
    Vec2fc position() const { return m_position; }
    void position(Vec2fc p) {
        if (!m_sound) return;
        m_position = p;
        m_sound->setPosition(sf::Vector3f{ p.x, p.y, 1.0f });
        
        //m_sound->setMinDistance(1.0f);
        //m_sound->setRelativeToListener(true);
    }
    f32c offset() const { return m_sound->getPlayingOffset().asSeconds(); }
    bool is_looped() const { return m_sound->isLooping(); } void is_looped(const bool b) { m_sound->setLooping(b); }

    void play()  { if (m_sound) m_sound->play();  } bool is_playing() const { return m_sound->getStatus() == sf::SoundSource::Status::Playing; }
    void stop()  { if (m_sound) m_sound->stop();  } bool is_stopped() const { return m_sound->getStatus() == sf::SoundSource::Status::Stopped; }
    void pause() { if (m_sound) m_sound->pause(); } bool is_paused()  const { return m_sound->getStatus() == sf::SoundSource::Status::Paused; }
    
    f32c pitch() const { return m_sound ? m_sound->getPitch() : 0.0f; } void pitch(f32c p) { if (m_sound) m_sound->setPitch(p); }
};

export namespace sound {
    class Set {
        //static inline std::vector<std::unique_ptr<Sound>> s_sounds;
        static inline std::vector<Sound*> s_sounds;
        static inline std::vector<i32>    s_unused_ids;
    public:
        //static std::unique_ptr<Sound>&  at(size_t i) { return s_sounds.at(i); }
        static Sound*  at(size_t i) { return s_sounds.at(i); }
        static bool    is_valid(size_t i) { return (i >= 0 && i < s_sounds.size() && s_sounds.at(i)) ? true : false; }
        static size_t  size() { return s_sounds.size(); }

        static i32c make(const std::filesystem::path& path) {
            //s_sounds.push_back(std::make_unique<Sound>(path));
            //return s_sounds.size() - 1;

            Sound* sound = new Sound(path);
            if (!s_unused_ids.empty()) {
                sound->id = s_unused_ids.back();
                s_unused_ids.pop_back();
                if (!s_sounds.empty() && sound->id >= 0 && sound->id < s_sounds.size() && s_sounds.at(sound->id)) {
                    delete s_sounds.at(sound->id);
                    s_sounds.at(sound->id) = nullptr;
                }
            } else {
                sound->id = s_sounds.size();
                s_sounds.push_back(nullptr);
            }
            s_sounds.at(sound->id) = sound;
            return sound->id;
        }
        static bool erase(size_t i) {
            if (!(i < s_sounds.size()) || !s_sounds.at(i)) {
                Console::log("sound::Set::erase ", i, " can't do it\n");
                return false;
            }
            Console::log("sound::Set::erase ", i, "\n");
            s_sounds.at(i)->stop();
            delete s_sounds.at(i);
            s_sounds.at(i) = nullptr;
            s_unused_ids.push_back(i);
            return true;
        }
        static void clear() {
            for (i32 i = 0; i < s_sounds.size(); ++i) {
                erase(i);
            }
            s_sounds.clear();
            s_unused_ids.clear();
        }
    };
}