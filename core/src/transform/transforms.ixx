module;
#include <vector>

export module transforms;

export import transform;
import console;
import types;

export class Transforms {
    static inline size_t                  m_num_transform_ids = 0;
    static inline std::vector<Transform*> m_transforms;
    static inline std::vector<size_t>     m_unused_ids;

public:
    static Transform* at(size_t i) { return (i >= 0 && i <= m_transforms.size() - 1) ? m_transforms.at(i) : nullptr; }
    static bool   is_valid(size_t id) { return (id >= 0 && id <= m_num_transform_ids - 1) ? true : false; }
    static size_t size() { return m_transforms.size(); }
    static size_t num_transform_ids() { return m_num_transform_ids; }
    static size_t add(Transform* transform) {
        if (!transform) return -1;
        Console::log("m_num_transform_ids: ", m_num_transform_ids, "\n");
        if (!m_unused_ids.empty()) {
            transform->id = m_unused_ids.back();
            m_unused_ids.pop_back();
        } else {
            transform->id = m_num_transform_ids;
            ++m_num_transform_ids;
            m_transforms.push_back(nullptr);
        }

        if (!m_transforms.empty() && transform->id >= 0 && transform->id <= m_transforms.size() - 1 && m_transforms.at(transform->id)) {
            Console::log("Sprites::add erase ", transform->id, "\n");
            delete m_transforms.at(transform->id);
            m_transforms.at(transform->id) = nullptr;
        } else {
            m_transforms.at(transform->id) = transform;
        }
        m_transforms.at(transform->id) = transform;
        return transform->id;
    }    
    static void erase(size_t i) {
        if (i < 0 || i > m_transforms.size() - 1 || !m_transforms.at(i)) {
            Console::log("Transforms::erase ", i, "can't do it\n");
            return;
        }
        Console::log("Transforms::erase ", i, "\n");
        delete m_transforms.at(i);
        m_transforms.at(i) = nullptr;
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
        m_num_transform_ids = 0;
    }    
};
