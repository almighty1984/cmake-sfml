module;
#include <vector>

export module lines;

import console;
import line;
import types;
import window;

export class Lines {    
    static inline std::vector<Line*>   m_lines;
    static inline std::vector<size_t>  m_unused_ids;
public:    
    static bool   is_valid(size_t i) { return (i >= 0 && i < m_lines.size() && m_lines.at(i)) ? true : false; }
    static size_t size() { return m_lines.size(); }    
    static Line*  at(size_t i) { return (i >= 0 && i <= m_lines.size() - 1) ? m_lines.at(i) : nullptr; }
    static size_t make(Vec2fc start, Vec2fc end) {
        Line* line = new Line(start, end);
        
        if (!m_unused_ids.empty()) {
            line->id = m_unused_ids.back();
            m_unused_ids.pop_back();
        } else {
            line->id = m_lines.size();
            m_lines.push_back(nullptr);
        }
        if (!m_lines.empty() && line->id >= 0 && line->id < m_lines.size() && m_lines.at(line->id)) {
            delete m_lines.at(line->id);
        }
        m_lines.at(line->id) = line;
        return line->id;
    }    
    static void erase(size_t i) {
        if (i < 0 || i > m_lines.size() - 1 || !m_lines.at(i)) {
            Console::log("Lines::erase ", i, "can't do it\n");
            return;
        }
        Console::log("Lines::erase ", i, "\n");
        delete m_lines.at(i);
        m_lines.at(i) = nullptr;
        m_unused_ids.push_back(i);
    }
    static void draw(u8c layer, Window& window) {
        for (auto& i : m_lines) {
            if (!i || i->id == -1 || i->layer != layer) continue;
            i->update();
            window.draw(i);
        }
    }
    static void clear() {
        for (size_t i = 0; i < m_lines.size(); ++i) {
            Console::log("Lines::clear erasing ", i, "\n");
            erase(i);
        }
        m_unused_ids.clear();
    }
};