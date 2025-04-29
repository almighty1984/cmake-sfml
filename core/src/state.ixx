module;
#include <filesystem>
#include <string>
#include <set>

export module state;
import console;
import types;

export namespace state {
    enum class Type {
        null,
        Edit,
        Game
    };
    Type type_from(const std::string_view& s) {
        if (s == "Edit")   return Type::Edit;
        else if (s == "Game")   return Type::Game;
        return Type::null;
    }
    const char* to_string(state::Type t) {
        switch (t) {
        case Type::Edit: return "Edit";
        case Type::Game: return "Game";
        default:         return {};
        }
    }
    
    std::filesystem::path tile_set_texture_path(u8c tile_set_number) { return "res/textures/set_" + std::to_string((int)tile_set_number) + ".png"; }

    class State {
    private:
        state::Type m_next    = state::Type::Game,
                    m_current = state::Type::null;

        std::set<u8> m_visible_layers;

    public:
        std::set<u8>& get_visible_layers() { return m_visible_layers; }
        bool add_visible_layer(u8c layer) {
            if (layer > NUM_VISIBLE_LAYERS) return false;
            m_visible_layers.insert(layer);
            return true;
        }
        bool remove_visible_layer(u8c layer) {
            if (layer > NUM_VISIBLE_LAYERS) return false;
            m_visible_layers.erase(layer);
            return true;
        }

        i32 current_fps = 0;
        bool is_drawing_debug_lines = false;

        bool is_to_change() { return m_next != m_current; }
        state::Type get_next()    const { return m_next; }    void set_next(state::Type next) { m_next = next; }
        state::Type get_current() const { return m_current; } void set_current(state::Type current) { m_current = m_next = current; }

        virtual ~State() {}
        virtual void update() {

        }
    };
}