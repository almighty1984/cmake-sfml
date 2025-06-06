module;
#include <cmath>
module player;
import console;

void player::Object::collide_melee_x(sprite::Infoc our, sprite::Infoc other) {
    if (!transform()) return;
    //if (std::find(m_melee_collider_ids.begin(), m_melee_collider_ids.end(), other.collider_id) != m_melee_collider_ids.end()) {
    if (other.collider_id == m_melee_left_collider_id || other.collider_id == m_melee_right_collider_id) {
        // collide melee
        if (other.type == sprite::Type::swing) {
            if (!is_pressed(key_melee) || m_state != player::State::run || m_time_left_in_state > 0) {
                return;
            }
            m_prev_state = m_state;
            m_next_state = player::State::swing;

            Vec2fc rect_size = { other.rect.w - other.rect.x, other.rect.h - other.rect.y };

            m_swing_level_offset = Vec2f{ other.rect.x, other.rect.y } - transform::Set::at(level_transform_id)->position - rect_size / 2;

            //Console::log("rect size: ", rect_size.x, " ", rect_size.y, "\n");

            Vec2fc pos_diff = transform()->position - Vec2f{ other.rect.x + rect_size.x, other.rect.y + rect_size.y };

            f32 time_left_in_state = std::abs(pos_diff.y / pos_diff.x) * 2.0f;

            
            if (time_left_in_state < 0.0f) time_left_in_state = 0.0f;
            if (time_left_in_state > 2.0f) time_left_in_state = 2.0f;


            m_time_left_in_state = (int)time_left_in_state;

            Console::log("\nplayer::Object::collide_melee_x time_left_in_state: ", time_left_in_state, " ", (int)m_time_left_in_state, "\n");
            m_rotation_speed = -transform()->velocity.x * transform()->velocity.y / 2.0f;
        }
        return;
    }
}

void player::Object::collide_melee_y(sprite::Infoc our, sprite::Infoc other) {
    collide_melee_x(our, other);
}