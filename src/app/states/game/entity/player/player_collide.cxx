module;
#include <sstream>
module player;
import console;

void player::Object::collide_x(aabb::Infoc our, aabb::Infoc other) {
    if (!transform() || !our.owner || !other.owner || !other.owner->transform() || !aabb::Set::at(our.id) || !aabb::Set::at(other.id)) return;
    if (aabb::Set::at(our.id)->is_inactive() || aabb::Set::at(other.id)->is_inactive()) return;
    if (aabb::Set::at(our.id)->points().size() < 4 || aabb::Set::at(other.id)->points().size() < 4) return;

    aabb::Name our_aabb_name = aabb::Set::at(our.id)->name;

    if (our_aabb_name == aabb::Name::interact) {
        collide_interact_x(our, other);
        return;
    } else if (our_aabb_name == aabb::Name::melee_L || our_aabb_name == aabb::Name::melee_R) {
        collide_melee_x(our, other);
        return;
    }

    Rectfc our_rect = { aabb::Set::at(our.id)->points().at(0).x, aabb::Set::at(our.id)->points().at(0).y,
                        aabb::Set::at(our.id)->points().at(3).x, aabb::Set::at(our.id)->points().at(3).y };
    Rectfc other_rect = { aabb::Set::at(other.id)->points().at(0).x, aabb::Set::at(other.id)->points().at(0).y,
                        aabb::Set::at(other.id)->points().at(3).x, aabb::Set::at(other.id)->points().at(3).y };

    if (other_rect.x < our_rect.x && transform()->velocity.x > 0.0f ||
        other_rect.x > our_rect.x && transform()->velocity.x < 0.0f) return;

    entity::Typec other_type = other.owner->type();
    aabb::Name other_aabb_name = aabb::Set::at(other.id)->name;

    entity::Statec other_state = other.owner->state();

    Vec2fc other_velocity = other.owner->transform()->velocity;

    f32c overlap_x = our_rect.x < other_rect.x ? our_rect.w - other_rect.x : -(other_rect.w - our_rect.x);

    //Console::log(sprite::type_to_string(other_type), " overlap_x: ", overlap_x, "\n");
    
    if (other_type == entity::Type::arch_L_1x1 || other_type == entity::Type::arch_R_1x1 ||
        other_type == entity::Type::arch_L_2x1_0 || other_type == entity::Type::arch_L_2x1_1 ||
        other_type == entity::Type::arch_R_2x1_0 || other_type == entity::Type::arch_R_2x1_1) {
        collide_y(our, other);
    } else if (other_type == entity::Type::bug) {
        if (other.owner->parent() == this || other.owner->is_interacting()) return;

        if (other_state == entity::State::upended) {
            if (transform()->velocity.x < -transform()->velocity_limit.x * 0.75f) {
                transform()->velocity.x = -transform()->velocity_limit.x * 0.75f;
            } else if (transform()->velocity.x > transform()->velocity_limit.x * 0.75f) {
                transform()->velocity.x = transform()->velocity_limit.x * 0.75f;
            }
            other.owner->transform()->velocity.x = transform()->velocity.x;
            //other.owner->transform()->deceleration = 0.0f;
            //other.owner->transform()->position.x -= overlap_x;
            return;
        }
        transform()->position.x -= overlap_x;

        transform()->velocity.x = other_velocity.x;

        if (our_rect.x < other_rect.x) {
            transform()->velocity.x -= 1.0f;
            //transform()->position.x -= 2.0f;
        } else if (our_rect.w > other_rect.w) {
            transform()->velocity.x += 1.0f;
            //transform()->position.x += 2.0f;
        }

        transform()->velocity.y = -1.0f;
    } else if (other_type == entity::Type::clip) {
        transform()->position.x -= overlap_x;
        transform()->velocity.x = transform()->moved_velocity.x = 0.0f;
    } else if (other_type == entity::Type::clip_duck) {
        if ((other_rect.y < our_rect.y && m_is_on_ground) || m_is_climbing_ledge) {            
            m_time_left_rising = m_time_to_rise;
            m_time_left_ducking = m_time_to_duck;
            return;
        }
        /*if (transform()->velocity.x < 0.0f && our_rect.w < other_rect.x ||
            transform()->velocity.x > 0.0f && our_rect.x > other_rect.w) {
            return;
        }*/
        if (m_time_left_rising > 0) {
            return;
        }
        m_is_wall_to_left = other_rect.x < our_rect.x;
        if ((m_is_wall_to_left && transform()->velocity.x + transform()->moved_velocity.x > 0.0f) ||
            (!m_is_wall_to_left && transform()->velocity.x + transform()->moved_velocity.x < 0.0f)) return;

        if (m_time_left_jumping_wall == 0 && !m_is_carrying && !m_is_hovering &&
            transform()->velocity.y > 0.0f &&
            (m_is_wall_to_left && is_pressed(key_left) || !m_is_wall_to_left && is_pressed(key_right))) {
            m_is_sliding_wall = true;
            if (transform()->velocity.y > 1.0f) {
                transform()->velocity.y = 1.0f;
            }
            sprite()->is_leftward = !m_is_wall_to_left;
        }
        if (is_pressed(key_jump) && !m_is_carrying && !m_is_hovering && !m_is_locked_jump && m_time_left_jumping_wall <= 0.0f) {
            m_time_left_jumping_wall = m_time_to_jump_wall;
            m_num_jumps = 0;
        }

        if (transform()->velocity.y > 0.0f) {
            transform()->position.x -= overlap_x;
            transform()->velocity.x = 0.0f;
            transform()->moved_velocity.x = 0.0f;

            if (m_is_on_ground) {
                m_time_left_rising = m_time_to_rise;
                m_time_left_ducking = m_time_to_duck;
                m_is_ducking = true;
                //press(key_down);
                //collider::aabb::Set::at(aabb_ids().back())->set_rect(m_transform_id, { 0, 16, 16, 8 });
            }
        }
    } else if (other_type == entity::Type::clip_U || other_type == entity::Type::slope_U) {
    } else if (other_type == entity::Type::clip_D) {
    } else if (other_type == entity::Type::clip_ledge) {
        if (m_is_on_ledge) return;

        /*if (transform()->velocity.x < 0.0f && our_rect.w < other_rect.x ||
            transform()->velocity.x > 0.0f && our_rect.x > other_rect.w) {
            return;
        }*/

        transform()->position.x -= overlap_x;
        transform()->velocity.x = 0.0f;
        transform()->moved_velocity.x = 0.0f;
        transform()->moved_velocity.y = 0.0f;
        if (m_is_hovering || m_is_locked_jump || m_is_on_ground) return;
        //if (m_is_on_ground && !is_pressed(key_up)) return;

        m_is_wall_to_left = other_rect.x < our_rect.x;
        if (( m_is_wall_to_left && transform()->velocity.x + transform()->moved_velocity.x > 0.0f) ||
            (!m_is_wall_to_left && transform()->velocity.x + transform()->moved_velocity.x < 0.0f)) return;
                
        if (m_time_left_jumping_wall == 0 && !m_is_carrying && !m_is_hovering &&
            transform()->velocity.y > 0.0f &&
            (m_is_wall_to_left && is_pressed(key_left) || !m_is_wall_to_left && is_pressed(key_right))) {
            m_is_sliding_wall = true;
            if (transform()->velocity.y > 1.0f) {
                transform()->velocity.y = 1.0f;
            }            
            sprite()->is_leftward = !m_is_wall_to_left;
        }
        if (is_pressed(key_jump) && !m_is_locked_jump && m_time_left_jumping_wall <= 0.0f && !m_is_carrying && !m_is_hovering) {
            m_time_left_jumping_wall = m_time_to_jump_wall;
            m_num_jumps = 0;
        }
        if (transform()->velocity.y > 0.0f && !m_is_carrying && !m_is_hovering) {
            if ( m_is_wall_to_left && is_pressed(key_left) ||
                !m_is_wall_to_left && is_pressed(key_right)) {
                if (!m_is_on_ledge && our_rect.y <= other_rect.y - 0 && our_rect.y >= other_rect.y - 16) {
                    m_is_on_ledge = true;
                    m_is_sliding_wall = false;
                    transform()->position.y = other_rect.y - 8;
                    transform()->velocity.y = 0.0f;

                    reset_anim(m_ledge_grab_anim_id);

                    sound::Set::at(m_ledge_grab_sound_id)->position({ position().x / 160.0f, position().y / 90.0f });
                    sound::Set::at(m_ledge_grab_sound_id)->play();
                }
            }
        }        
    } else if (other_type == entity::Type::clip_L || other_type == entity::Type::clip_D_L) {
        if (transform()->velocity.x + transform()->moved_velocity.x <= 0.0f || m_is_on_ledge) return;
        transform()->position.x -= overlap_x;
        transform()->velocity.x = 0.0f;
        transform()->moved_velocity.x = 0.0f;
        transform()->moved_velocity.y = 0.0f;
        m_is_wall_to_left = false;        
        if (m_is_on_ground) return;
               
        if (transform()->velocity.y > 0.0f && is_pressed(key_right) && m_time_left_jumping_wall == 0 && !m_is_carrying && !m_is_hovering) {
            m_is_sliding_wall = true;
            sprite()->is_leftward = true;            
            if (transform()->velocity.y > 1.0f) {
                transform()->velocity.y = 1.0f;
            }
            //set_anim(m_slide_wall_anim_id);
        }
        if (is_pressed(key_jump) && !m_is_locked_jump && m_time_left_jumping_wall <= 0.0f) {
            m_time_left_jumping_wall = m_time_to_jump_wall;
            m_num_jumps = 0;
        }

    } else if (other_type == entity::Type::clip_R || other_type == entity::Type::clip_D_R) {
        if (transform()->velocity.x + transform()->moved_velocity.x >= 0.0f || m_is_on_ledge) return;
        transform()->position.x -= overlap_x;
        transform()->velocity.x = 0.0f;
        transform()->moved_velocity.x = 0.0f;
        transform()->moved_velocity.y = 0.0f;
        m_is_wall_to_left = true;
        if (m_is_on_ground) return;
        if (transform()->velocity.y > 0.0f && is_pressed(key_left) && m_time_left_jumping_wall == 0 && !m_is_carrying && !m_is_hovering) {
            m_is_sliding_wall = true;
            sprite()->is_leftward = false;
            if (transform()->velocity.y > 1.0f) {
                transform()->velocity.y = 1.0f;
            }            
            //set_anim(m_slide_wall_anim_id);
        }
        if (is_pressed(key_jump) && !m_is_locked_jump && m_time_left_jumping_wall <= 0.0f && !m_is_carrying && !m_is_hovering) {
            m_time_left_jumping_wall = m_time_to_jump_wall;
            m_num_jumps = 0;
        }
    } else if (entity::is_conduit(other_type)) {
        return;
        if (other.owner->is_dead()) return;
        transform()->position.x -= overlap_x;
        transform()->velocity.x = transform()->moved_velocity.x = 0.0f;
    } else if (other_type == entity::Type::frog) {        
        //transform()->velocity.x = transform()->moved_velocity.x = 0.0f;

        transform()->position.x -= overlap_x;
        if (std::abs(transform()->velocity.x) > std::abs(other.owner->transform()->velocity.x)) {
            return;
        }
        f32c vel_x = transform()->velocity.x;
        transform()->velocity.x = other.owner->transform()->velocity.x * 0.5f;
        other.owner->transform()->velocity.x = vel_x * 0.25f;
        if (our_rect.x < other_rect.x) {
            transform()->velocity.x -= 1.0f;
            other.owner->transform()->velocity.x += 1.0f;
        } else {
            transform()->velocity.x += 1.0f;
            other.owner->transform()->velocity.x -= 1.0f;
        }
    } else if (other_type == entity::Type::level_L_0) {
        m_next_level = start::Type::L_0;
        m_next_start = { start::Type::R_0, 0 };
    } else if (other_type == entity::Type::level_R_0) {
        m_next_level = start::Type::R_0;
        m_next_start = { start::Type::L_0, 0 };
    } else if (other_type == entity::Type::level_center) {
        m_is_to_write_save = true;
    }
    //else if (other_type == entity::Type::lever) {
    //}
    else if (other_type == entity::Type::slope_L_1x1) {
        //if (transform()->velocity.x < 0.0f) return;
        //transform()->position.y -= overlap_x;
        transform()->velocity.y = -transform()->velocity.x;
        if (transform()->velocity.x > transform()->velocity_limit.x * 0.7f) {
            transform()->velocity.x = transform()->velocity_limit.x * 0.7f;
        }
        m_is_on_ground = true;
        m_is_on_slope = true;
    } else if (other_type == entity::Type::slope_L_2x1_0 || other_type == entity::Type::slope_L_2x1_1) {
        //if (transform()->velocity.x < 0.0f) return;
        //transform()->position.y -= overlap_x;
        transform()->velocity.y = -transform()->velocity.x / 2.0f;
        if (transform()->velocity.x > transform()->velocity_limit.x * 0.9f) {
            transform()->velocity.x = transform()->velocity_limit.x * 0.9f;
        }
        m_is_on_ground = true;
        m_is_on_slope = true;
    } else if (other_type == entity::Type::slope_R_1x1) {
        //if (transform()->velocity.x > 0.0f) return;
        //transform()->position.y -= overlap_x;
        //transform()->position.x -= overlap_x;
        //transform()->velocity.y = 0.0f;
        transform()->velocity.y = transform()->velocity.x;
        if (transform()->velocity.x < -transform()->velocity_limit.x * 0.7f) {
            transform()->velocity.x = -transform()->velocity_limit.x * 0.7f;
        }
        m_is_on_ground = true;
        m_is_on_slope = true;
    } else if (other_type == entity::Type::slope_R_2x1_0 || other_type == entity::Type::slope_R_2x1_1) {
        //if (transform()->velocity.x > 0.0f) return;
        //transform()->position.y -= overlap_x;
        //transform()->position.x -= overlap_x;
        transform()->velocity.y = transform()->velocity.x / 2.0f;
        if (transform()->velocity.x < -transform()->velocity_limit.x * 0.9f) {
            transform()->velocity.x = -transform()->velocity_limit.x * 0.9f;
        }
        m_is_on_ground = true;
        m_is_on_slope = true;
    } else if (other_type == entity::Type::spring_U) {
        collide_y(our, other);
    } else if (other_type == entity::Type::player) {
        return;
        transform()->position.x -= overlap_x;
        //transform()->velocity.x = 0.0f;

        //transform()->velocity.x = transform()->velocity.x * 0.5f + velocity.x * 0.5f;
        
        transform()->velocity.x = transform()->velocity.x * 0.75f + other_velocity.x * 0.25f;
        //transform()->velocity.y = transform()->velocity.y * 0.5f + velocity.y * 0.5f;
    }
};




