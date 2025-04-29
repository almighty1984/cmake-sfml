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

    Vec2i scroll_limit = { INT_MAX, INT_MAX };

    void add_transform_id(i32c id) {
        m_transform_ids.push_back(id);
    }
    void set_position(Vec2f p) {
        position = p;
        for (auto& i : m_transform_ids) {
            if (transform::Set::at(i)) {
                transform::Set::at(i)->position -= p;
            }
        }
    }
    void update() {
        if (!transform::Set::at(focus_transform)) return;        
        //Console::log("position: ", position.x, " ", position.y, " speed: ", speed.x, " ", speed.y, "\n");

        if (transform::Set::at(focus_transform)->position.x > focal_point.x || transform::Set::at(focus_transform)->position.x < focal_point.x) {            
            f32 diff_x = (transform::Set::at(focus_transform)->position.x - focal_point.x) * speed.x;
            
            // slow down at level edges
            if (diff_x < 0.0f && position.x < diff_x * -10.0f) {
                diff_x = -position.x / 20.0f;
            } else if (diff_x > 0.0f && position.x > scroll_limit.x - diff_x * 10.0f) {
                diff_x = -(position.x - scroll_limit.x) / 20.0f;
            }

            if (diff_x < 0 && position.x <= 0.0f) {
                diff_x = -position.x;
            } else if (diff_x > 0 && position.x >= scroll_limit.x) {
                diff_x = scroll_limit.x - position.x;
            }
            for (auto& i : m_transform_ids) {
                if (transform::Set::at(i)) {
                    transform::Set::at(i)->position.x -= diff_x;                    
                }
            }
            position.x += diff_x;
        }    
        if (transform::Set::at(focus_transform)->position.y > focal_point.y || transform::Set::at(focus_transform)->position.y < focal_point.y) {
            f32 diff_y = (transform::Set::at(focus_transform)->position.y - focal_point.y) * speed.y;

            // slow down at level edges
            if (diff_y < 0.0f && position.y < diff_y * -10.0f) {
                diff_y = -position.y / 20.0f;
            } else if (diff_y > 0.0f && position.y > scroll_limit.y - diff_y * 10.0f) {
                diff_y = -(position.y - scroll_limit.y) / 20.0f;
            }

            if (diff_y < 0 && position.y <= 0.0f) {
                diff_y = -position.y;
            } else if (diff_y > 0 && position.y >= scroll_limit.y) {
                diff_y = scroll_limit.y - position.y;
            }
            for (auto& i : m_transform_ids) {
                if (transform::Set::at(i)) {
                    transform::Set::at(i)->position.y -= diff_y;                    
                }
            }
            position.y += diff_y;
        }
    }
};