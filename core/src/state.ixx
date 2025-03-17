module;
#include <string_view>
#include <set>

export module state;
import console;
import types;

export namespace state {
    enum class Type {
        null,
        Edit,
        GameInit
    };
    Type type_from(const std::string_view& s) {
        if      (s == "Edit")   return Type::Edit;
        else if (s == "GameInit")   return Type::GameInit;
        return Type::null;
    }
    const char* string_from(Type t) {
        if      (t == Type::Edit)   return "Edit";
        else if (t == Type::GameInit)   return "GameInit";
        return "";
    }    
    class State {
    public:
        Type next    = Type::GameInit,
             current = Type::null;
        
        u8 num_visible_layers = 14;
        std::set<u8> visible_layers;

        i32 current_fps;

        bool is_to_change() {
            return next != current;
        }
        void set_next(Type t) {
            //if (t == next) return;
            //Console::log("state::set_next: ", string_from(t), "\n");
            next = t;
        }
        
        virtual ~State() {}
        virtual void update() {

        }
    };
}