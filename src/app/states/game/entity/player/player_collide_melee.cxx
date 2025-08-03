module;
#include <cmath>
module player;
import console;

void player::Object::collide_melee_x(aabb::Infoc our, aabb::Infoc other) {    

    entity::Typec other_type = other.owner->type();

    Rectfc other_rect = { aabb::Set::at(other.id)->points().at(0).x, aabb::Set::at(other.id)->points().at(0).y,
                          aabb::Set::at(other.id)->points().at(3).x, aabb::Set::at(other.id)->points().at(3).y };

    if (other_type == entity::Type::bug) {
        /*if (sound::Set::at(m_melee_hit_sound_id)) {
            sound::Set::at(m_melee_hit_sound_id)->position({ position().x / 160.0f, position().y / 90.0f });
            sound::Set::at(m_melee_hit_sound_id)->play();
        }*/
    } else if (other_type == entity::Type::frog) {
        //other.owner->is_struck(true);
        //aabb::Set::at(our.id)->is_inactive(true);

        //melee_hit();

        //aabb::Set::at(our.id)->is_inactive(true);
    } else if (other_type == entity::Type::swing) {
        if (!is_pressed(key_melee) || m_state != entity::State::run || m_time_left_in_state > 0) {
            return;
        }
        //next_state(entity::State::swing);
        m_prev_state = m_state;
        m_next_state = entity::State::swing;

        Vec2fc rect_size = { other_rect.w - other_rect.x, other_rect.h - other_rect.y };

        m_swing_level_offset = Vec2f{ other_rect.x, other_rect.y } - transform::Set::at(level_transform_id)->position - rect_size / 2;

        //Console::log("rect size: ", rect_size.x, " ", rect_size.y, "\n");

        Vec2fc pos_diff = transform()->position - Vec2f{ other_rect.x + rect_size.x, other_rect.y + rect_size.y };

        f32 time_left_in_state = std::abs(pos_diff.y / pos_diff.x) * 2.0f;

            
        if (time_left_in_state < 0.0f) time_left_in_state = 0.0f;
        if (time_left_in_state > 2.0f) time_left_in_state = 2.0f;


        m_time_left_in_state = (int)time_left_in_state;

        //Console::log("\nplayer::Object::collide_melee_x time_left_in_state: ", time_left_in_state, " ", (int)m_time_left_in_state, "\n");
        m_rotation_speed = -transform()->velocity.x * transform()->velocity.y / 2.0f;
    }
        
}

void player::Object::collide_melee_y(aabb::Infoc our, aabb::Infoc other) {
    collide_melee_x(our, other);
}