void player::Object::collide_y(aabb::Infoc our, aabb::Infoc other) {
    if (!transform() || !our.owner || !other.owner || !other.owner->transform() || !aabb::Set::at(our.id) || !aabb::Set::at(other.id)) return;
    if (aabb::Set::at(our.id)->is_inactive() || aabb::Set::at(other.id)->is_inactive()) return;
    if (aabb::Set::at(our.id)->points().size() < 4 || aabb::Set::at(other.id)->points().size() < 4) return;

    aabb::Name our_aabb_name = aabb::Set::at(our.id)->name;
    
    if (our_aabb_name == aabb::Name::interact) {
        collide_interact_x(our, other);
        return;
    } else if (our_aabb_name == aabb::Name::melee_L || our_aabb_name == aabb::Name::melee_R) {
        collide_melee_x(our, other);
        return;
    }

    Rectfc our_rect = { aabb::Set::at(our.id)->points().at(0).x, aabb::Set::at(our.id)->points().at(0).y,
                        aabb::Set::at(our.id)->points().at(3).x, aabb::Set::at(our.id)->points().at(3).y };

    entity::Typec other_type = other.owner->type();
    Vec2fc        other_velocity = other.owner->transform()->velocity;
    aabb::Name    other_aabb_name = aabb::Set::at(other.id)->name;
    Rectfc        other_rect = { aabb::Set::at(other.id)->points().at(0).x, aabb::Set::at(other.id)->points().at(0).y,
                                 aabb::Set::at(other.id)->points().at(3).x, aabb::Set::at(other.id)->points().at(3).y };

    /*if (other_rect.y < our_rect.y && transform()->velocity.y >= 0.0f ||
        other_rect.y > our_rect.y && transform()->velocity.y <= 0.0f) return;*/
    
    f32c overlap_y = our_rect.y < other_rect.y ? our_rect.h - other_rect.y : -(other_rect.h - our_rect.y);
    
    //Console::log(sprite::type_to_string(other_type), " overlap_y: ", overlap_y, "\n");
  
    if (other_type == entity::Type::arch_L_1x1   || other_type == entity::Type::arch_R_1x1 || 
        other_type == entity::Type::arch_L_2x1_0 || other_type == entity::Type::arch_L_2x1_1 ||
        other_type == entity::Type::arch_R_2x1_0 || other_type == entity::Type::arch_R_2x1_1) {
            
        if (m_is_on_ground) {
            m_time_left_ducking = m_time_to_duck;
        }
        if (our_rect.y < other_rect.y || m_time_left_ducking > 0) {
            return;
        }        
        
        if (!m_is_on_ground && m_time_left_ducking == 0 && transform()->velocity.y < 0.0f) {
            sound::Set::at(m_bump_head_sound_id)->position({ position().x / 160.0f, position().y / 90.0f });
            sound::Set::at(m_bump_head_sound_id)->play();
        }
        transform()->position.y -= overlap_y;
        transform()->velocity.y = std::abs(transform()->velocity.y) * 0.75f;

        if (other_type == entity::Type::arch_L_1x1)
            transform()->velocity.x += -transform()->velocity.y;
        else if (other_type == entity::Type::arch_R_1x1)
            transform()->velocity.x += transform()->velocity.y;
        else if (other_type == entity::Type::arch_L_2x1_0 || other_type == entity::Type::arch_L_2x1_1)
            transform()->velocity.x += transform()->velocity.y * -0.5f;
        else if (other_type == entity::Type::arch_R_2x1_0 || other_type == entity::Type::arch_R_2x1_1)
            transform()->velocity.x += transform()->velocity.y * 0.5f;


    } else if (other_type == entity::Type::bug) {
        if (other.owner->parent() == this || other.owner->is_interacting()) return;
        /*if (our_rect.y > other_rect.h) {
            return;
        }*/
        transform()->position.y -= overlap_y;
        if (our_rect.y >= other_rect.y) {
            Vec2f other_vel = other.owner->transform()->velocity + other.owner->transform()->moved_velocity;            
            other.owner->transform()->velocity.y = transform()->velocity.y / 4.0f - 3.0f;
            other.owner->transform()->position.y -= 8.0f;

            f32c our_mid_x = our_rect.w - our_rect.x / 2.0f;
            f32c other_mid_x = other_rect.w - other_rect.x / 2.0f;
            
            f32c mid_diff_x = other_mid_x - our_mid_x;
            other.owner->transform()->velocity.x = (transform()->velocity.x + transform()->moved_velocity.x) + mid_diff_x / 4.0f;

            //Console::log("mid diff: ", , "\n");
            // 
            //transform()->velocity = vel / 2.0f;

            f32c v = other.owner->transform()->velocity.y;

            //Console::log("entity::Bug::collide_y vel y: ", v, "\n");

            //hurt();
            Console::log("player hurt\n");

            return;
        }
        if (other.owner->state() == entity::State::upended || other.owner->state() == entity::State::bounce) {
            if (transform()->velocity.y < 0.0f) return;
            transform()->position.y -= overlap_y;
            transform()->velocity_limit.y = 10.0f;
            transform()->velocity.y = -5.0f - transform()->velocity.y / 4.0f;
            m_is_down_thrusting ? reset_anim(m_jump_spin_anim_id) : reset_anim(m_jump_anim_id);
            m_is_down_thrusting = false;
            m_is_on_ground = false;
            m_is_on_slope = false;
            m_num_jumps = 1;
            m_time_left_bouncing = m_time_to_bounce;
            return;
        }
        transform()->moved_velocity = other_velocity;
        //transform()->velocity.x *= 0.5f;
        transform()->velocity.y = 0.0f;       
        m_is_on_ground = true;
    } else if (other_type == entity::Type::clip) {
        transform()->position.y -= overlap_y;
        transform()->velocity.y = transform()->moved_velocity.y = 0.0f;
        if (our_rect.y < other_rect.y) {
            m_is_on_ground = true;
            m_is_on_slope = false;
        }
    } else if (other_type == entity::Type::clip_duck) {
        if ((other_rect.y < our_rect.y && m_is_on_ground) || m_is_climbing_ledge) {
            m_time_left_rising = m_time_to_rise;
            m_time_left_ducking = m_time_to_duck;            
            return;
        }
        if (m_time_left_rising > 0) {
            return;
        }
        transform()->position.y -= overlap_y;
        if (transform()->velocity.y >= 0.0f) {
            transform()->velocity.y = transform()->moved_velocity.y = 0.0f;

            //if (our_rect.y + our_rect.h > other_rect.y + other_rect.h
                //&& (sprite()->is_leftward && other_rect.x + other_rect.w < our_rect.x) || (!sprite()->is_leftward && other_rect.x > our_rect.x + our_rect.w)
                //) {
            m_time_left_rising = m_time_to_rise;
            m_time_left_ducking = m_time_to_duck;

            if (our_rect.y < other_rect.y) {
                m_is_on_ground = true;
                m_is_on_slope = false;
            }
        } else {
            transform()->velocity.y = std::abs(transform()->velocity.y) * 0.75f;
            sound::Set::at(m_bump_head_sound_id)->position({ position().x / 160.0f, position().y / 90.0f });
            sound::Set::at(m_bump_head_sound_id)->play();
        }
    } else if (other_type == entity::Type::clip_D || other_type == entity::Type::clip_D_L || other_type == entity::Type::clip_D_R) {
        if (transform()->velocity.y > 0.0f) return;
        transform()->position.y -= overlap_y;
        
        m_is_on_ground = false;
        m_is_on_slope = false;

        transform()->velocity.y = std::abs(transform()->velocity.y) * 0.75f;
        sound::Set::at(m_bump_head_sound_id)->play();
    } else if (other_type == entity::Type::clip_L) {
    } else if (other_type == entity::Type::clip_R) {
    } else if (other_type == entity::Type::clip_ledge) {
        //Console::log("ledge pos: ", other_rect.x, " ", other_rect.y, "\n");
        /*Console::log(" ", our_rect.y, " ", our_rect.h, "  ", other_rect.y, " ", other_rect.h, "\n");*/
        transform()->position.y -= overlap_y;

        if (our_rect.y < other_rect.y) {
            m_is_on_ground = true;
            m_is_on_slope = false;
            transform()->moved_velocity.x = 0.0f;
            transform()->velocity.y = transform()->moved_velocity.y = 0.0f;
        } else {            
            transform()->velocity.y = std::abs(transform()->velocity.y) * 0.75f;
            sound::Set::at(m_bump_head_sound_id)->position({ position().x / 160.0f, position().y / 90.0f });
            sound::Set::at(m_bump_head_sound_id)->play();
        }        
    } else if (other_type == entity::Type::clip_U || other_type == entity::Type::slope_U) {
        if (transform()->velocity.y < 0.0f) return;
        transform()->position.y -= overlap_y;
        
        transform()->moved_velocity.x = 0.0f;
        transform()->velocity.y = transform()->moved_velocity.y = 0.0f;
        m_is_on_ground = true;
        m_is_on_slope = other_type == entity::Type::slope_U;
    } else if (entity::is_conduit(other_type)) {
        return;
        if (other.owner->is_dead()) return;
        transform()->position.y -= overlap_y;
        if (our_rect.y < other_rect.y) {
            m_is_on_ground = true;
            m_is_on_slope = false;
            transform()->moved_velocity.x = 0.0f;
            transform()->velocity.y = transform()->moved_velocity.y = 0.0f;
        } else {
            transform()->velocity.y = std::abs(transform()->velocity.y) * 0.75f;
            sound::Set::at(m_bump_head_sound_id)->position({ position().x / 160.0f, position().y / 90.0f });
            sound::Set::at(m_bump_head_sound_id)->play();
        }
    } else if (other_type == entity::Type::frog) {
        if (our_rect.h < other_rect.y) {

        } else {
            transform()->position.y -= overlap_y;
            transform()->velocity.y = 0.0f;
            transform()->moved_velocity.y = transform()->velocity.y;
            m_is_on_ground = true;
            m_is_on_slope = false;
        }
    } else if (other_type == entity::Type::level_L_0 || other_type == entity::Type::level_R_0) {
        collide_x(our, other);
    } else if (other_type == entity::Type::level_center) {
        collide_x(our, other);
    } 
    //else if (other_type == entity::Type::lever) {
    //}
    else if (other_type == entity::Type::player) {
        return;
        transform()->position.y -= overlap_y;

        //Console::log("overlap_y: ", overlap_y, "\n");        

        if (our_rect.y < other_rect.y) {
            m_is_on_ground = true;
            m_is_on_slope = false;
            //time_left_holding_jump = time_to_hold_jump;

            transform()->moved_velocity = other_velocity;
            transform()->velocity.y = 0.0f;
            //transform()->position.y -= 1.0f;

            //Console::log("velocity: ", velocity.x, " ", velocity.y, "\n");
            //transform()->velocity = { 0.0f, 0.0f };
            //transform()->velocity.x -= (transform()->velocity.x - velocity.x);
        } else {
            //object->is_on_ground = true;

            //object->transform()->moved_velocity = transform()->velocity;
            //if (!is_on_ground && transform()->velocity.y < 0.0f) {
            //    transform()->velocity.y *= -1.0f;
            //}
            //transform()->moved_velocity = other_velocity;
            //transform()->velocity.y = 0.0f;
        }


        //transform()->velocity = { 0.0f, 0.0f };


        //transform()->velocity.y = velocity.y;
        //transform()->velocity.y = 0.0f;
    }
    else if (other_type == entity::Type::slope_L_1x1) {
        //if (transform()->velocity.y < 0.0f) return;
        transform()->position.y -= overlap_y;

        transform()->velocity.y = 0.0f;
        transform()->moved_velocity.x = 0.0f;
        transform()->moved_velocity.y = 0.0f;
        //if (transform()->velocity.x < 0.0f) {
            transform()->velocity.y = -transform()->velocity.x;
        //}
        m_is_on_ground = true;
        m_is_on_slope = true;
        if (transform()->velocity.x > transform()->velocity_limit.x * 0.7f) {
            transform()->velocity.x = transform()->velocity_limit.x * 0.7f;
        }
    } else if (other_type == entity::Type::slope_L_2x1_0 || other_type == entity::Type::slope_L_2x1_1) {
        //if (transform()->velocity.y < 0.0f) return;
        transform()->position.y -= overlap_y;

        transform()->velocity.y = 0.0f;
        transform()->moved_velocity.x = 0.0f;
        transform()->moved_velocity.y = 0.0f;
        //if (transform()->velocity.x < 0.0f) {
            transform()->velocity.y = transform()->velocity.x / -2.0f;
        //}
        m_is_on_ground = true;
        m_is_on_slope = true;
        if (transform()->velocity.x > transform()->velocity_limit.x * 0.9f) {
            transform()->velocity.x = transform()->velocity_limit.x * 0.9f;
        }
    } else if (other_type == entity::Type::slope_R_1x1) {
        //if (transform()->velocity.y < 0.0f) return;
        transform()->position.y -= overlap_y;

        transform()->velocity.y = 0.0f;
        transform()->moved_velocity.x = 0.0f;
        transform()->moved_velocity.y = 0.0f;
        //if (transform()->velocity.x > 0.0f) {
            transform()->velocity.y = transform()->velocity.x;
        //}
        m_is_on_ground = true;
        m_is_on_slope = true;
        if (transform()->velocity.x < -transform()->velocity_limit.x * 0.7f) {
            transform()->velocity.x = -transform()->velocity_limit.x * 0.7f;
        }
    } else if (other_type == entity::Type::slope_R_2x1_0 || other_type == entity::Type::slope_R_2x1_1) {
        //if (transform()->velocity.y < 0.0f) return;
        transform()->position.y -= overlap_y;

        transform()->velocity.y = 0.0f;
        transform()->moved_velocity.x = 0.0f;
        transform()->moved_velocity.y = 0.0f;
        if (transform()->velocity.x > 0.0f) {
            transform()->velocity.y = transform()->velocity.x / 2.0f;
        }
        m_is_on_ground = true;
        m_is_on_slope = true;
        if (transform()->velocity.x < -transform()->velocity_limit.x * 0.9f) {
            transform()->velocity.x = -transform()->velocity_limit.x * 0.9f;
        }
    } else if (other_type == entity::Type::spring_U) {
        if (transform()->velocity.y < 0.0f) return;
        transform()->position.y -= overlap_y;
        transform()->velocity_limit.y = 10.0f;
        transform()->velocity.y = -5.0f - transform()->velocity.y / 4.0f;
        m_is_down_thrusting ? reset_anim(m_jump_spin_anim_id) : reset_anim(m_jump_anim_id);
        m_is_down_thrusting = false;
        m_is_on_ground = false;
        m_is_on_slope = false;
        m_num_jumps = 1;
        m_time_left_bouncing = m_time_to_bounce;
    } else if (other_type == entity::Type::train) {
        //Console::log("hello train ", aabb::name_as_string(other_aabb_name), "\n");
        if (transform()->velocity.y < 0.0f || other_aabb_name == aabb::Name::bone || our_rect.h > other_rect.h) return;

        if (other.owner->transform()) {
            transform()->moved_velocity = other.owner->transform()->velocity;
            // move in so we don't fly off at turns            
            //if (transform()->moved_velocity.y < 0.0f) {
                //transform()->velocity.y = -transform()->moved_velocity.y;
                transform()->velocity.y = 1.0f;
                transform()->position.y -= overlap_y / 6.0f;
            //} else {
                //transform()->velocity.y = 0.0f;
                //transform()->position.y -= overlap_y;
            //}
            m_is_on_ground = true;
        }
    }
};