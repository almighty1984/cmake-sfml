module;
#include <set>
export module camera;
import console;
import transform;
import types;

export class Camera {
    static inline std::set<I32> m_transform_ids;

public:
    static inline I32 focus_transform = -1;
    static inline Vec2F focal_point   = { 160.0F, 90.0F };
    static inline Vec2F speed         = { 0.0625f, 0.0625f };
    static inline Vec2F position      = { 0.0F, 0.0F };
    static inline Vec2F difference    = { 0.0F, 0.0F };

    static inline Vec2I scroll_limit = { INT_MAX, INT_MAX };

    static void add_transform_id(cI32 id) {
        //Console::log("Camera::add_transform_id: ", id, "\n");
        m_transform_ids.insert(id);
    }
    static void remove_transform_id(cI32 id) {
        m_transform_ids.erase(id);
    }
    static void clear() {
        focus_transform = -1;
        m_transform_ids.clear();
        position   = { 0.0F, 0.0F };
        difference = { 0.0F, 0.0F };
        scroll_limit = { INT_MAX, INT_MAX };
    }
    static void set_position(cVec2F p) {
        position = p;
        if (position.x < 0.0F) {
            position.x = 0.0F;
        } else if (position.x > scroll_limit.x) {
            position.x = scroll_limit.x;
        }
        if (position.y < 0.0F) {
            position.y = 0.0F;
        } else if (position.y > scroll_limit.y) {
            position.y = scroll_limit.y;
        }

        for (auto& i : m_transform_ids) {
            if (transform::Manager::get(i)) {
                transform::Manager::get(i)->position -= position;
            }
        }
    }
    static void update() {
        if (focus_transform == -1 || !transform::Manager::get(focus_transform)) return;
        //Console::log(focus_transform, " position: ", position.x, " ", position.y, "\n");
        
        //Console::log("focus transform position: ", transform::Manager::get(focus_transform)->position.x, " ", transform::Manager::get(focus_transform)->position.y, "\n");

        difference = { 0.0F, 0.0F };
        if (transform::Manager::get(focus_transform)->position.x > focal_point.x || transform::Manager::get(focus_transform)->position.x < focal_point.x) {            
            difference.x = (transform::Manager::get(focus_transform)->position.x - focal_point.x) * speed.x;
            
            if (difference.x < 0 && position.x <= 0.0F) {
                difference.x = -position.x;
            } else if (difference.x > 0 && position.x >= scroll_limit.x) {
                difference.x = scroll_limit.x - position.x;
            } else {
                // slow down at level edges
                if (difference.x < 0.0F && position.x < difference.x * -10.0F) {
                    difference.x = -position.x / 20.0F;
                } else if (difference.x > 0.0F && position.x > scroll_limit.x - difference.x * 10.0F) {
                    difference.x = -(position.x - scroll_limit.x) / 20.0F;
                }
            }
            for (auto& i : m_transform_ids) {
                if (transform::Manager::get(i)) {
                    transform::Manager::get(i)->position.x -= difference.x;
                }
            }
            position.x += difference.x;
        }    
        if (transform::Manager::get(focus_transform)->position.y > focal_point.y || transform::Manager::get(focus_transform)->position.y < focal_point.y) {
            difference.y = (transform::Manager::get(focus_transform)->position.y - focal_point.y) * speed.y;

            if (difference.y < 0.0F && position.y <= 0.0F) {
                difference.y = -position.y;
            } else if (difference.y > 0.0F && position.y >= scroll_limit.y) {
                difference.y = scroll_limit.y - position.y;
            } else {
                // slow down at level edges
                if (difference.y < 0.0F && position.y < difference.y * -10.0F) {
                    difference.y = -position.y / 20.0F;
                } else if (difference.y > 0.0F && position.y > scroll_limit.y - difference.y * 10.0F) {
                    difference.y = -(position.y - scroll_limit.y) / 20.0F;
                }
            }
            for (auto& i : m_transform_ids) {
                if (transform::Manager::get(i)) {
                    transform::Manager::get(i)->position.y -= difference.y;
                }
            }
            position.y += difference.y;
        }
    }
};