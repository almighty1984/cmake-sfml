module;
#include <fstream>
#include <filesystem>
#include <string>
#include <set>

export module state;
import console;
import types;

export namespace state {
    enum class Type {
        null,
        edit,
        game
    };
    Type type_from(const std::string_view& s) {
        if      (s == "edit")   return Type::edit;
        else if (s == "game")   return Type::game;
        return Type::null;
    }
    const char* to_string(state::Type t) {
        switch (t) {
        case Type::edit: return "edit"; break;
        case Type::game: return "game"; break;
        default:         return {};     break; }
    }
    
    std::filesystem::path tile_set_texture_path(u8c tile_set_number) { return "res/texture/set_" + std::to_string((int)tile_set_number) + ".png"; }

    class State {
    private:
        state::Type m_next_state    = state::Type::game,
                    m_current_state = state::Type::null;

        std::set<u8> m_visible_layers;

        start::Info m_start_info;

        Rectf m_view = { 0.0f, 0.0f, 320.0f, 180.0f };

        start::Type m_next_start = start::Type::center;

        bool m_is_drawing_debug_lines = false;
        bool m_is_to_write_save = false;
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

        bool is_drawing_debug_lines() const { return m_is_drawing_debug_lines; } void is_drawing_debug_lines(bool is) { m_is_drawing_debug_lines = is; }
        bool is_to_write_save()       const { return m_is_to_write_save; }       void is_to_write_save(bool is) { m_is_to_write_save = is; }
        bool is_to_change()           const { return m_next_state != m_current_state; }
        
        state::Type next_state()    const { return m_next_state;    } void next_state(state::Type t)    { m_next_state = t;                   }
        state::Type current_state() const { return m_current_state; } void current_state(state::Type t) { m_current_state = m_next_state = t; }


        Rectfc view() const { return m_view; } void view(Rectfc v) { m_view = v; }

        start::Type next_start() const { return m_next_start; } void next_start(start::Type s) { m_next_start = s; }

        start::Info start_info() { return m_start_info; }   void start_info(start::Info i) { m_start_info = i; }

        //entity::Type next_level_start() const { return m_next_level_start; }  void next_level_start(entity::Type t) { m_next_level_start = t; }
        virtual std::filesystem::path level_path_to_save() { return {}; }

        virtual ~State() {}
        virtual void update() {}
        virtual void update_unlocked() {}
    };
}