module player;

void Player::update() {
    if (!is_all_valid()) return;

    //if      (transform()->velocity.x < 0.0f)    transform()->velocity.x += 0.01f;
    //else if (transform()->velocity.x > 0.0f)    transform()->velocity.x -= 0.01f;

    //Console::log("sprite offset: ", sprite()->offset.x, " ", sprite()->offset.y, "\n");

    transform()->velocity.y += 0.2f;
    transform()->deceleration.x = is_on_ground ? 0.1f : 0.0f;
    transform()->acceleration.x = is_on_ground ? 0.2f : 0.1f;

    

    anim::Set::at(m_current_anim_id)->update();
    sprite()->source_rect.x = anim::Set::at(m_current_anim_id)->source.x;
    sprite()->source_rect.y = anim::Set::at(m_current_anim_id)->source.y;


    
    if (anim::Set::at(m_jump_spin_anim_id)->source.x >= anim::Set::at(m_jump_spin_anim_id)->source.w * 4) {
        anim::Set::at(m_jump_spin_anim_id)->first_frame = 4;
    } else {
        anim::Set::at(m_jump_spin_anim_id)->first_frame = 0;
    }
    
    if (m_is_sliding_wall) {
        m_num_jumps = 1;
        set_anim(m_slide_wall_anim_id);
        //Console::log("sliding wall\n");
        is_on_ground = false;
    }

    if (is_on_ground || m_is_on_ledge) {
        if (m_time_left_jump_again > 0) {
            m_time_left_jump_again -= 1;
        } else {
            m_num_jumps = 0;
        }
        if (m_num_jumps > 2) {
            m_num_jumps = 0;
        }
        if (is_pressed(key_sprint)) {
            transform()->velocity_limit.x = m_start_velocity_limit.x * 1.2f;
        } else {
            transform()->velocity_limit.x = m_start_velocity_limit.x;
        }
    }

    if (transform()->velocity.y > 0.0f) {
        m_is_climbing_ledge = false;
    }


    if (transform()->velocity.y >= (0.2f + is_on_slope ? transform()->velocity_limit.y : 0.0f)) {
        //if (transform()->velocity.y >= 1.0f && !is_on_slope) {
        is_on_ground = false;
        is_on_slope = false;        
    }


    if (!is_on_ground && !is_on_slope && !m_is_sliding_wall && !m_is_on_ledge && !m_is_climbing_ledge && m_num_jumps <= 2) {
        if (transform()->velocity.y < 0.0f) {
            reset_anim(m_jump_anim_id);
        } else if (transform()->velocity.y >= 0.0f) {
            reset_anim(m_fall_anim_id);
            anim::Set::at(m_fall_anim_id)->speed = transform()->velocity.y / 10.0f;
        }
    }
    //if (is_sliding_wall) {
        //set_anim(m_slide_wall_anim_id);
    //}

    if (is_on_ground &&
        !is_pressed(key_left) && !is_pressed(key_right)
        //transform()->velocity.x >= -transform()->acceleration.x && transform()->velocity.x <= transform()->acceleration.x
        ) {
        set_anim(m_idle_anim_id);
    }

    if (m_is_on_ledge) {
        is_on_ground = false;
        reset_anim(m_ledge_grab_anim_id);

        transform()->deceleration.x = 0.0f;
        transform()->acceleration.x = 0.0f;
        transform()->velocity.x = 0.0f;
        transform()->velocity.y = 0.0f;

        if (is_pressed(key_up)) {
            release(key_up);
            m_is_on_ledge = false;
            m_is_climbing_ledge = true;
            transform()->velocity.y = -3.5f;
            reset_anim(m_ledge_climb_anim_id);
            ++m_num_jumps;
            if (m_num_jumps > 2) {
                m_num_jumps = 2;
            }
            m_time_left_jump_again = m_time_to_jump_again;
        }
        if (is_pressed(key_down)) {
            release(key_down);
            m_is_on_ledge = false;
        }
        if (is_pressed(key_jump)) {
            m_time_left_jumping_wall = m_time_to_jump_wall;
            m_is_on_ledge = false;
        }
    }

    /*if (id == 0) {
        Console::log("position: ", transform()->position.x, " ", transform()->position.y, "\n");
    }*/


    if (m_time_left_jumping_wall > 0) {
        //set_anim(m_jump_anim_id);

        transform()->velocity = { 0.0f, 0.0f };
        --m_time_left_jumping_wall;
        if (is_pressed(key_left)) {
            m_is_to_wall_jump_left = true;
        }
        if (is_pressed(key_right)) {
            m_is_to_wall_jump_left = false;
        }
        if (m_time_left_jumping_wall == 0) {
            if (m_is_to_wall_jump_left) {
                transform()->velocity.x = m_is_wall_to_left ? 1.0f : -1.5f;
            }
            else {
                transform()->velocity.x = m_is_wall_to_left ? 1.5f : -1.0f;
            }
            //Console::log("velocity.y: ", transform()->velocity.y, "\n");
            is_on_ground = true;
            m_is_locked_jump = false;
            m_is_on_ledge = false;
        }
    }


    if (is_pressed(key_jump)) {
        if (is_on_ground && !m_is_locked_jump) {
            ++m_num_jumps;
            m_time_left_jump_again = m_time_to_jump_again;
            m_is_locked_jump = true;
            is_on_ground = false;
            is_on_slope = false;            
            m_time_left_holding_jump = m_time_to_hold_jump;
            //transform()->velocity += transform()->moved_velocity;
            //transform()->moved_velocity = 0.0f;
            transform()->position.y -= 2.0f;
            
            if (m_num_jumps > 2) {                
                transform()->velocity.y = -3.0f;
                reset_anim(m_jump_spin_anim_id);
            } else {
                transform()->velocity.y = -2.0f;
            }
        } else {
            if (m_time_left_holding_jump > 0 && transform()->velocity.y < 0.0f) {
                --m_time_left_holding_jump;
                transform()->velocity.y -= 0.3f;
                //Console::log("Player holding jump: ", m_time_left_holding_jump, "\n");
            } else {
                //release(key_jump);
                m_time_left_holding_jump = 0;
            }
        }
    } else {
        m_is_locked_jump = false;
    }

    


    if (is_pressed(key_up)) {
        //release(key_up);
        //transform()->velocity.y -= transform()->acceleration.y;
    }
    if (is_pressed(key_down)) {
        //release(key_down);
        //transform()->velocity.y += transform()->acceleration.y;
    }

    if (m_current_anim_id == m_run_anim_id || m_current_anim_id == m_walk_anim_id) {
        //Console::log("vel: ", transform()->velocity.x, " ", anim::Set::at(m_current_anim_id), "\n");
        if (transform()->velocity.x < 0.0f) {
            anim::Set::at(m_current_anim_id)->speed = transform()->velocity.x / -5.0f;
        } else {
            anim::Set::at(m_current_anim_id)->speed = transform()->velocity.x / 5.0f;
        }
        if (anim::Set::at(m_current_anim_id)->speed < 0.2f) {
            anim::Set::at(m_current_anim_id)->speed = 0.2f;
        }
    }

    if (is_pressed(key_left)) {
        if (!m_is_sliding_wall && !m_is_on_ledge && m_time_left_jumping_wall == 0) {
            sprite()->is_leftward = true;
        }       
        if (is_on_ground) {
            if (transform()->velocity.x != 0.0f) {
                is_pressed(key_sprint) ? set_anim(m_run_anim_id) : set_anim(m_walk_anim_id);
            } else {
                set_anim(m_idle_anim_id);
            }
        }
        
        //release(key_left);    
        transform()->velocity.x -= transform()->acceleration.x;

        
        //Console::log("anim source.x: ", anim::Set::at(m_current_anim_id)->source.x, "\n");
    }
    if (is_pressed(key_right)) {
        if (!m_is_sliding_wall && !m_is_on_ledge && m_time_left_jumping_wall == 0) {
            sprite()->is_leftward = false;
        }
        if (is_on_ground) {
            if (transform()->velocity.x != 0.0f) {
                is_pressed(key_sprint) ? set_anim(m_run_anim_id) : set_anim(m_walk_anim_id);
            } else {
                set_anim(m_idle_anim_id);
            }
        }
        //release(key_right);
        //Console::log("input ", input_id, " pressed right\n");
        transform()->velocity.x += transform()->acceleration.x;        
    }

    m_is_sliding_wall = false;
}