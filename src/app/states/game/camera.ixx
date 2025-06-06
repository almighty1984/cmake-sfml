module;
#include <vector>
export module camera;
import console;
import transform;
import types;

export class Camera {
    std::vector<i32> m_transform_ids;

public:
    i32 focus_transform = -1;
    Vec2f focal_point = { 160.0f, 90.0f };
    Vec2f speed = { 0.0625f, 0.0625f };
    Vec2f position;
    Vec2f difference;

    Vec2i scroll_limit = { INT_MAX, INT_MAX };

    void add_transform_id(i32c id) {
        m_transform_ids.push_back(id);
    }
    void set_position(Vec2fc p) {
        position = p;
        if (position.x < 0.0f) {
            position.x = 0.0f;
        } else if (position.x > scroll_limit.x) {
            position.x = scroll_limit.x;
        }
        if (position.y < 0.0f) {
            position.y = 0.0f;
        } else if (position.y > scroll_limit.y) {
            position.y = scroll_limit.y;
        }

        for (auto& i : m_transform_ids) {
            if (transform::Set::at(i)) {
                transform::Set::at(i)->position -= position;
            }
        }
    }
    void update() {
        if (!transform::Set::at(focus_transform)) return;        
        //Console::log("position: ", position.x, " ", position.y, " speed: ", speed.x, " ", speed.y, "\n");

        if (transform::Set::at(focus_transform)->position.x > focal_point.x || transform::Set::at(focus_transform)->position.x < focal_point.x) {            
            difference.x = (transform::Set::at(focus_transform)->position.x - focal_point.x) * speed.x;
            
            // slow down at level edges
            if (difference.x < 0.0f && position.x < difference.x * -10.0f) {
                difference.x = -position.x / 20.0f;
            } else if (difference.x > 0.0f && position.x > scroll_limit.x - difference.x * 10.0f) {
                difference.x = -(position.x - scroll_limit.x) / 20.0f;
            }

            if (difference.x < 0 && position.x <= 0.0f) {
                difference.x = -position.x;
            } else if (difference.x > 0 && position.x >= scroll_limit.x) {
                difference.x = scroll_limit.x - position.x;
            }
            for (auto& i : m_transform_ids) {
                if (transform::Set::at(i)) {
                    transform::Set::at(i)->position.x -= difference.x;
                }
            }
            position.x += difference.x;
        }    
        if (transform::Set::at(focus_transform)->position.y > focal_point.y || transform::Set::at(focus_transform)->position.y < focal_point.y) {
            difference.y = (transform::Set::at(focus_transform)->position.y - focal_point.y) * speed.y;

            // slow down at level edges
            if (difference.y < 0.0f && position.y < difference.y * -10.0f) {
                difference.y = -position.y / 20.0f;
            } else if (difference.y > 0.0f && position.y > scroll_limit.y - difference.y * 10.0f) {
                difference.y = -(position.y - scroll_limit.y) / 20.0f;
            }

            if (difference.y < 0 && position.y <= 0.0f) {
                difference.y = -position.y;
            } else if (difference.y > 0 && position.y >= scroll_limit.y) {
                difference.y = scroll_limit.y - position.y;
            }
            for (auto& i : m_transform_ids) {
                if (transform::Set::at(i)) {
                    transform::Set::at(i)->position.y -= difference.y;
                }
            }
            position.y += difference.y;
        }
    }
};