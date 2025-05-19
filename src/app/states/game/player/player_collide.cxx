module;
#include <sstream>
module player;
import console;

void player::Object::collide_x(sprite::Infoc our, sprite::Infoc other) {
    if (!transform()) return;

    f32c overlap_x = our.rect.x < other.rect.x ? our.rect.w - other.rect.x : -(other.rect.w - our.rect.x);

    //Console::log(sprite::type_to_string(other.type), " overlap_x: ", overlap_x, "\n");

    if (other.type == sprite::Type::clip) {
        transform()->position.x -= overlap_x;
        transform()->velocity.x = 0.0f;
        transform()->moved_velocity.x = 0.0f;
    } else if (other.type == sprite::Type::clip_U || other.type == sprite::Type::slope_U) {
    } else if (other.type == sprite::Type::clip_D) {
    } else if (other.type == sprite::Type::clip_ledge) {
        if (m_is_on_ledge) return;
        transform()->position.x -= overlap_x;
        transform()->velocity.x = 0.0f;
        transform()->moved_velocity.x = 0.0f;
        if (is_on_ground) return;

        m_is_wall_to_left = other.rect.x < our.rect.x;
        if (( m_is_wall_to_left && transform()->velocity.x + transform()->moved_velocity.x > 0.0f) ||
            (!m_is_wall_to_left && transform()->velocity.x + transform()->moved_velocity.x < 0.0f)) return;
                
        if (m_time_left_jumping_wall == 0 && !m_is_hovering &&
            transform()->velocity.y > 0.0f &&
            (m_is_wall_to_left && is_pressed(key_left) || !m_is_wall_to_left && is_pressed(key_right))) {
            m_is_sliding_wall = true;
            if (transform()->velocity.y > 1.0f) {
                transform()->velocity.y = 1.0f;
            }            
            sprite()->is_leftward = !m_is_wall_to_left;
        }
        if (is_pressed(key_jump) && !m_is_locked_jump && m_time_left_jumping_wall <= 0.0f) {
            m_time_left_jumping_wall = m_time_to_jump_wall;
            m_num_jumps = 0;
        }
        if (transform()->velocity.y > 0.0f && !is_on_ground) {
            if ( m_is_wall_to_left && is_pressed(key_left) ||
                !m_is_wall_to_left && is_pressed(key_right)) {
                if (!m_is_on_ledge && our.rect.y <= other.rect.y - 4 && our.rect.y >= other.rect.y - 2 - 4) {
                    m_is_on_ledge = true;
                    m_is_sliding_wall = false;
                    transform()->position.y = other.rect.y - 8;
                    transform()->velocity.y = 0.0f;

                    sound::Set::at(m_ledge_grab_sound_id)->position({ position().x / 160.0f, position().y / 90.0f });
                    sound::Set::at(m_ledge_grab_sound_id)->play();
                }
            }
        }        
    } else if (other.type == sprite::Type::clip_L || other.type == sprite::Type::clip_D_L) {
        if (transform()->velocity.x + transform()->moved_velocity.x <= 0.0f || m_is_on_ledge) return;
        transform()->position.x -= overlap_x;
        transform()->velocity.x = 0.0f;
        transform()->moved_velocity.x = 0.0f;
        m_is_wall_to_left = false;        
        if (is_on_ground) return;
               
        if (transform()->velocity.y > 0.0f && is_pressed(key_right) && m_time_left_jumping_wall == 0 && !m_is_hovering) {
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

    } else if (other.type == sprite::Type::clip_R || other.type == sprite::Type::clip_D_R) {
        if (transform()->velocity.x + transform()->moved_velocity.x >= 0.0f || m_is_on_ledge) return;
        transform()->position.x -= overlap_x;
        transform()->velocity.x = 0.0f;
        transform()->moved_velocity.x = 0.0f;
        m_is_wall_to_left = true;
        if (is_on_ground) return;
        if (transform()->velocity.y > 0.0f && is_pressed(key_left) && m_time_left_jumping_wall == 0 && !m_is_hovering) {
            m_is_sliding_wall = true;
            sprite()->is_leftward = false;
            if (transform()->velocity.y > 1.0f) {
                transform()->velocity.y = 1.0f;
            }            
            //set_anim(m_slide_wall_anim_id);
        }
        if (is_pressed(key_jump) && !m_is_locked_jump && m_time_left_jumping_wall <= 0.0f) {
            m_time_left_jumping_wall = m_time_to_jump_wall;
            m_num_jumps = 0;
        }
    } else if (other.type == sprite::Type::level_start) {
    } else if (other.type == sprite::Type::slope_L_1x1) {
        //if (transform()->velocity.x < 0.0f) return;
        //transform()->position.y -= overlap_x;
        transform()->velocity.y = -transform()->velocity.x;
        if (transform()->velocity.x > transform()->velocity_limit.x * 0.7f) {
            transform()->velocity.x = transform()->velocity_limit.x * 0.7f;
        }
        is_on_ground = true;
        is_on_slope = true;
    } else if (other.type == sprite::Type::slope_L_2x1_0 || other.type == sprite::Type::slope_L_2x1_1) {
        //if (transform()->velocity.x < 0.0f) return;
        //transform()->position.y -= overlap_x;
        transform()->velocity.y = -transform()->velocity.x / 2.0f;
        if (transform()->velocity.x > transform()->velocity_limit.x * 0.9f) {
            transform()->velocity.x = transform()->velocity_limit.x * 0.9f;
        }
        is_on_ground = true;
        is_on_slope = true;
    } else if (other.type == sprite::Type::slope_R_1x1) {
        //if (transform()->velocity.x > 0.0f) return;
        //transform()->position.y -= overlap_x;
        //transform()->position.x -= overlap_x;
        //transform()->velocity.y = 0.0f;
        transform()->velocity.y = transform()->velocity.x;
        if (transform()->velocity.x < -transform()->velocity_limit.x * 0.7f) {
            transform()->velocity.x = -transform()->velocity_limit.x * 0.7f;
        }
        is_on_ground = true;
        is_on_slope = true;
    } else if (other.type == sprite::Type::slope_R_2x1_0 || other.type == sprite::Type::slope_R_2x1_1) {
        //if (transform()->velocity.x > 0.0f) return;
        //transform()->position.y -= overlap_x;
        //transform()->position.x -= overlap_x;
        transform()->velocity.y = transform()->velocity.x / 2.0f;
        if (transform()->velocity.x < -transform()->velocity_limit.x * 0.9f) {
            transform()->velocity.x = -transform()->velocity_limit.x * 0.9f;
        }
        is_on_ground = true;
        is_on_slope = true;
    } else if (other.type == sprite::Type::player) {
        transform()->position.x -= overlap_x;
        //transform()->velocity.x = 0.0f;

        //transform()->velocity.x = transform()->velocity.x * 0.5f + velocity.x * 0.5f;
        
        transform()->velocity.x = transform()->velocity.x * 0.75f + other.velocity.x * 0.25f;
        //transform()->velocity.y = transform()->velocity.y * 0.5f + velocity.y * 0.5f;
    }
};
void player::Object::collide_y(sprite::Infoc our, sprite::Infoc other) {
    if (!transform()) return;

    f32c overlap_y = our.rect.y < other.rect.y ? our.rect.h - other.rect.y : -(other.rect.h - our.rect.y);

    
    //Console::log(sprite::type_to_string(other.type), " overlap_y: ", overlap_y, "\n");
  
    if (other.type == sprite::Type::clip) {
        transform()->position.y -= overlap_y;
        transform()->velocity.y = 0.0f;
        is_on_ground = true;
        is_on_slope = false;
    } else if (other.type == sprite::Type::clip_D || other.type == sprite::Type::clip_D_L || other.type == sprite::Type::clip_D_R) {
        if (transform()->velocity.y > 0.0f) return;
        transform()->position.y -= overlap_y;
        
        is_on_ground = false;
        is_on_slope = false;

        transform()->velocity.y = std::abs(transform()->velocity.y) * 0.75f;
        sound::Set::at(m_bump_head_sound_id)->play();
    } else if (other.type == sprite::Type::clip_L) {
    } else if (other.type == sprite::Type::clip_R) {
    } else if (other.type == sprite::Type::clip_ledge) {
        //Console::log("ledge pos: ", other.rect.x, " ", other.rect.y, "\n");
        /*Console::log(" ", our.rect.y, " ", our.rect.h, "  ", other.rect.y, " ", other.rect.h, "\n");*/
        transform()->position.y -= overlap_y;

        if (our.rect.y < other.rect.y) {
            is_on_ground = true;
            is_on_slope = false;
            transform()->velocity.y = 0.0f;
        } else {            
            transform()->velocity.y = std::abs(transform()->velocity.y) * 0.75f;
            sound::Set::at(m_bump_head_sound_id)->play();
        }
    } else if (other.type == sprite::Type::clip_U || other.type == sprite::Type::slope_U) {
        if (transform()->velocity.y < 0.0f) return;
        transform()->position.y -= overlap_y;
        
        transform()->velocity.y = 0.0f;
        transform()->moved_velocity.y = 0.0f;
        is_on_ground = true;
        is_on_slope = other.type == sprite::Type::slope_U;
    } else if (other.type == sprite::Type::level_start) {
    } else if (other.type == sprite::Type::slope_L_1x1) {
        //if (transform()->velocity.y < 0.0f) return;
        transform()->position.y -= overlap_y;

        transform()->velocity.y = 0.0f;
        transform()->moved_velocity.y = 0.0f;
        //if (transform()->velocity.x < 0.0f) {
            transform()->velocity.y = -transform()->velocity.x;
        //}
        is_on_ground = true;
        is_on_slope = true;
        if (transform()->velocity.x > transform()->velocity_limit.x * 0.7f) {
            transform()->velocity.x = transform()->velocity_limit.x * 0.7f;
        }
    } else if (other.type == sprite::Type::slope_L_2x1_0 || other.type == sprite::Type::slope_L_2x1_1) {
        //if (transform()->velocity.y < 0.0f) return;
        transform()->position.y -= overlap_y;

        transform()->velocity.y = 0.0f;
        transform()->moved_velocity.y = 0.0f;
        //if (transform()->velocity.x < 0.0f) {
            transform()->velocity.y = transform()->velocity.x / -2.0f;
        //}
        is_on_ground = true;
        is_on_slope = true;
        if (transform()->velocity.x > transform()->velocity_limit.x * 0.9f) {
            transform()->velocity.x = transform()->velocity_limit.x * 0.9f;
        }
    } else if (other.type == sprite::Type::slope_R_1x1) {
        //if (transform()->velocity.y < 0.0f) return;
        transform()->position.y -= overlap_y;

        transform()->velocity.y = 0.0f;
        transform()->moved_velocity.y = 0.0f;
        //if (transform()->velocity.x > 0.0f) {
            transform()->velocity.y = transform()->velocity.x;
        //}
        is_on_ground = true;
        is_on_slope = true;
        if (transform()->velocity.x < -transform()->velocity_limit.x * 0.7f) {
            transform()->velocity.x = -transform()->velocity_limit.x * 0.7f;
        }
    } else if (other.type == sprite::Type::slope_R_2x1_0 || other.type == sprite::Type::slope_R_2x1_1) {
        //if (transform()->velocity.y < 0.0f) return;
        transform()->position.y -= overlap_y;

        transform()->velocity.y = 0.0f;
        transform()->moved_velocity.y = 0.0f;
        if (transform()->velocity.x > 0.0f) {
            transform()->velocity.y = transform()->velocity.x / 2.0f;
        }
        is_on_ground = true;
        is_on_slope = true;
        if (transform()->velocity.x < -transform()->velocity_limit.x * 0.9f) {
            transform()->velocity.x = -transform()->velocity_limit.x * 0.9f;
        }
    } else if (other.type == sprite::Type::player) {
        transform()->position.y -= overlap_y;

        //Console::log("overlap_y: ", overlap_y, "\n");        

        if (our.rect.y < other.rect.y) {
            is_on_ground = true;
            is_on_slope = false;
            //time_left_holding_jump = time_to_hold_jump;

            transform()->moved_velocity = other.velocity;
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
            //transform()->moved_velocity = other.velocity;
            //transform()->velocity.y = 0.0f;
        }


        //transform()->velocity = { 0.0f, 0.0f };

        
        //transform()->velocity.y = velocity.y;
        //transform()->velocity.y = 0.0f;
    }
};