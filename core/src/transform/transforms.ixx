module;
#include <vector>
//#include <memory>
//#include <sstream>

export module transforms;

export import transform;
import console;
import types;

export class Transforms {
    static inline std::vector<Transform*> m_transforms;
    static inline std::vector<size_t>     m_unused_ids;

public:
    static Transform* at(size_t i) { return (i >= 0 && i < m_transforms.size()) ? m_transforms.at(i) : nullptr; }
    static bool   is_valid(size_t i) { return (i >= 0 && i < m_transforms.size() && m_transforms.at(i)) ? true : false; }
    static size_t size() { return m_transforms.size(); }
    
    static size_t make() {
        Transform* transform = new Transform;

        if (!m_unused_ids.empty()) {
            transform->id = m_unused_ids.back();
            m_unused_ids.pop_back();
        } else {
            transform->id = m_transforms.size();
            //m_transforms.push_back(std::make_unique<Transform>());
            m_transforms.push_back(nullptr);
        }

        if (!m_transforms.empty() && transform->id >= 0 && transform->id < m_transforms.size() && m_transforms.at(transform->id)) {            
            delete m_transforms.at(transform->id);
        }
        m_transforms.at(transform->id) = transform;        

        return transform->id;
        //m_transforms.at(transform->id) = std::move(transform);
        //return m_transforms.at(transform->id).get()->id;
    }
    static void erase(size_t i) {
        if (m_transforms.empty() || i < 0 || i > m_transforms.size() - 1 || !m_transforms.at(i)) {
            Console::log("Transforms::erase ", i, "can't do it\n");
            return;
        }
        Console::log("Transforms::erase ", i, "\n");
        delete m_transforms.at(i);
        m_transforms.at(i) = nullptr;
        //m_transforms.at(i).reset();
        m_unused_ids.push_back(i);
    }    
    static void update() {
        for (auto& i : m_transforms) {
            if (!i) continue;
            i->update();
        }
    }
    static void clear() {
        for (size_t i = 0; i < m_transforms.size(); ++i) {
            Console::log("Transforms::clear erasing ", i, "\n");
            erase(i);
        }
        m_transforms.clear();
        m_unused_ids.clear();
    }    
};
