module;
#include <sstream>
module player;
import console;

void player::Object::collide_interact_x(aabb::Infoc our, aabb::Infoc other) {    
    if (other.owner->is_interacting()) {
        return;
    }

    entity::Typec other_type = other.owner->type();
    
    if (other_type == entity::Type::player || m_time_left_interacting == 1) return;

    if (other_type == entity::Type::trigger) {
        if (!m_is_on_ground) return;

        m_time_left_interacting = 1;
        m_time_left_lever = m_time_to_lever;
        reset_anim(m_lever_anim_id);
        sprite()->is_leftward = other.owner->is_dead();

        sound::Set::at(m_lever_sound_id)->position({ position().x / 160.0f, position().y / 90.0f });
        sound::Set::at(m_lever_sound_id)->play();
    } else if (other_type == entity::Type::bug) {        
        if (other.owner->parent()) {            
            //m_time_left_interacting = 1;
            other.owner->parent(nullptr);
            other.owner->transform()->velocity = (transform()->velocity + transform()->moved_velocity);
            //other.owner->transform()->position.x += other.owner->transform()->velocity.x;
            if (sprite()->is_leftward) {
                other.owner->transform()->velocity.x -= 2.0f;
                other.owner->transform()->position.x -= 8.0f;
            } else {
                other.owner->transform()->velocity.x += 2.0f;
                other.owner->transform()->position.x += 8.0f;
            }
            other.owner->transform()->velocity.y -= 1.5f;
            other.owner->next_state(entity::State::tossed);
            other.owner->time_left_interacting(10.0f);

            sound::Set::at(m_toss_sound_id)->position({ position().x / 160.0f, position().y / 90.0f });
            sound::Set::at(m_toss_sound_id)->play();

            m_is_carrying = false;            
        } else {
            if (!m_is_carrying) {
                //m_is_carrying = true;
                sound::Set::at(m_pick_up_sound_id)->position({ position().x / 160.0f, position().y / 90.0f });
                sound::Set::at(m_pick_up_sound_id)->play();
            }
        }
        //Console::log("collide_interact_x ", entity::type_to_string(other_type), "\n");
    }
}

void player::Object::collide_interact_y(aabb::Infoc our, aabb::Infoc other) {
    collide_interact_x(our, other);
}