module player;
import console;
import collider;

void player::Object::update_run() {
    if (m_prev_state != State::run) {
        m_prev_state = State::run;
        m_next_state = State::run;
        Console::log("player::Object::update_run\n");
    }


    if (m_time_left_until_down_thrust > 0) {
        --m_time_left_until_down_thrust;
        transform()->velocity = { 0.0f, 0.0f };
        if (m_time_left_until_down_thrust > 0) {
            return;
        } else if (!m_is_down_thrusting) {
            m_is_down_thrusting = true;            
            //anim::Set::at(m_down_thrust_anim_id)->speed = 0.5f;
            transform()->velocity_limit.y = 10.0f;
            transform()->velocity.y = 10.0f;
        }
    }

    if (m_is_down_thrusting) {        
        if (is_on_ground) {
            m_is_down_thrusting = false;
            m_time_left_hitting_ground = m_time_to_hit_ground;
            reset_anim(m_hit_ground_anim_id);            
            sound::Set::at(m_hit_ground_sound_id)->position({ position().x / 160.0f, position().y / 90.0f });
            sound::Set::at(m_hit_ground_sound_id)->play();
        } else {
            set_anim(m_down_thrust_anim_id);
            return;
        }
    }
    if (m_time_left_hitting_ground > 0) {
        --m_time_left_hitting_ground;
        if (m_time_left_hitting_ground > 0) {
            transform()->velocity.x = 0.0f;            
            aabb::Set::at(m_melee_left_collider_id)->is_inactive(false);
            aabb::Set::at(m_melee_right_collider_id)->is_inactive(false);
            return;
        } else {
            aabb::Set::at(m_melee_left_collider_id)->is_inactive(true);
            aabb::Set::at(m_melee_right_collider_id)->is_inactive(true);
        }
    }


    if (is_on_ground && !m_is_ducking && m_time_left_rising == 0 &&
        !is_pressed(key_left) && !is_pressed(key_right) && m_time_left_melee == 0
        //transform()->velocity.x >= -transform()->acceleration.x && transform()->velocity.x <= transform()->acceleration.x
        ) {
        set_anim(m_idle_anim_id);
    }

    m_is_sliding_ground = false;
    if (is_on_ground && m_time_left_melee == 0 && m_time_left_skidding == 0) {
        if (transform()->velocity.x >= -transform()->acceleration.x && transform()->velocity.x <= transform()->acceleration.x) {
            if (!m_is_ducking && m_time_left_rising == 0) {
                set_anim(m_idle_anim_id);
            }
            if (m_is_ducking) {
                /*if (m_current_anim_id == m_idle_anim_id || m_current_anim_id == m_run_anim_id || m_current_anim_id == m_walk_anim_id) {
                    anim::Set::at(m_duck_anim_id)->source.x = 0;
                }*/
                /*if (m_current_anim_id == m_crawl_anim_id) {
                    anim::Set::at(m_duck_anim_id)->source.w * 4;
                }*/
                
                /*if (m_current_anim_id == m_idle_anim_id || m_current_anim_id == m_walk_anim_id || m_current_anim_id == m_run_anim_id) {
                    reset_anim(m_duck_anim_id);
                } else {*/
                    set_anim(m_duck_anim_id);
                //}
                
            }
        } else {
            if (m_is_ducking) {
                if (m_time_left_rising == m_time_to_rise) {                    
                    if (transform()->velocity.x < transform()->velocity_limit.x * -0.7f ||
                        transform()->velocity.x > transform()->velocity_limit.x *  0.7f) {
                        m_is_sliding_ground = true;
                    }

                    if (m_is_sliding_ground) {
                        set_anim(m_slide_ground_anim_id);
                    } else {
                        anim::Set::at(m_duck_anim_id)->source.x = anim::Set::at(m_duck_anim_id)->source.w * 4;
                        set_anim(m_crawl_anim_id);
                        if (transform()->velocity.x < transform()->velocity_limit.x * -0.5f) {
                            transform()->velocity.x = transform()->velocity_limit.x * -0.5f;
                        } else if (transform()->velocity.x > transform()->velocity_limit.x * 0.5f) {
                            transform()->velocity.x = transform()->velocity_limit.x * 0.5f;
                        }
                    }
                                        
                }
            } else {
                anim::Set::at(m_slide_ground_anim_id)->source.x = 0;
                //Console::log("run walk\n");
                is_pressed(key_sprint) ? set_anim(m_run_anim_id) : set_anim(m_walk_anim_id);
            }            
        }
    }

    if (is_pressed(key_left)) {
        if (!m_is_sliding_ground && (transform()->velocity.x > -m_ground_velocity_limit.x * is_pressed(key_sprint) ? 1.2f : 1.0f)) {
            transform()->velocity.x -= transform()->acceleration.x;
        }        
        //Console::log("anim source.x: ", anim::Set::at(m_current_anim_id)->source.x, "\n");

        if (is_on_ground && !m_is_ducking) {
            if (transform()->velocity.x >= 0.0f) {
                m_time_left_skidding = m_time_to_skid;
                if (!sprite()->is_leftward) {
                    anim::Set::at(m_skid_anim_id)->source.x = 0;
                }
            }
            if (transform()->velocity.x < -2.0f) {
                m_time_left_skidding = 0.0f;
            }           
        }
        if (!m_is_sliding_wall && !m_is_on_ledge && m_time_left_jumping_wall == 0) {
            sprite()->is_leftward = true;
        }
    }
    if (is_pressed(key_right)) {
        if (!m_is_sliding_ground && (transform()->velocity.x < m_ground_velocity_limit.x * is_pressed(key_sprint) ? 1.2f : 1.0f)) {
            transform()->velocity.x += transform()->acceleration.x;
        }
        if (is_on_ground && !m_is_ducking) {
            if (transform()->velocity.x <= 0.0f) {
                m_time_left_skidding = m_time_to_skid;
                if (sprite()->is_leftward) {
                    anim::Set::at(m_skid_anim_id)->source.x = 0;
                }
            }
            if (transform()->velocity.x > 2.0f) {
                m_time_left_skidding = 0.0f;
            }            
        }
        if (!m_is_sliding_wall && !m_is_on_ledge && m_time_left_jumping_wall == 0) {
            sprite()->is_leftward = false;
        }
    }
    if (m_time_left_skidding > 0) {
        if (m_time_left_skidding == m_time_to_skid) {
            sound::Set::at(m_skid_sound_id)->position({ position().x / 160.0f, position().y / 90.0f });
            sound::Set::at(m_skid_sound_id)->play();
        }

        --m_time_left_skidding;
        set_anim(m_skid_anim_id);
        //Console::log("player::Object::update_run m_time_left_skidding: ", m_time_left_skidding, "\n");
    } else {
        anim::Set::at(m_skid_anim_id)->source.x = 0;
    }

    if (is_pressed(key_melee) && !m_is_ducking && !m_is_locked_jump) {
        if (!m_is_locked_melee) {
            m_is_locked_melee = true;
            m_time_left_melee = m_time_to_melee;            
            reset_anim(m_melee_anim_id);

            if (m_melee_sound_id != -1) {
                //m_sounds.at(m_melee_sound_id)->position({ position().x / 160.0f, position().y / 90.0f });
                //Console::log("player::Object::update_run melee sound position: ", m_sounds.at(m_melee_sound_id)->position().x, " ", m_sounds.at(m_melee_sound_id)->position().y, "\n");
                //m_sounds.at(m_melee_sound_id)->play();
                sound::Set::at(m_melee_sound_id)->position({ position().x / 160.0f, position().y / 90.0f });
                sound::Set::at(m_melee_sound_id)->play();
            }
        }
    } else {
        m_is_locked_melee = false;
    }

    if (m_time_left_melee > 0) {
        --m_time_left_melee;
        
        //aabb::Set::at(m_melee_collider_ids.front())->is_inactive(!sprite()->is_leftward || m_time_left_melee == 0);
        //aabb::Set::at(m_melee_collider_ids.back())->is_inactive(sprite()->is_leftward || m_time_left_melee == 0);

        aabb::Set::at(m_melee_left_collider_id)->is_inactive(!sprite()->is_leftward || m_time_left_melee == 0);
        aabb::Set::at(m_melee_right_collider_id)->is_inactive(sprite()->is_leftward || m_time_left_melee == 0);
                
        if (is_on_ground) {
            transform()->velocity.x *= 0.9f;
        }
    }


    transform::Set::at(m_transform_id)->acceleration = { 0.2f, 0.2f };
    transform::Set::at(m_transform_id)->deceleration = { 0.04f, 0.04f };

    if (is_on_ground) {
        transform::Set::at(m_transform_id)->velocity_limit = m_ground_velocity_limit;
    }
    //else {
        //transform::Set::at(m_transform_id)->velocity_limit = m_air_velocity_limit;
    //}
    



    transform()->velocity.y += transform()->acceleration.y;

    if (is_on_ground) {
        if (m_is_sliding_ground) {
            transform()->deceleration.x = 0.025f;
        } else {
            transform()->deceleration.x = 0.1f;
        }        
        transform()->acceleration.x = 0.2f;
    } else {
        transform()->deceleration.x = 0.0f;
        transform()->acceleration.x = 0.1f;
    }    

    if (is_on_ground || m_is_on_ledge) {
        //m_velocity_at_jump = 0.0f;
        m_is_hovering = false;
        if (m_time_left_jump_again > 0) {
            m_time_left_jump_again -= 1;
        } else {
            m_num_jumps = 0;
        }        
        if (is_pressed(key_sprint)) {
            transform()->velocity_limit.x = m_ground_velocity_limit.x * 1.2f;
        } else {
            transform()->velocity_limit.x = m_ground_velocity_limit.x;
        }
    }

    if (transform()->velocity.y > 0.0f) {
        m_is_climbing_ledge = false;
    }


    if (transform()->velocity.y > transform()->acceleration.y + (is_on_slope ? transform()->velocity_limit.y : 0.2f)) {
        //if (transform()->velocity.y >= 1.0f && !is_on_slope) {
        is_on_ground = false;
        is_on_slope = false;
        m_is_ducking = false;
        m_is_on_ledge = false;
        m_is_climbing_ledge = false;
    }
    if (is_on_slope && transform()->velocity.y >= transform()->velocity_limit.y) {
        is_on_slope = false;
    }


    if (!is_on_ground && !is_on_slope && !m_is_sliding_wall && !m_is_on_ledge && !m_is_climbing_ledge && !m_is_ducking && m_time_left_melee == 0) {
        if (transform()->velocity.y > 0.0f) {
            if (is_pressed(key_jump) && !m_is_locked_jump && m_time_left_jumping_wall == 0.0f) {
                m_is_hovering = true;
                m_is_locked_jump = true;
            } else if (m_current_anim_id != m_jump_spin_anim_id) {
                reset_anim(m_fall_anim_id);
                anim::Set::at(m_fall_anim_id)->speed = transform()->velocity.y / 10.0f;
            }
        }
    }


    if (is_pressed(key_down) && !m_is_on_ledge) {
        if (is_on_ground) {
            m_time_left_ducking = m_time_to_duck;
            release(key_sprint);
        } else {
            release(key_down);
            m_time_left_until_down_thrust = m_time_until_down_thrust;
            reset_anim(m_down_thrust_anim_id);
            if (sound::Set::at(m_hover_sound_id)->is_playing()) {
                sound::Set::at(m_hover_sound_id)->stop();
            }
            sound::Set::at(m_down_thrust_sound_id)->position({ position().x / 160.0f, position().y / 90.0f });
            sound::Set::at(m_down_thrust_sound_id)->play();
        }
    }

    if (m_is_on_ledge) {
        is_on_ground = false;
        m_num_jumps = 0;
        reset_anim(m_ledge_grab_anim_id);

        sprite()->is_leftward = m_is_wall_to_left;

        transform()->deceleration.x = 0.0f;
        transform()->acceleration.x = 0.0f;
        transform()->velocity.x = 0.0f;
        transform()->velocity.y = 0.0f;

        if (is_pressed(key_up)) {
            release(key_up);
            sound::Set::at(m_ledge_climb_sound_id)->position({ position().x / 160.0f, position().y / 90.0f });
            sound::Set::at(m_ledge_climb_sound_id)->play();

            m_is_on_ledge = false;
            m_is_climbing_ledge = true;
            transform()->velocity.y = -3.0f;
            reset_anim(m_ledge_climb_anim_id);
            //++m_num_jumps;
            /*if (m_num_jumps > 2) {
                m_num_jumps = 2;
            }*/
            m_time_left_jump_again = m_time_to_jump_again;
            //m_time_left_ducking = m_time_to_duck;
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
                //Console::log("m_is_wall_to_left: ", m_is_wall_to_left, "\n");
                transform()->velocity.x = m_is_wall_to_left ? 1.0f : -1.5f;
            } else {
                //Console::log("m_is_wall_to_left: ", m_is_wall_to_left, "\n");
                transform()->velocity.x = m_is_wall_to_left ? 1.5f : -1.0f;
            }
            //Console::log("velocity.y: ", transform()->velocity.y, "\n");
            is_on_ground = true;
            m_is_locked_jump = false;
            m_is_on_ledge = false;
        }
    }


    if (is_pressed(key_jump)) {
        if (is_on_ground && !m_is_locked_jump && !m_is_ducking) {
            ++m_num_jumps;
            m_time_left_jump_again = m_time_to_jump_again;
            m_is_locked_jump = true;
            is_on_ground = false;
            is_on_slope = false;
            m_is_hovering = false;
            m_is_sliding_ground = false;
            m_time_left_holding_jump = m_time_to_hold_jump;
            m_time_left_rising = 0;
            //transform()->velocity += transform()->moved_velocity;
            //transform()->moved_velocity = 0.0f;

            /*if (transform()->velocity.x <= -m_ground_velocity_limit.x || transform()->velocity.x >= m_ground_velocity_limit.x) {
                m_velocity_at_jump = transform()->velocity.x;
                Console::log("velocity at jump: ", m_velocity_at_jump, "\n");
            }*/

            transform()->position.y -= 2.0f;

            if (m_num_jumps == 3) {
                m_time_left_skidding = 0;
                m_num_jumps = 0;
                transform()->velocity.y = -3.0f;
                reset_anim(m_jump_spin_anim_id);
                sound::Set::at(m_jump_sound_id)->pitch(1.122454545454545f);
            } else {
                if (m_num_jumps == 2) {
                    transform()->velocity.y = -2.0f;
                    reset_anim(m_jump_anim_id);
                    sound::Set::at(m_jump_sound_id)->pitch(1.059454545454545f);
                } else {
                    transform()->velocity.y = -2.0f;
                    reset_anim(m_jump_anim_id);
                    sound::Set::at(m_jump_sound_id)->pitch(1.0f);
                }
                if (m_time_left_skidding > 0) {
                    m_time_left_skidding = 0;
                    reset_anim(m_jump_skid_anim_id);
                }
            }
            sound::Set::at(m_jump_sound_id)->position({ position().x / 160.0f, position().y / 90.0f });
            sound::Set::at(m_jump_sound_id)->play();
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
        m_is_hovering = false;
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

    

    if (m_is_hovering) {        
        if (transform()->velocity.y >= 1.0f) {            
            transform()->velocity.y = 1.0f;
            set_anim(m_hover_anim_id);
        }
        if (sound::Set::at(m_hover_sound_id)->is_stopped()) {
            sound::Set::at(m_hover_sound_id)->position({ position().x / 160.0f, position().y / 90.0f });
            sound::Set::at(m_hover_sound_id)->play();
        }
    } else {
        sound::Set::at(m_hover_sound_id)->stop();
    }

    
    if (!m_is_sliding_wall && sound::Set::at(m_slide_wall_sound_id)->is_playing()) {
        sound::Set::at(m_slide_wall_sound_id)->stop();
    }
    if ((!is_pressed(key_left) && !is_pressed(key_right)) || m_time_left_jumping_wall > 0 || is_on_ground || is_on_slope ||
        transform()->velocity.x < -transform()->acceleration.x || transform()->velocity.x > transform()->acceleration.x) {
        m_is_sliding_wall = false;
    } else {
        if (m_is_sliding_wall && !m_is_on_ledge) {
            set_anim(m_slide_wall_anim_id);
            if (sound::Set::at(m_slide_wall_sound_id)->is_stopped()) {
                //Console::log("sliding wall\n");
                sound::Set::at(m_slide_wall_sound_id)->play();
            }
            is_on_ground = false;
        }        
    }
    
       
    if (anim::Set::at(m_jump_spin_anim_id)->source.x >= anim::Set::at(m_jump_spin_anim_id)->source.w * 4) {
        anim::Set::at(m_jump_spin_anim_id)->first_frame = 4;
    } else {
        anim::Set::at(m_jump_spin_anim_id)->first_frame = 0;
    }


    if (m_current_anim_id == m_run_anim_id || m_current_anim_id == m_walk_anim_id) {
        if (anim::Set::at(m_current_anim_id)->source.x == 128 || anim::Set::at(m_current_anim_id)->source.x == 384) {
            sound::Set::at(m_step_grass_sound_id)->position({ position().x / 160.0f, position().y / 90.0f });
            sound::Set::at(m_step_grass_sound_id)->play();
        }
    }
        

    //Console::log(start::to_string(m_next_level), "\n");

    if (is_pressed(input::Key::num_5)) {
        release(input::Key::num_5);
        m_prev_state = m_state;
        m_next_state = player::State::swim;
        m_time_left_in_state = 20;
    }


    /*if (m_velocity_at_jump < 0.0f && transform()->velocity.x < m_velocity_at_jump) {
        transform()->velocity.x = m_velocity_at_jump;
    }
    if (m_velocity_at_jump > 0.0f && transform()->velocity.x > m_velocity_at_jump) {
        transform()->velocity.x = m_velocity_at_jump;
    }*/

    //Console::log("swing_position: ", (transform()->position.x - m_swing_position.x), "\n");


    if (m_time_left_ducking > 0) {
        Console::log("time_left_ducking: ", (int)m_time_left_ducking, "\n");
        --m_time_left_ducking;

        if (!m_is_sliding_ground) {
            if (!is_pressed(key_left) && !is_pressed(key_right)) {
                m_is_ducking ? set_anim(m_duck_anim_id) : reset_anim(m_duck_anim_id);
            } else {
                set_anim(m_crawl_anim_id);
            }
        }

        m_is_ducking = m_time_left_ducking > 0;

        anim::Set::at(m_rise_anim_id)->source.x = 0;
        m_time_left_rising = m_time_to_rise;
    }

    if (!m_is_ducking) {
        anim::Set::at(m_duck_anim_id)->source.x = 0;
        if (m_time_left_rising > 0) {
            --m_time_left_rising;
            set_anim(m_rise_anim_id);
        }
    }


    if (m_is_ducking || m_is_climbing_ledge) {
        for (auto& i : m_run_collider_ids)  aabb::Set::at(i)->is_inactive(true);
        for (auto& i : m_duck_collider_ids) aabb::Set::at(i)->is_inactive(false);
    } else {
        for (auto& i : m_run_collider_ids)  aabb::Set::at(i)->is_inactive(false);
        for (auto& i : m_duck_collider_ids) aabb::Set::at(i)->is_inactive(true);
    }


    if (m_time_left_in_state == 0) {
        if (m_next_state == player::State::swim) {
            m_state = player::State::swim;
            sprite()->is_upended = sprite()->is_leftward;
            sprite()->is_leftward ? sprite()->set_rotation(180.0f) : sprite()->set_rotation(0.0f);
            for (auto& i : m_run_collider_ids)  aabb::Set::at(i)->is_inactive(true);
            for (auto& i : m_duck_collider_ids) aabb::Set::at(i)->is_inactive(true);
            for (auto& i : m_swim_collider_ids) aabb::Set::at(i)->is_inactive(false);
        }
        if (m_next_state == player::State::swing) {
            m_state = player::State::swing;
        }
    }




}