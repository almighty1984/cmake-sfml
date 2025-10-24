module;
#include <string>
#include <sstream>
#include <iostream>

export module console;

//template<typename T>
//concept StringLike = std::is_convertible_v<T, std::string_view>;

export class Console {
    static inline bool        s_is_hidden = false;
    static inline std::string s_log, s_prev_text, s_prev_command;
public:
    static void log() {}
    template <typename First, typename... Rest> //requires StringLike<First> && (StringLike<Rest> && ...)    
    static void log(First first, Rest... rest) {
        std::ostringstream oss;
        oss << first;
        const std::string text = oss.str().c_str();    // c_str or prints address for other than int
        std::cout << oss.str().c_str();
        if (text.back() != '\n') {
            s_prev_text += text;
        } else {
            s_log.append(s_prev_text + text);
            s_prev_text.clear();
            /*++s_current_line_num;
            if (s_current_line_num > 14) {
                s_current_line_num = 0;
            }*/
        }
        log(rest...);
    }
    static void warning() {}
    template <typename First, typename... Rest>
    static void warning(First first, Rest... rest) {
        log(first, rest...);        
    }
    static void error() {}
    template <typename First, typename... Rest>
    static void error(First first, Rest... rest) {
        log(first, rest...);        
    }
};