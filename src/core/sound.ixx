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

class Object {
    std::unique_ptr<sf::Sound> m_sound;
    std::filesystem::path m_path;
    Vec2F m_position;
public:
    I32 id = -1;
    //Sound() {}
    Object(const std::filesystem::path& path) {
        load(path);
    }
    void load(const std::filesystem::path& path) {
        m_path = path;
        m_sound = std::make_unique<sf::Sound>(load_sound_buffer(path));        
    }
    ~Object() {
        m_sound.release();
    }
    cVec2F position() const { return m_position; }
    void position(cVec2F p) {
        if (!m_sound) return;
        m_position = p;
        m_sound->setPosition(sf::Vector3f{ p.x, p.y, 1.0F });
        
        //m_sound->setMinDistance(1.0F);
        //m_sound->setRelativeToListener(true);
    }
    cF32 offset() const { return m_sound->getPlayingOffset().asSeconds(); }
    bool is_looped() const { return m_sound->isLooping(); } void is_looped(const bool b) { m_sound->setLooping(b); }

    void play()  { if (m_sound) m_sound->play();  } bool is_playing() const { return m_sound->getStatus() == sf::SoundSource::Status::Playing; }
    void stop()  { if (m_sound) m_sound->stop();  } bool is_stopped() const { return m_sound->getStatus() == sf::SoundSource::Status::Stopped; }
    void pause() { if (m_sound) m_sound->pause(); } bool is_paused()  const { return m_sound->getStatus() == sf::SoundSource::Status::Paused; }
    
    cF32 pitch() const { return m_sound ? m_sound->getPitch() : 0.0F; } void pitch(cF32 p) { if (m_sound) m_sound->setPitch(p); }
};

export namespace sound {
    class Manager {
        //static inline std::vector<std::unique_ptr<Sound>> s_sounds;
        static inline std::vector<Object*> s_objects;
        static inline std::vector<I32>     s_unused_ids;
    public:
        //static std::unique_ptr<Sound>&  at(size_t i) { return s_sounds.at(i); }
        static Object*  get(size_t i) { return s_objects.at(i); }
        static bool     is_valid(size_t i) { return (i >= 0 && i < s_objects.size() && s_objects.at(i)) ? true : false; }
        static size_t   size() { return s_objects.size(); }

        static cI32 make(const std::filesystem::path& path) {
            //s_sounds.emplace_back(std::make_unique<Sound>(path));
            //return s_sounds.size() - 1;

            Object* object = new Object(path);
            if (!s_unused_ids.empty()) {
                object->id = s_unused_ids.back();
                s_unused_ids.pop_back();
                if (!s_objects.empty() && object->id >= 0 && object->id < s_objects.size() && s_objects.at(object->id)) {
                    delete s_objects.at(object->id);
                    s_objects.at(object->id) = nullptr;
                }
            } else {
                object->id = s_objects.size();
                s_objects.emplace_back(nullptr);
            }
            s_objects.at(object->id) = object;
            return object->id;
        }
        static bool erase(cI32 i) {
            if (i < 0) return false;
            if (s_objects.empty() || i > s_objects.size() - 1 || !s_objects.at(i)) {
                Console::log("sound::Manager::erase ", i, " can't do it!\n");
                return false;
            }            
            //Console::log("sound::Manager::erase ", i, "\n");
            
            if (s_objects.at(i)->is_playing()) {
                s_objects.at(i)->stop();
            }

            delete s_objects.at(i);
            s_objects.at(i) = nullptr;
            s_unused_ids.emplace_back(i);
            return true;
        }
        static void clear() {
            for (I32 i = 0; i < s_objects.size(); ++i) {
                //erase(i);
                if (s_objects.at(i)) {
                    erase(i);
                }
            }
            s_objects.clear();
            s_unused_ids.clear();
        }
    };
}