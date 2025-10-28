module;
#include <sstream>
module player;
import console;
import particle;

void player::Object::collide_x(aabb::cInfo our, aabb::cInfo other) {
    if (!transform() || !our.owner || !other.owner || !other.owner->transform() || !aabb::Manager::get(our.id) || !aabb::Manager::get(other.id)) return;
    if (other.owner->parent() == this) return;
    
    if (aabb::Manager::get(our.id)->points().size() < 4 || aabb::Manager::get(other.id)->points().size() < 4) return;

    aabb::cName our_aabb_name = aabb::Manager::get(our.id)->name;

    cRectF our_rect   = { aabb::Manager::get(our.id)->points().at(0).x, aabb::Manager::get(our.id)->points().at(0).y,
                          aabb::Manager::get(our.id)->points().at(3).x, aabb::Manager::get(our.id)->points().at(3).y };
    cRectF other_rect = { aabb::Manager::get(other.id)->points().at(0).x, aabb::Manager::get(other.id)->points().at(0).y,
                          aabb::Manager::get(other.id)->points().at(3).x, aabb::Manager::get(other.id)->points().at(3).y };

    if (other_rect.x < our_rect.x && transform()->velocity.x > 0.0F ||
        other_rect.x > our_rect.x && transform()->velocity.x < 0.0F) return;

    entity::cType other_type      = other.owner->type();
    aabb::cName   other_aabb_name = aabb::Manager::get(other.id)->name;

    entity::cState other_state = other.owner->state();

    cVec2F our_velocity = transform()->velocity;
    cVec2F other_velocity = other.owner->transform()->velocity;

    cF32 overlap_x = our_rect.x < other_rect.x ? our_rect.w - other_rect.x : -(other_rect.w - our_rect.x);

    if (m_state == entity::State::swing) {
        //transform()->position.x -= overlap_x;
        //transform()->velocity.x = -transform()->velocity.x;

        if (entity::is_arch(other_type)) {
            collide_y(our, other);
        }
        else if (entity::is_clip(other_type)) {
            if (!sound::Manager::get(m_bump_head_sound_id)->is_playing()) {
                sound::Manager::get(m_bump_head_sound_id)->stop();
                sound::Manager::get(m_bump_head_sound_id)->position({ position().x / 160.0F, position().y / 90.0F });
                sound::Manager::get(m_bump_head_sound_id)->play();

                m_rotation_speed *= -1.0F;                
            }
        }
        return;
    }

    //Console::log(sprite::to_string(other_type), " overlap_x: ", overlap_x, "\n");


    if (entity::is_arch(other_type)) {
        if (m_is_climbing_ledge) {
            return;
        }
        if (m_is_on_ground && m_is_on_slope) {
            transform()->position.x -= overlap_x;
            transform()->velocity.x *= 0.0F;
            //return;
        }
        //if (m_state == entity::State::swim) {
        //    transform()->position.x -= overlap_x;
            /*if (other_type == entity::Type::arch_L_1x1 && transform()->velocity.x >= 0.0F)
                transform()->velocity.y = std::abs(transform()->velocity.x);
            else if (other_type == entity::Type::arch_R_1x1 && transform()->velocity.x <= 0.0F)
                transform()->velocity.y = std::abs(transform()->velocity.x);
            else if (other_type == entity::Type::arch_L_2x1_0 || other_type == entity::Type::arch_L_2x1_1 && transform()->velocity.x >= 0.0F)
                transform()->velocity.y = std::abs(transform()->velocity.x) * 0.75F;
            else if (other_type == entity::Type::arch_R_2x1_0 || other_type == entity::Type::arch_R_2x1_1 && transform()->velocity.x <= 0.0F)
                transform()->velocity.y = std::abs(transform()->velocity.y) * 0.75F;*/
        //    transform()->moved_velocity.x = 0.0F;

        //    transform()->velocity.x *= 0.9F;
        //    if (other_type == entity::Type::arch_L_1x1) {
        //        //if (transform()->velocity.x > 0.0F) {
        //            transform()->velocity.y = transform()->velocity.x;
        //        //}
        //    }
        //    else if (other_type == entity::Type::arch_R_1x1) {
        //        //if (transform()->velocity.x < 0.0F) {
        //            transform()->velocity.y = -transform()->velocity.x;
        //        //}
        //    }
        //    else if (other_type == entity::Type::arch_L_2x1_0 || other_type == entity::Type::arch_L_2x1_1) {
        //        //if (transform()->velocity.x > 0.0F) {
        //            transform()->velocity.y = transform()->velocity.x * 0.5F;
        //        //}
        //    }            
        //    else if (other_type == entity::Type::arch_R_2x1_0 || other_type == entity::Type::arch_R_2x1_1) {
        //        //if (transform()->velocity.x < 0.0F) {
        //            transform()->velocity.y = -transform()->velocity.x * 0.5F;
        //        //}
        //    }            
        //    return;
        //} else {
            collide_y(our, other);
        //}
    }
    else if (entity::is_slope(other_type) && other_type != entity::Type::slope_U && m_state == entity::State::swim) {
        collide_y(our, other);
    }
    else if (other_type == entity::Type::brick) {
        if (our_rect.h < other_rect.y + 2.0F ||
            our_rect.y > other_rect.h - 2.0F) {
            //collide_y(our, other);
            return;
        }
        transform()->position.x -= overlap_x;
        transform()->velocity.x *= 0.9F;
    } 
    else if (other_type == entity::Type::bug) {
        /*if (our_rect.h < other_rect.y + 2.0F ||
            our_rect.y > other_rect.h - 2.0F) {
            return;
        }
        transform()->position.x -= overlap_x;*/
    }
    else if (other_type == entity::Type::clip) {
        transform()->position.x -= overlap_x;
        transform()->velocity.x = transform()->moved_velocity.x = 0.0F;
    } else if (other_type == entity::Type::clip_duck) {
        if (m_state == entity::State::swim) {
            transform()->velocity.x = 0.0F;
            return;
        }
        if (m_is_carrying) {
            transform()->velocity.x = 0.0F;
            return;
        }

        if ((/*other_rect.h < our_rect.y &&*/ m_is_on_ground) || m_is_climbing_ledge) {
            m_time_left_rising = m_time_to_rise;
            m_time_left_ducking = m_time_to_duck;
            if (!is_pressed(key_down)) {
                if (transform()->velocity.x < 0.0F && transform()->velocity.x < -m_ground_velocity_limit.x ||
                    transform()->velocity.x > 0.0F && transform()->velocity.x > m_ground_velocity_limit.x) {
                    transform()->velocity.x *= 0.5F;
                    //Console::log("yo crawl damnit\n");
                }                
            }
            return;
        }
        /*if (transform()->velocity.x < 0.0F && our_rect.w < other_rect.x ||
            transform()->velocity.x > 0.0F && our_rect.x > other_rect.w) {
            return;
        }*/
        if (m_time_left_rising > 0) {
            return;
        }
        m_is_wall_to_left = other_rect.x < our_rect.x;
        if ((m_is_wall_to_left && transform()->velocity.x + transform()->moved_velocity.x > 0.0F) ||
            (!m_is_wall_to_left && transform()->velocity.x + transform()->moved_velocity.x < 0.0F)) return;
        
        if (m_time_left_jumping_wall == 0 && !m_is_on_ground && !m_is_carrying && !m_is_hovering &&
            transform()->velocity.y > 0.0F &&
            (m_is_wall_to_left && is_pressed(key_left) || !m_is_wall_to_left && is_pressed(key_right))) {
            m_is_sliding_wall = true;
            if (transform()->velocity.y > 1.0F) {
                transform()->velocity.y = 1.0F;
            }
            sprite()->is_leftward = !m_is_wall_to_left;
        }
        if (is_pressed(key_jump) && !m_is_carrying && !m_is_hovering && !m_is_locked_jump && m_time_left_jumping_wall <= 0.0F) {
            m_time_left_jumping_wall = m_time_to_jump_wall;
            m_num_jumps = 0;
        }

        if (transform()->velocity.y > 0.0F) {
            transform()->position.x -= overlap_x;
            transform()->velocity.x = 0.0F;
            transform()->moved_velocity.x = 0.0F;

            if (m_is_on_ground) {
                m_time_left_rising = m_time_to_rise;
                m_time_left_ducking = m_time_to_duck;
                m_is_ducking = true;
                //press(key_down);
                //collider::aabb::Manager::get(aabb_ids().back())->set_rect(m_transform_id, { 0, 16, 16, 8 });
            }
        }
    } else if (other_type == entity::Type::clip_U || other_type == entity::Type::slope_U) {
    } else if (other_type == entity::Type::clip_D) {
    } else if (other_type == entity::Type::clip_ledge) {
        if (m_is_on_ledge) return;
        if (our_rect.y >= other_rect.h - 2.0F) {
            return;
        }
        if (our_rect.h <= other_rect.y + 2.0F) {
            return;
        }
        if (transform()->velocity.x < 0.0F && our_rect.w < other_rect.x + transform()->velocity.x ||
            transform()->velocity.x > 0.0F && our_rect.x > other_rect.w + transform()->velocity.x) {
            return;
        }

        transform()->position.x -= overlap_x;
        transform()->velocity.x = 0.0F;
        transform()->moved_velocity.x = 0.0F;
        transform()->moved_velocity.y = 0.0F;
        if (m_state == entity::State::swim) {
            if (m_num_jumps == 1 && transform()->velocity.y > 0.0F) {
                next_state(entity::State::run);
            }
            return;
        }
        m_is_on_slope = false;
        if (m_is_hovering || m_is_locked_jump || m_is_on_ground) return;
        //if (m_is_on_ground && !is_pressed(key_up)) return;

        m_is_wall_to_left = other_rect.x < our_rect.x;
        if (( m_is_wall_to_left && transform()->velocity.x + transform()->moved_velocity.x > 0.0F) ||
            (!m_is_wall_to_left && transform()->velocity.x + transform()->moved_velocity.x < 0.0F)) return;
                
        if (m_time_left_jumping_wall == 0 && !m_is_on_ground && !m_is_carrying && !m_is_hovering &&
            transform()->velocity.y > 0.0F &&
            (m_is_wall_to_left && is_pressed(key_left) || !m_is_wall_to_left && is_pressed(key_right))) {
            m_is_sliding_wall = true;
            if (transform()->velocity.y > 1.0F) {
                transform()->velocity.y = 1.0F;
            }            
            sprite()->is_leftward = !m_is_wall_to_left;
        }
        if (is_pressed(key_jump) && !m_is_locked_jump && m_time_left_jumping_wall <= 0.0F && !m_is_carrying && !m_is_hovering) {
            m_time_left_jumping_wall = m_time_to_jump_wall;
            m_num_jumps = 0;
        }
        if (transform()->velocity.y > 0.0F && !m_is_carrying && !m_is_hovering) {
            if ( m_is_wall_to_left && is_pressed(key_left) ||
                !m_is_wall_to_left && is_pressed(key_right)) {
                if (!m_is_on_ledge && our_rect.y <= other_rect.y - 0 && our_rect.y >= other_rect.y - 16) {
                    m_is_on_ledge = true;
                    m_is_sliding_wall = false;
                    transform()->position.y = other_rect.y - 8;
                    transform()->velocity.y = 0.0F;

                    reset_anim(m_ledge_grab_anim_id);

                    sound::Manager::get(m_ledge_grab_sound_id)->position({ position().x / 160.0F, position().y / 90.0F });
                    sound::Manager::get(m_ledge_grab_sound_id)->play();
                }
            }
        }        
    } else if (other_type == entity::Type::clip_L || other_type == entity::Type::clip_LD) {
        if (transform()->velocity.x + transform()->moved_velocity.x <= 0.0F || m_is_on_ledge) return;
        transform()->position.x -= overlap_x;
        transform()->velocity.x = 0.0F;
        transform()->moved_velocity.x = 0.0F;
        transform()->moved_velocity.y = 0.0F;
        if (m_state == entity::State::swim && m_num_jumps == 1 && transform()->velocity.y > 0.0F) {
            m_next_state = entity::State::run;
            return;
        }
        m_is_wall_to_left = false;
        m_is_on_slope = false;
        if (m_is_on_ground) return;
               
        if (transform()->velocity.y > 0.0F && is_pressed(key_right) && m_time_left_jumping_wall == 0 && !m_is_on_ground && !m_is_carrying && !m_is_hovering) {
            m_is_sliding_wall = true;
            sprite()->is_leftward = true;            
            if (transform()->velocity.y > 1.0F) {
                transform()->velocity.y = 1.0F;
            }
            //set_anim(m_slide_wall_anim_id);
        }
        if (is_pressed(key_jump) && !m_is_locked_jump && m_time_left_jumping_wall <= 0.0F) {
            m_time_left_jumping_wall = m_time_to_jump_wall;
            m_num_jumps = 0;
        }

    } else if (other_type == entity::Type::clip_R || other_type == entity::Type::clip_RD) {
        if (transform()->velocity.x + transform()->moved_velocity.x >= 0.0F || m_is_on_ledge) return;
        transform()->position.x -= overlap_x;
        transform()->velocity.x = 0.0F;
        transform()->moved_velocity.x = 0.0F;
        transform()->moved_velocity.y = 0.0F;
        if (m_state == entity::State::swim && m_num_jumps == 1 && transform()->velocity.y > 0.0F) {
            m_next_state = entity::State::run;
            return;
        }
        m_is_wall_to_left = true;
        m_is_on_slope = false;
        if (m_is_on_ground) return;
        if (transform()->velocity.y > 0.0F && is_pressed(key_left) && m_time_left_jumping_wall == 0 && !m_is_on_ground && !m_is_carrying && !m_is_hovering) {
            m_is_sliding_wall = true;
            sprite()->is_leftward = false;
            if (transform()->velocity.y > 1.0F) {
                transform()->velocity.y = 1.0F;
            }            
            //set_anim(m_slide_wall_anim_id);
        }
        if (is_pressed(key_jump) && !m_is_locked_jump && m_time_left_jumping_wall <= 0.0F && !m_is_carrying && !m_is_hovering) {
            m_time_left_jumping_wall = m_time_to_jump_wall;
            m_num_jumps = 0;
        }
    } else if (entity::is_conduit(other_type)) {
        return;
        if (other.owner->is_dead()) return;
        transform()->position.x -= overlap_x;
        transform()->velocity.x = transform()->moved_velocity.x = 0.0F;
    } else if (other_type == entity::Type::frog) {
        if (our_rect.h + 1.0F <= other_rect.y) return;
        //transform()->velocity.x = transform()->moved_velocity.x = 0.0F;

        transform()->position.x -= overlap_x;
        if (std::abs(transform()->velocity.x) > std::abs(other.owner->transform()->velocity.x)) {
            return;
        }

        cF32 vel_x = transform()->velocity.x;
        transform()->velocity.x = other.owner->transform()->velocity.x * 0.5F;
        other.owner->transform()->velocity.x = vel_x * 0.25f;
        if (our_rect.x < other_rect.x) {
            transform()->velocity.x -= 1.0F;
            other.owner->transform()->velocity.x += 1.0F;
        } else {
            transform()->velocity.x += 1.0F;
            other.owner->transform()->velocity.x -= 1.0F;
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
    else if (other_type == entity::Type::particle_brick) {
        //transform()->position.x -= overlap_x;
        //transform()->velocity.x *= 0.95F;
    }
    else if (other_type == entity::Type::slope_L_1x1) {
        if (our_rect.h > other_rect.h) return;

        //if (transform()->velocity.x < 0.0F) return;
        if (m_state == entity::State::swim) {
            if (our_velocity.x > 0.0F && std::abs(our_velocity.y) < our_velocity.x) {
                transform()->velocity.y = -our_velocity.x * 0.9F;
                transform()->velocity.x *= 0.95f;
                transform()->position.y -= 1.0F;
            }
            return;
        }
        //transform()->position.y -= overlap_x;
        if (transform()->velocity.x > transform()->max_velocity.x * 0.7F) {
            transform()->velocity.x = transform()->max_velocity.x * 0.7F;
        }
        //transform()->velocity.y = -transform()->velocity.x;
        //transform()->position.y -= 1.0F;
        transform()->position.y -= transform()->velocity.x;
        
        m_is_on_ground = true;
        m_is_on_slope = true;
    } else if (other_type == entity::Type::slope_R_1x1) {
        if (our_rect.h > other_rect.h) return;
        
        //if (transform()->velocity.x > 0.0F) return;
        if (m_state == entity::State::swim) {
            if (our_velocity.x < 0.0F && std::abs(our_velocity.y) < our_velocity.x) {
                transform()->velocity.y = our_velocity.x * 0.9F;
                transform()->velocity.x *= 0.95f;
                transform()->position.y -= 1.0F;
            }
            return;
        }
        //transform()->position.y -= overlap_x;
        //transform()->position.x -= overlap_x;
        //transform()->velocity.y = 0.0F;
        if (transform()->velocity.x < -transform()->max_velocity.x * 0.7F) {
            transform()->velocity.x = -transform()->max_velocity.x * 0.7F;
        }
        //transform()->velocity.y = transform()->velocity.x;
        //transform()->position.y -= 1.0F;
        transform()->position.y += transform()->velocity.x;
        m_is_on_ground = true;
        m_is_on_slope = true;
    } else if (other_type == entity::Type::slope_L_2x1_0 || other_type == entity::Type::slope_L_2x1_1) {
        if (our_rect.h > other_rect.h) return;

        //if (transform()->velocity.x < 0.0F) return;
        if (m_state == entity::State::swim) {
            if (our_velocity.x > 0.0F && std::abs(our_velocity.y) < our_velocity.x) {
                transform()->velocity.y = -our_velocity.x * 0.5F;
                transform()->velocity.x *= 0.95f;
                transform()->position.y -= 1.0F;
            }
            return;
        }
        //transform()->position.y -= overlap_x;
        if (transform()->velocity.x > transform()->max_velocity.x * 0.9F) {
            transform()->velocity.x = transform()->max_velocity.x * 0.9F;
        }
        //transform()->velocity.y = -transform()->velocity.x / 2.0F;
        //transform()->velocity.y -= 0.5F;
        transform()->position.y -= transform()->velocity.x / 2.0F;
        m_is_on_ground = true;
        m_is_on_slope = true;
    } else if (other_type == entity::Type::slope_R_2x1_0 || other_type == entity::Type::slope_R_2x1_1) {
        if (our_rect.h > other_rect.h) return;

        //if (transform()->velocity.x > 0.0F) return;
        if (m_state == entity::State::swim) {
            if (our_velocity.x < 0.0F && std::abs(our_velocity.y) < our_velocity.x) {
                transform()->velocity.y = our_velocity.x * 0.5F;
                transform()->velocity.x *= 0.95f;
                transform()->position.y -= 1.0F;
            }
            return;
        }
        //transform()->position.y -= overlap_x;
        //transform()->position.x -= overlap_x;
        if (transform()->velocity.x < -transform()->max_velocity.x * 0.9F) {
            transform()->velocity.x = -transform()->max_velocity.x * 0.9F;
        }
        //transform()->velocity.y = transform()->velocity.x / 2.0F;
        //transform()->velocity.y -= 0.5F;
        transform()->position.y += transform()->velocity.x / 2.0F;
        
        m_is_on_ground = true;
        m_is_on_slope = true;
    } else if (other_type == entity::Type::spring_U) {
        collide_y(our, other);
    } else if (other_type == entity::Type::train) {
        if (other.owner->time_left_alive() > 0) {
            m_is_on_ledge = false;
        }
    } else if (other_type == entity::Type::player) {
        return;
        transform()->position.x -= overlap_x;
        //transform()->velocity.x = 0.0F;

        //transform()->velocity.x = transform()->velocity.x * 0.5F + velocity.x * 0.5F;
        
        transform()->velocity.x = transform()->velocity.x * 0.75F + other_velocity.x * 0.25f;
        //transform()->velocity.y = transform()->velocity.y * 0.5F + velocity.y * 0.5F;
    }


    if (m_is_sliding_wall && m_state == entity::State::run) {
        //Console::log("sliding wall: ", (int)m_time_sliding_wall, "\n");
        if (m_time_sliding_wall > 5) {
            m_time_sliding_wall = 0;
            cF32 wall_x = our_rect.x < other_rect.x ? other_rect.x : other_rect.w;

            particle::Manager::spawn({ .parent = this,
                                              .type = particle::Type::dust,
                                              .position = Vec2F{ wall_x, position().y + 8.0F },
                                              .velocity = Vec2F{ 0.0F, 0.0F } });
        }
    }
};








void player::Object::collide_y(aabb::cInfo our, aabb::cInfo other) {
    if (!transform() || !our.owner || !other.owner || !other.owner->transform() || !aabb::Manager::get(our.id) || !aabb::Manager::get(other.id)) return;
    if (other.owner->parent() == this) return;

    aabb::cName our_aabb_name = aabb::Manager::get(our.id)->name;
    
    cRectF our_rect = { aabb::Manager::get(our.id)->points().at(0).x, aabb::Manager::get(our.id)->points().at(0).y,
                        aabb::Manager::get(our.id)->points().at(3).x, aabb::Manager::get(our.id)->points().at(3).y };

    entity::cType other_type = other.owner->type();
    cVec2F        other_velocity = other.owner->transform()->velocity;
    aabb::cName   other_aabb_name = aabb::Manager::get(other.id)->name;
    cRectF        other_rect = { aabb::Manager::get(other.id)->points().at(0).x, aabb::Manager::get(other.id)->points().at(0).y,
                                 aabb::Manager::get(other.id)->points().at(3).x, aabb::Manager::get(other.id)->points().at(3).y };

    cVec2F        our_velocity = transform()->velocity;

    /*if (other_rect.y < our_rect.y && transform()->velocity.y >= 0.0F ||
        other_rect.y > our_rect.y && transform()->velocity.y <= 0.0F) return;*/
    
    cF32 overlap_y = our_rect.y < other_rect.y ? our_rect.h - other_rect.y : -(other_rect.h - our_rect.y);
    

    if (m_state == entity::State::swim && position_on_level().y < m_water_line_y) {
        if (entity::is_clip(other_type)     ||
            entity::is_slope(other_type)    ||
            (other_type == entity::Type::bug && other.owner->state() != entity::State::swim) ||
            (other_type == entity::Type::frog && other.owner->state() != entity::State::swim)) {
            Console::log("player::Object::collide_y ground above water line\n");
            next_state(entity::State::run);
        }
    }

    if (m_state == entity::State::run && our_rect.y < other_rect.y) {
        if (!m_is_on_ground && !m_is_on_slope && transform()->velocity.y > 1.0F &&
               (entity::is_clip(other_type) ||
                entity::is_slope(other_type))
            ) {
            particle::Manager::spawn({ .parent = this,
                                              .type = particle::Type::dust,
                                              .position = Vec2F{ our_rect.x + 8.0F, other_rect.y },
                                              .velocity = Vec2F{ 0.0F, 0.0F } });            
        }
    }

    if (m_state == entity::State::swing && (entity::is_clip(other_type) || entity::is_arch(other_type))) {
        if (!sound::Manager::get(m_bump_head_sound_id)->is_playing()) {
            sound::Manager::get(m_bump_head_sound_id)->stop();
            sound::Manager::get(m_bump_head_sound_id)->position({ position().x / 160.0F, position().y / 90.0F });
            sound::Manager::get(m_bump_head_sound_id)->play();

            m_rotation_speed *= -1.0F;
            //transform()->position.y -= overlap_y;
            //transform()->velocity.y = -transform()->velocity.y;
        }
        return;
    }
    if (entity::is_arch(other_type)) {
        if (m_is_climbing_ledge) {
            m_time_left_rising = m_time_to_rise;
            m_time_left_ducking = m_time_to_duck;
            return;
        }
        if (m_state == entity::State::swim) {
            //transform()->position.y -= overlap_y;
            transform()->moved_velocity.y = 0.0F;

            if (std::abs(transform()->velocity.x) > std::abs(transform()->velocity.y)) {                
                if (other_type == entity::Type::arch_L_1x1 && transform()->velocity.x > 0.0F) {
                    transform()->velocity.y = transform()->velocity.x * 0.8F;
                    //transform()->position.y += transform()->velocity.x;
                } else if (other_type == entity::Type::arch_R_1x1 && transform()->velocity.x < 0.0F) {
                    transform()->velocity.y = -transform()->velocity.x * 0.8F;
                    //transform()->position.y -= transform()->velocity.x;
                } else if (other_type == entity::Type::arch_L_2x1_0 || other_type == entity::Type::arch_L_2x1_1 && transform()->velocity.x > 0.0F) {
                    transform()->velocity.y = transform()->velocity.x * 0.75F * 0.8F;
                    //transform()->position.y += transform()->velocity.x * 0.75F;
                } else if (other_type == entity::Type::arch_R_2x1_0 || other_type == entity::Type::arch_R_2x1_1 && transform()->velocity.x < 0.0F) {
                    transform()->velocity.y = -transform()->velocity.x * 0.75F * 0.8F;
                    //transform()->position.y -= transform()->velocity.x * 0.75F;
                }
                transform()->position.y += 0.5F;
                transform()->velocity.x *= 0.95f;
            } else {
                //if (std::abs(transform()->velocity.y) > 0.2F) {
                    //transform()->velocity.y *= 0.9F;
                //}
                if (other_type == entity::Type::arch_L_1x1 && transform()->velocity.y < 0.0F) {
                    transform()->velocity.x = transform()->velocity.y * 0.8F;
                    transform()->position.x -= 1.0F;
                } else if (other_type == entity::Type::arch_R_1x1 && transform()->velocity.y < 0.0F) {
                    transform()->velocity.x = -transform()->velocity.y * 0.8F;
                    transform()->position.x += 1.0F;
                } else if (other_type == entity::Type::arch_L_2x1_0 || other_type == entity::Type::arch_L_2x1_1 && transform()->velocity.y < 0.0F) {
                    transform()->velocity.x = transform()->velocity.y * 1.2F * 0.8F;
                } else if (other_type == entity::Type::arch_R_2x1_0 || other_type == entity::Type::arch_R_2x1_1 && transform()->velocity.y < 0.0F) {
                    transform()->velocity.x = -transform()->velocity.y * 1.2F * 0.8F;
                }
                transform()->velocity.y *= 0.95f;
                /*if (transform()->velocity.x > 0.0F) {
                    transform()->position.y += transform()->velocity.x;
                } else {
                    transform()->position.y -= transform()->velocity.x;
                }*/                
            }
            return;
        }
        if (m_is_on_ground) {
            m_time_left_ducking = m_time_to_duck;
        }
        if (our_rect.y < other_rect.y || m_time_left_ducking > 0) {
            return;
        }        
        
        if (!m_is_on_ground && m_time_left_ducking == 0 && transform()->velocity.y < 0.0F) {
            sound::Manager::get(m_bump_head_sound_id)->position({ position().x / 160.0F, position().y / 90.0F });
            sound::Manager::get(m_bump_head_sound_id)->play();
        }
        transform()->position.y -= overlap_y;
        transform()->velocity.y = std::abs(transform()->velocity.y) * 0.75F;

        if (other_type == entity::Type::arch_L_1x1)
            transform()->velocity.x += -transform()->velocity.y;
        else if (other_type == entity::Type::arch_R_1x1)
            transform()->velocity.x += transform()->velocity.y;
        else if (other_type == entity::Type::arch_L_2x1_0 || other_type == entity::Type::arch_L_2x1_1)
            transform()->velocity.x += transform()->velocity.y * -0.5F;
        else if (other_type == entity::Type::arch_R_2x1_0 || other_type == entity::Type::arch_R_2x1_1)
            transform()->velocity.x += transform()->velocity.y * 0.5F;

    }
    
    if (entity::is_slope(other_type) && other_type != entity::Type::slope_U && m_state == entity::State::swim) {
        //transform()->position.y -= overlap_y;
        transform()->moved_velocity.y = 0.0F;

        F32 multiplier = other_type == entity::Type::slope_L_1x1 || other_type == entity::Type::slope_R_1x1 ? 0.5F : 1.0F;
        transform()->position.y -= (0.5F / multiplier);

        if (std::abs(transform()->velocity.x) * multiplier > std::abs(transform()->velocity.y)) {
            if (other_type == entity::Type::slope_L_1x1 && transform()->velocity.x > 0.0F) {
                transform()->velocity.y = -transform()->velocity.x * 0.8F;
            } else if (other_type == entity::Type::slope_R_1x1 && transform()->velocity.x < 0.0F) {
                transform()->velocity.y = transform()->velocity.x * 0.8F;
            } else if (other_type == entity::Type::slope_L_2x1_0 || other_type == entity::Type::slope_L_2x1_1 && transform()->velocity.x > 0.0F) {
                transform()->velocity.y = -transform()->velocity.x * 0.75F * 0.8F;
            } else if (other_type == entity::Type::slope_R_2x1_0 || other_type == entity::Type::slope_R_2x1_1 && transform()->velocity.x < 0.0F) {
                transform()->velocity.y = transform()->velocity.x * 0.75F * 0.8F;
            }
            transform()->velocity.x *= 0.8F;
        } else {
            if (other_type == entity::Type::slope_L_1x1 && transform()->velocity.y > 0.0F) {
                transform()->velocity.x = -transform()->velocity.y * 0.8F;
                //transform()->position.x -= 1.0F;
            } else if (other_type == entity::Type::slope_R_1x1 && transform()->velocity.y > 0.0F) {
                transform()->velocity.x = transform()->velocity.y * 0.8F;
                //transform()->position.x += 1.0F;
            } else if (other_type == entity::Type::slope_L_2x1_0 || other_type == entity::Type::slope_L_2x1_1 && transform()->velocity.y > 0.0F) {
                transform()->velocity.x = -transform()->velocity.y * 1.2F * 0.8F;
                //transform()->position.x -= 1.0F;
            } else if (other_type == entity::Type::slope_R_2x1_0 || other_type == entity::Type::slope_R_2x1_1 && transform()->velocity.y > 0.0F) {
                transform()->velocity.x = transform()->velocity.y * 1.2F * 0.8F;
                //transform()->position.x += 1.0F;
            }
            transform()->velocity.y *= 0.8F;
        }
        return;
    }
    else if (other_type == entity::Type::brick) {
        /*if (transform()->velocity.y < 0.0F && our_rect.y < other_rect.h - 2.0F || transform()->velocity.y > 0.0F && our_rect.h > other_rect.y + 2.0F) {
            collide_x(our, other);
            return;
        }*/

        if (our_rect.h < other_rect.y + 4.0F && transform()->velocity.y > 0.0F) {
            m_is_on_ground = true;
            transform()->moved_velocity = other_velocity;            
            transform()->velocity.y = 0.0F;

            transform()->position.y -= overlap_y;
        }

        
        /*if (our_rect.h < other_rect.y + 2.0F && transform()->velocity.y > 0.0F) {
            transform()->velocity.y = 0.0F;
            m_is_on_ground = true;
        } else if (our_rect.y > other_rect.y - 2.0F && transform()->velocity.y < 0.0F) {
            transform()->velocity.y *= -0.9F;
        }*/
    }
    else if (other_type == entity::Type::bug) {
        if (other.owner->parent() == this || other.owner->is_dead() || other.owner->is_interacting()) return;

        /*if (our_rect.y > other_rect.h) {
            return;
        }*/

        if (our_rect.y >= other_rect.y) {
            if (other.owner->state() == entity::State::swim) {
                if (transform()->velocity.y < 0.0F) {
                    transform()->velocity.y *= -1.0F;
                }
                return;
            }
            Vec2F other_vel = other.owner->transform()->velocity + other.owner->transform()->moved_velocity;            
            other.owner->transform()->velocity.y = transform()->velocity.y / 4.0F - 3.0F;
            other.owner->transform()->position.y -= 8.0F;

            cF32 our_mid_x = our_rect.w - our_rect.x / 2.0F;
            cF32 other_mid_x = other_rect.w - other_rect.x / 2.0F;
            
            cF32 mid_diff_x = other_mid_x - our_mid_x;
            other.owner->transform()->velocity.x = (transform()->velocity.x + transform()->moved_velocity.x) + mid_diff_x / 4.0F;

            //Console::log("mid diff: ", , "\n");
            // 
            //transform()->velocity = vel / 2.0F;

            cF32 v = other.owner->transform()->velocity.y;

            //Console::log("entity::Bug::collide_y vel y: ", v, "\n");

            //hurt();
            Console::log("player hurt\n");

            return;
        }

        if (other_rect.y > our_rect.h - 2.0F - transform()->velocity.y && !m_is_on_ground) {
            transform()->position.y -= overlap_y;
            if ((other.owner->state() == entity::State::upended || other.owner->state() == entity::State::bounce) &&
                other.owner->time_left_interacting() == 0) {
                if (transform()->velocity.y < 0.0F) return;
                transform()->position.y -= overlap_y;
                transform()->max_velocity.y = 10.0F;
                transform()->velocity.y = -5.0F - transform()->velocity.y / 4.0F;
                m_is_down_thrusting ? reset_anim(m_jump_spin_anim_id) : reset_anim(m_jump_anim_id);
                m_is_down_thrusting = false;
                m_is_on_ground = false;
                m_is_on_slope = false;
                m_num_jumps = 1;
                m_time_left_bouncing = m_time_to_bounce;
                return;
            }
            transform()->moved_velocity = other_velocity;
            //transform()->velocity.x *= 0.5F;
            transform()->velocity.y = 0.0F;
            m_is_on_ground = true;
        }
    } else if (other_type == entity::Type::clip) {
        transform()->position.y -= overlap_y;        
        
        if (m_state == entity::State::swim && m_num_jumps == 0) {
            transform()->velocity.y = transform()->moved_velocity.y = 0.0F;
            return;
        }

        if (transform()->velocity.y > 0.0F && our_rect.y < other_rect.y) {
            m_is_on_ground = true;
            m_is_on_slope = false;
            transform()->moved_velocity.x = 0.0F;
            transform()->velocity.y = transform()->moved_velocity.y = 0.0F;
        } else {
            transform()->velocity.y = std::abs(transform()->velocity.y) * 0.75F;
            sound::Manager::get(m_bump_head_sound_id)->position({ position().x / 160.0F, position().y / 90.0F });
            sound::Manager::get(m_bump_head_sound_id)->play();
        }
    } else if (other_type == entity::Type::clip_duck) {
        if (m_state == entity::State::swim) {
            transform()->position.y -= overlap_y;
            transform()->velocity.y = transform()->moved_velocity.y = 0.0F;
            return;
        }

        if ((other_rect.y < our_rect.y && m_is_on_ground) || m_is_climbing_ledge) {
            m_time_left_rising = m_time_to_rise;
            m_time_left_ducking = m_time_to_duck;
            return;
        }
        if (m_time_left_rising > 0) {
            return;
        }
        transform()->position.y -= overlap_y;        

        if (transform()->velocity.y < 0.0F) {            
            transform()->velocity.y = std::abs(transform()->velocity.y) * 0.75F;
            sound::Manager::get(m_bump_head_sound_id)->position({ position().x / 160.0F, position().y / 90.0F });
            sound::Manager::get(m_bump_head_sound_id)->play();
            reset_anim(m_fall_anim_id);
        } else {
            transform()->velocity.y = transform()->moved_velocity.y = 0.0F;
            
            //if (our_rect.y + our_rect.h > other_rect.y + other_rect.h
                //&& (sprite()->is_leftward && other_rect.x + other_rect.w < our_rect.x) || (!sprite()->is_leftward && other_rect.x > our_rect.x + our_rect.w)
                //) {
            m_time_left_rising = m_time_to_rise;
            m_time_left_ducking = m_time_to_duck;

            if (our_rect.y < other_rect.y) {
                m_is_on_ground = true;
                m_is_on_slope = false;
            }
        }
    } else if (other_type == entity::Type::clip_D || other_type == entity::Type::clip_LD || other_type == entity::Type::clip_RD) {
        if (transform()->velocity.y > 0.0F) return;
        transform()->position.y -= overlap_y;
        if (m_state == entity::State::swim) {
            transform()->velocity.y = transform()->moved_velocity.y = 0.0F;
            return;
        }
        if (our_rect.y > other_rect.y) {
            m_is_on_ground = false;
            m_is_on_slope = false;

            transform()->velocity.y = std::abs(transform()->velocity.y) * 0.75F;
            sound::Manager::get(m_bump_head_sound_id)->play();
            reset_anim(m_fall_anim_id);
        }
    } else if (other_type == entity::Type::clip_L) {
    } else if (other_type == entity::Type::clip_R) {
    } else if (other_type == entity::Type::clip_ledge) {
        if (transform()->velocity.y < 0.0F && our_rect.y < other_rect.h - 4.0F ||
            transform()->velocity.y > 0.0F && our_rect.h > other_rect.y + 4.0F) {
            collide_x(our, other);
            return;
        }

        /*Console::log(" ", our_rect.y, " ", our_rect.h, "  ", other_rect.y, " ", other_rect.h, "\n");*/
        transform()->position.y -= overlap_y;
        if (m_state == entity::State::swim) {
            if (m_num_jumps == 0) {
                transform()->velocity.y = transform()->moved_velocity.y = 0.0F;
                return;
            }
        }

        if (our_rect.y < other_rect.y) {
            m_is_on_ground = true;
            m_is_on_slope = false;
            transform()->moved_velocity.x = 0.0F;
            transform()->velocity.y = transform()->moved_velocity.y = 0.0F;
        } else if (our_rect.h > other_rect.h) {
            transform()->velocity.y = std::abs(transform()->velocity.y) * 0.75F;
            sound::Manager::get(m_bump_head_sound_id)->position({ position().x / 160.0F, position().y / 90.0F });
            sound::Manager::get(m_bump_head_sound_id)->play();
        }
    } else if (other_type == entity::Type::clip_U || other_type == entity::Type::slope_U) {
        if (transform()->velocity.y < 0.0F) return;
        transform()->position.y -= overlap_y;
        
        transform()->moved_velocity.x = 0.0F;
        transform()->velocity.y = transform()->moved_velocity.y = 0.0F;
        m_is_on_ground = true;
        m_is_on_slope = other_type == entity::Type::slope_U;
    } else if (entity::is_conduit(other_type)) {
    } else if (other_type == entity::Type::frog) {
        if (our_rect.y < other_rect.y && transform()->velocity.y > 0.0F) {
            transform()->position.y -= overlap_y;

            if (m_is_down_thrusting) {
                reset_anim(m_jump_spin_anim_id);
                m_is_down_thrusting = false;
                m_is_on_ground = false;
                m_is_on_slope = false;
                m_num_jumps = 1;

                //transform()->velocity.y *= -0.9F;
            } else {
                m_is_on_ground = true;
                m_is_on_slope = false;
                transform()->velocity.y = 0.2F;
                transform()->moved_velocity = other_velocity;
            }
        } else {
            Console::log("player::Object::collide_y frog above\n");
        }

        //Console::log("other vx: ", other_velocity.x, " ", our.owner->transform()->moved_velocity.x, "\n");        

        //if (our_rect.y < other_rect.y) {
        //    transform()->position.y -= overlap_y;// / 4.0F;

        //    if (transform()->velocity.y >= 9.0F) {
        //        //bounce
        //        transform()->velocity.y *= -0.9F;
        //        transform()->moved_velocity.y = 0.0F;
        //        return;
        //    }
        //    transform()->velocity.y = 0.2F;
        //    

        //    transform()->moved_velocity = other_velocity;
        //    m_is_on_ground = true;
        //    m_is_on_slope = false;
        //}
        //else {
        //    //transform()->position.y -= overlap_y;
        //}
    } else if (other_type == entity::Type::level_L_0 || other_type == entity::Type::level_R_0) {
        collide_x(our, other);
    } else if (other_type == entity::Type::level_center) {
        collide_x(our, other);
    }
    else if (other_type == entity::Type::particle_brick) {
        if (our_rect.y < other_rect.y) {
            //transform()->position.y -= overlap_y;
            //transform()->velocity.y = 0.0F;
            //transform()->moved_velocity = other_velocity;
            //m_is_on_ground = true;
        }
    }
    else if (other_type == entity::Type::player) {
        return;
        transform()->position.y -= overlap_y;

        //Console::log("overlap_y: ", overlap_y, "\n");

        if (our_rect.y < other_rect.y) {
            m_is_on_ground = true;
            m_is_on_slope = false;
            //time_left_holding_jump = time_to_hold_jump;

            transform()->moved_velocity = other_velocity;
            transform()->velocity.y = 0.0F;
            //transform()->position.y -= 1.0F;

            //Console::log("velocity: ", velocity.x, " ", velocity.y, "\n");
            //transform()->velocity = { 0.0F, 0.0F };
            //transform()->velocity.x -= (transform()->velocity.x - velocity.x);
        } else {
            //object->is_on_ground = true;

            //object->transform()->moved_velocity = transform()->velocity;
            //if (!is_on_ground && transform()->velocity.y < 0.0F) {
            //    transform()->velocity.y *= -1.0F;
            //}
            //transform()->moved_velocity = other_velocity;
            //transform()->velocity.y = 0.0F;
        }


        //transform()->velocity = { 0.0F, 0.0F };


        //transform()->velocity.y = velocity.y;
        //transform()->velocity.y = 0.0F;
    }
    else if (other_type == entity::Type::slope_L_1x1) {
        //if (transform()->velocity.y < 0.0F) return;
        transform()->position.y -= overlap_y;

        if (m_state == entity::State::swim) {
            if (transform()->velocity.y > 0.0F && std::abs(transform()->velocity.x) < transform()->velocity.y) {                
                transform()->velocity.x = -transform()->velocity.y * 0.5F;
                transform()->velocity.y *= 0.95f;
            }
            return;
        }
        if (transform()->velocity.x > transform()->max_velocity.x * 0.7F) {
            transform()->velocity.x = transform()->max_velocity.x * 0.7F;
        }
        //if (transform()->velocity.x < 0.0F) {
            transform()->velocity.y = -transform()->velocity.x;
        //}
        
        //transform()->velocity.y = 0.0F;
        transform()->moved_velocity.x = 0.0F;
        transform()->moved_velocity.y = 0.0F;        
        
        m_is_on_ground = true;
        m_is_on_slope = true;
    } else if (other_type == entity::Type::slope_R_1x1) {
        //if (transform()->velocity.y < 0.0F) return;
        transform()->position.y -= overlap_y;
        
        if (m_state == entity::State::swim) {
            if (transform()->velocity.y > 0.0F && std::abs(transform()->velocity.x) < transform()->velocity.y) {                
                transform()->velocity.x = transform()->velocity.y * 0.5F;
                transform()->velocity.y *= 0.95f;
            }
            return;
        }
        if (transform()->velocity.x < -transform()->max_velocity.x * 0.7F) {
            transform()->velocity.x = -transform()->max_velocity.x * 0.7F;
        }
        //if (transform()->velocity.x > 0.0F) {
            transform()->velocity.y = transform()->velocity.x;
        //}
        //transform()->velocity.y = 0.0F;
        transform()->moved_velocity.x = 0.0F;
        transform()->moved_velocity.y = 0.0F;
                
        m_is_on_ground = true;
        m_is_on_slope = true;        
    } else if (other_type == entity::Type::slope_L_2x1_0 || other_type == entity::Type::slope_L_2x1_1) {
        //if (transform()->velocity.y < 0.0F) return;
        transform()->position.y -= overlap_y;

        if (m_state == entity::State::swim) {
            if (transform()->velocity.y > 0.0F && std::abs(transform()->velocity.x) < transform()->velocity.y) {
                transform()->velocity.x = transform()->velocity.y * -0.5F;
                transform()->velocity.y *= 0.95f;
            }
            return;
        }
        if (transform()->velocity.x > transform()->max_velocity.x * 0.9F) {
            transform()->velocity.x = transform()->max_velocity.x * 0.9F;
        }
        //if (transform()->velocity.x < 0.0F) {
            transform()->velocity.y = transform()->velocity.x / -2.0F;
        //}
        //transform()->velocity.y = 0.0F;
        transform()->moved_velocity.x = 0.0F;
        transform()->moved_velocity.y = 0.0F;
                
        m_is_on_ground = true;
        m_is_on_slope = true;
    } else if (other_type == entity::Type::slope_R_2x1_0 || other_type == entity::Type::slope_R_2x1_1) {
        //if (transform()->velocity.y < 0.0F) return;
        transform()->position.y -= overlap_y;

        if (m_state == entity::State::swim) {
            if (transform()->velocity.y > 0.0F && std::abs(transform()->velocity.x) < transform()->velocity.y) {
                transform()->velocity.x = transform()->velocity.y * 0.5F;
                transform()->velocity.y *= 0.95f;
            }
            return;
        }
        if (transform()->velocity.x < -transform()->max_velocity.x * 0.9F) {
            transform()->velocity.x = -transform()->max_velocity.x * 0.9F;
        }
        //if (transform()->velocity.x > 0.0F) {
            transform()->velocity.y = transform()->velocity.x / 2.0F;
        //}
        //transform()->velocity.y = 0.0F;
        transform()->moved_velocity.x = 0.0F;
        transform()->moved_velocity.y = 0.0F;
                
        m_is_on_ground = true;
        m_is_on_slope = true;        
    } else if (other_type == entity::Type::spring_U) {
        if (transform()->velocity.y < 0.0F) return;
        transform()->position.y -= overlap_y;
        transform()->max_velocity.y = 10.0F;
        transform()->velocity.y = -5.0F - transform()->velocity.y / 4.0F;
        m_is_down_thrusting ? reset_anim(m_jump_spin_anim_id) : reset_anim(m_jump_anim_id);
        m_is_down_thrusting = false;
        m_is_on_ground = false;
        m_is_on_slope = false;
        m_num_jumps = 1;
        m_time_left_bouncing = m_time_to_bounce;
    } else if (other_type == entity::Type::train) {
        if (other.owner->time_left_alive() > 0) {
            m_is_on_ledge = false;
        }
        //Console::log("hello train ", aabb::name_as_string(other_aabb_name), "\n");
        if (transform()->velocity.y < 0.0F || other_aabb_name == aabb::Name::bone || 
            //our_rect.h > other_rect.h
            //||
            transform()->position.y + 20 > other_rect.y
            ) return;
        
        if (other.owner->transform()) {
            transform()->moved_velocity = other.owner->transform()->velocity;
            // move in so we don't fly off at turns
            //if (transform()->moved_velocity.y < 0.0F) {
                //transform()->velocity.y = -transform()->moved_velocity.y;
                transform()->velocity.y = 1.0F;
                transform()->position.y -= overlap_y / 6.0F;
            //} else {
                //transform()->velocity.y = 0.0F;
                //transform()->position.y -= overlap_y;
            //}
            m_is_on_ground = true;
        }
    } else if (entity::is_water_line(other_type)) {
        //next_state(entity::State::swim);
        bool is_to_splash = false,
             is_upwards   = false;

        if (transform()->velocity.y > 0.0F && our_rect.h > other_rect.y + 4 && !m_is_on_ground) {
            if (!sound::Manager::get(m_water_enter_sound_id)->is_playing() && !(m_state == entity::State::swim && m_num_jumps == 0)) {
                sound::Manager::get(m_water_enter_sound_id)->position({ position().x / 160.0F, position().y / 90.0F });
                sound::Manager::get(m_water_enter_sound_id)->play();
                is_to_splash = true;
            }
            m_next_state = entity::State::swim;
            //m_prev_state = m_state;
            //m_next_state = entity::State::swim;
            m_num_jumps = 0;
            m_water_line_y = other.owner->position_on_level().y;

        } else if (transform()->velocity.y < 0.0F && m_state == entity::State::swim) {
            //Console::log("is_pressed(key_jump): ", is_pressed(key_jump), "\n");
            if (transform()->velocity.y > -1.5F || is_pressed(key_jump)) {
                transform()->velocity.y = transform()->velocity.y * -0.5F;
                m_time_in_state = 0;
            }
            else if (transform()->velocity.y <= -1.5F) {
                is_upwards = true;
                if (!sound::Manager::get(m_water_exit_sound_id)->is_playing()) {
                    sound::Manager::get(m_water_exit_sound_id)->position({ position().x / 160.0F, position().y / 90.0F });
                    sound::Manager::get(m_water_exit_sound_id)->play();
                    is_to_splash = true;
                }
                if (m_num_jumps == 0) {
                    transform()->velocity.y *= 1.5F;
                    transform()->velocity.y += other.owner->transform()->velocity.y * 0.5F;
                }
                if (transform()->velocity.y > -3.0F) {
                    transform()->velocity.y = -3.0F;
                }
                m_num_jumps = 1;
                m_time_in_state = 0;
                m_is_on_ground = false;

                /*if (transform()->velocity.y <= -4.0F) {
                    transform()->velocity.y = -4.0F;
                }*/
                //next_state(entity::State::run);

                //m_prev_state = m_state;
                //m_next_state = entity::State::run;
            }
        }
        if (is_to_splash) {
            Vec2F pos = Vec2F{ our_rect.x, other_rect.y };
            spawn_fan(225.0F, 315.0F, 8,
                      particle::Type::drop, pos + Vec2F{0.0F, is_upwards ? -16.0F : 0.0F},
                      Vec2F{transform()->velocity.x * 0.5F, -std::abs(transform()->velocity.y * 0.1F)},
                      2.0F);
        }

    }
};