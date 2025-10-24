module player;
import collider.aabb;
import console;
import health;
import particle;

void player::Object::update_run() {
    if (m_is_first_state_update) {
        m_is_first_state_update = false;
        sprite()->is_hidden = false;
        m_water_line_y = 0.0F;
        if (m_prev_state != entity::State::run) {
            m_prev_state = entity::State::run;
            m_next_state = entity::State::run;
            Console::log("player::Object::update_run\n");
        }
        if (sprite()->rotation != 0.0F) {
            sprite()->is_leftward = transform()->velocity.x < 0.0F;
            sprite()->is_upended = false;
            sprite()->set_rotation(0.0F);
        }
    }


    for (auto& i : m_aabb_ids) {
        aabb::Manager::get(i)->is_active(false);
        if (!aabb::Manager::get(i)->is_active() && aabb::Manager::get(i)->name == aabb::Name::body) {
            aabb::Manager::get(i)->is_active(true);
        }
    }

    if (health()->amount <= 0.0f) {
        m_next_state = entity::State::dead;
        return;
    }
    if (is_pressed(input::Key::h)) {
        release(input::Key::h);
        health()->amount -= 8.0f;
        //Console::log("heath: ", health()->amount, "\n");
    }
    if (is_pressed(input::Key::j)) {
        release(input::Key::j);
        health()->amount += 8.0f;
        //Console::log("heath: ", health()->amount, "\n");
    }
    if (is_pressed(input::Key::b)) {
        release(input::Key::b);
        spawn_fan(180.0F, 0.0F, 4, particle::Type::brick,
                   transform()->position + Vec2F{6.0F, -4.0F},
                   transform()->velocity + transform()->moved_velocity,
                   3.0F);
    }

    //Console::log("is carrying: ", m_is_carrying, "\n");

    if (is_pressed(key_melee) && !m_is_carrying && !m_is_ducking && !m_is_on_ledge && !m_is_locked_jump) {
        if (!m_is_locked_melee) {
            m_is_locked_melee = true;
            m_time_left_melee = m_time_to_melee;
            reset_anim(m_melee_anim_id);

            if (m_melee_sound_id != -1 && sound::Manager::get(m_melee_sound_id)) {
                //m_sounds.at(m_melee_sound_id)->position({ position().x / 160.0F, position().y / 90.0F });
                //Console::log("player::Object::update_run melee sound position: ", m_sounds.at(m_melee_sound_id)->position().x, " ", m_sounds.at(m_melee_sound_id)->position().y, "\n");
                //m_sounds.at(m_melee_sound_id)->play();
                sound::Manager::get(m_melee_sound_id)->position({ position().x / 160.0F, position().y / 90.0F });
                sound::Manager::get(m_melee_sound_id)->play();
            }
        }
    } else {
        m_is_locked_melee = false;
    }

    if (m_time_left_melee > 0) {
        if (m_time_left_melee == m_time_to_melee) {
            particle::Manager::spawn({ .parent = this,
                                       .type = particle::Type::melee,
                                       .position = transform()->position + Vec2F{ sprite()->is_leftward ? -16.0F : 16.0F, 0.0F },
                                       .velocity = transform()->velocity + Vec2F{ sprite()->is_leftward ? -2.0F : 2.0F, 0.0F }                                       
                                       });
        }
        --m_time_left_melee;

        if (m_is_on_ground) {
            transform()->velocity.x *= 0.9F;
        }
    }

    //Console::log("player position: ", position().x, " ", position().y, "\n");
    
    if (m_time_left_bouncing > 0) {
        //Console::log("time left bouncing: ", (int)m_time_left_bouncing, "\n");
        --m_time_left_bouncing;
        if (m_time_left_bouncing == 0 && m_is_on_ground) {
            transform()->max_velocity = m_ground_velocity_limit;
        }
    }


    if (m_time_left_until_down_thrust > 0) {
        --m_time_left_until_down_thrust;
        transform()->velocity = { 0.0F, 0.0F };
        if (m_time_left_until_down_thrust > 0) {
            return;
        } else if (!m_is_down_thrusting) {
            m_is_down_thrusting = true;            
            //anim::Manager::get(m_down_thrust_anim_id)->speed = 0.5F;
            transform()->max_velocity.y = 10.0F;
            transform()->velocity.y = 10.0F;
        }
    }

    if (m_is_down_thrusting) {        
        if (m_is_on_ground) {
            m_is_down_thrusting = false;
            m_time_left_hitting_ground = m_time_to_hit_ground;
            reset_anim(m_hit_ground_anim_id);            
            sound::Manager::get(m_hit_ground_sound_id)->position({ position().x / 160.0F, position().y / 90.0F });
            sound::Manager::get(m_hit_ground_sound_id)->play();
        } else {
            set_anim(m_down_thrust_anim_id);
            return;
        }
    }
    
    if (m_time_left_hitting_ground > 0) {
        if (m_time_left_hitting_ground == m_time_to_hit_ground) {
            spawn_fan(0.0F, 360.0F, 8, particle::Type::drop_blood, transform()->position - Vec2F{8.0F, 8.0F}, {}, 5.0F);
            spawn_fan(0.0F, 360.0F, 8, particle::Type::drop_blood, transform()->position - Vec2F{8.0F, 8.0F}, {}, 4.0F);

            spawn_down_thrust(transform()->position + Vec2F{8.0f, 8.0f});
        }
        --m_time_left_hitting_ground;
        transform()->velocity.x = 0.0F;
        return;
    }


    if (m_is_on_ground && !m_is_ducking && m_time_left_rising == 0 &&
        !is_pressed(key_left) && !is_pressed(key_right) && m_time_left_melee == 0
        //transform()->velocity.x >= -transform()->acceleration.x && transform()->velocity.x <= transform()->acceleration.x
        ) {
        set_anim(m_idle_anim_id);
    }

    m_is_sliding_ground = false;
    if (m_is_on_ground && m_time_left_melee == 0 && m_time_left_skidding == 0) {
        if (transform()->velocity.x >= -transform()->acceleration.x && transform()->velocity.x <= transform()->acceleration.x) {
            if (!m_is_ducking && m_time_left_rising == 0) {
                set_anim(m_idle_anim_id);
            }
            if (m_is_ducking) {                
                set_anim(m_duck_anim_id);
            }
        } else {
            if (m_is_ducking) {
                if (m_time_left_rising == m_time_to_rise) {              
                    if (!m_is_carrying &&
                        transform()->velocity.x < transform()->max_velocity.x * -0.7F ||
                        transform()->velocity.x > transform()->max_velocity.x *  0.7F) {
                        m_is_sliding_ground = true;
                    }

                    if (m_is_sliding_ground) {
                        set_anim(m_slide_ground_anim_id);
                    } else {
                        anim::Manager::get(m_duck_anim_id)->source.x = anim::Manager::get(m_duck_anim_id)->source.w * 4;
                        set_anim(m_crawl_anim_id);
                        if (transform()->velocity.x < transform()->max_velocity.x * -0.5F) {
                            transform()->velocity.x = transform()->max_velocity.x * -0.5F;
                        } else if (transform()->velocity.x > transform()->max_velocity.x * 0.5F) {
                            transform()->velocity.x = transform()->max_velocity.x * 0.5F;
                        }
                    }
                                        
                }
            } else {
                anim::Manager::get(m_slide_ground_anim_id)->source.x = 0;
                //Console::log("run walk\n");
                is_pressed(key_sprint) ? set_anim(m_run_anim_id) : set_anim(m_walk_anim_id);
            }            
        }
    }

    if (is_pressed(key_left)) {
        if (!m_is_sliding_ground && (transform()->velocity.x > -m_ground_velocity_limit.x * is_pressed(key_sprint) ? 1.2F : 1.0F)) {
            //Console::log("transform()->max_velocity.x: ", transform()->max_velocity.x, "\n");
            //Console::log("transform()->acceleration.x: ", transform()->acceleration.x, "\n");
            //Console::log("m_ground_velocity_limit.x: ", m_ground_velocity_limit.x, "\n");
            transform()->velocity.x -= transform()->acceleration.x;
        }        
        //Console::log("anim source.x: ", anim::Manager::get(m_current_anim_id)->source.x, "\n");

        if (m_is_on_ground && !m_is_ducking) {
            if (transform()->velocity.x >= 0.0F) {
                m_time_left_skidding = m_time_to_skid;
                if (!sprite()->is_leftward) {
                    anim::Manager::get(m_skid_anim_id)->source.x = 0;
                }
            }
            if (transform()->velocity.x < -2.0F) {
                m_time_left_skidding = 0.0F;
            }           
        }
        if (!m_is_sliding_wall && !m_is_on_ledge && m_time_left_jumping_wall == 0) {
            sprite()->is_leftward = true;
        }
    }
    if (is_pressed(key_right)) {
        if (!m_is_sliding_ground && (transform()->velocity.x < m_ground_velocity_limit.x * is_pressed(key_sprint) ? 1.2F : 1.0F)) {
            transform()->velocity.x += transform()->acceleration.x;
        }
        if (m_is_on_ground && !m_is_ducking) {
            if (transform()->velocity.x <= 0.0F) {
                m_time_left_skidding = m_time_to_skid;
                if (sprite()->is_leftward) {
                    anim::Manager::get(m_skid_anim_id)->source.x = 0;
                }
            }
            if (transform()->velocity.x > 2.0F) {
                m_time_left_skidding = 0.0F;
            }            
        }
        if (!m_is_sliding_wall && !m_is_on_ledge && m_time_left_jumping_wall == 0) {
            sprite()->is_leftward = false;
        }
    }
    if (m_time_left_skidding > 0 && m_time_left_melee == 0 && !m_is_sliding_wall) {
        if (m_time_left_skidding == m_time_to_skid) {
            sound::Manager::get(m_skid_sound_id)->position({ position().x / 160.0F, position().y / 90.0F });
            sound::Manager::get(m_skid_sound_id)->play();
        }
        --m_time_left_skidding;
        set_anim(m_skid_anim_id);
        //Console::log("player::Object::update_run m_time_left_skidding: ", m_time_left_skidding, "\n");
    } else {
        anim::Manager::get(m_skid_anim_id)->source.x = 0;
    }



    transform::Manager::get(m_transform_id)->acceleration = { 0.2F, 0.2F };
    transform::Manager::get(m_transform_id)->deceleration = { 0.04f, 0.04f };

    if (m_is_on_ground) {
        transform::Manager::get(m_transform_id)->max_velocity = m_ground_velocity_limit;        
    }
    //else {
        //transform::Manager::get(m_transform_id)->max_velocity = m_air_velocity_limit;
    //}
    
    


    transform()->velocity.y += transform()->acceleration.y;
    //transform()->moved_velocity.y += transform()->acceleration.y;

    if (transform()->moved_velocity.y < 0.0F) {
        transform()->moved_velocity.y += transform()->acceleration.y;
    }


    //Console::log("moved y: ", transform()->moved_velocity.y, "\n");

    if (m_is_on_ground) {
        if (m_is_sliding_ground) {
            transform()->deceleration.x = 0.025f;
        } else {
            transform()->deceleration.x = 0.1F;
        }        
        transform()->acceleration.x = 0.2F;
    } else {
        transform()->deceleration.x = 0.0F;
        transform()->acceleration.x = 0.1F;
    }    

    if (m_is_on_ground || m_is_on_ledge) {
        //m_velocity_at_jump = 0.0F;
        m_is_hovering = false;
        if (m_time_left_jump_again > 0) {
            m_time_left_jump_again -= 1;
        } else {
            m_num_jumps = 0;
        }
                
        if (is_pressed(key_sprint)) {
            transform()->max_velocity.x = m_ground_velocity_limit.x * 1.2F;
        } else {
            transform()->max_velocity.x = m_ground_velocity_limit.x;
        }
        if (m_is_carrying) {
            transform()->max_velocity.x *= 0.9F;
        }
    }


    if (transform()->velocity.y > transform()->acceleration.y + (m_is_on_slope ? transform()->max_velocity.y : 1.0F)) {
        //if (transform()->velocity.y >= 1.0F && !m_is_on_slope) {
        m_is_on_ground = false;
        m_is_on_slope = false;
        m_is_ducking = false;
        //m_is_on_ledge = false;
    }
    if (m_is_on_slope && transform()->velocity.y >= transform()->max_velocity.y) {
        m_is_on_slope = false;
    }
    if (m_is_on_ground || transform()->velocity.y > 2.0F) {
        m_is_climbing_ledge = false;
    }


    if (!m_is_on_ground && !m_is_on_slope && !m_is_sliding_wall && !m_is_on_ledge && !m_is_climbing_ledge && !m_is_ducking && m_time_left_melee == 0) {
        if (transform()->velocity.y > transform()->acceleration.y) {
            if (is_pressed(key_jump) && !m_is_locked_jump && m_time_left_jumping_wall == 0.0F) {

                if (transform()->moved_velocity.y != 0.0F) {
                    transform()->velocity.y += transform()->moved_velocity.y;
                    transform()->moved_velocity.y = 0.0F;
                }
                m_is_hovering = true;
                m_is_locked_jump = true;
                m_time_left_ducking = 0;
                m_time_left_rising = 0;
            } else if (m_current_anim_id != m_jump_spin_anim_id) {
                reset_anim(m_fall_anim_id);
                anim::Manager::get(m_fall_anim_id)->speed = transform()->velocity.y / 10.0F;
            }
        }
    }

    if (is_pressed(key_down) && !m_is_on_ledge) {
        if (m_is_on_ground) {
            m_time_left_ducking = m_time_to_duck;
            release(key_sprint);
        } else {
            if (m_time_left_ducking == 0 && m_time_left_rising == 0) {
                release(key_down);
                release(key_jump);
                m_time_left_until_down_thrust = m_time_until_down_thrust;
                reset_anim(m_down_thrust_anim_id);
                if (sound::Manager::get(m_hover_sound_id)->is_playing()) {
                    sound::Manager::get(m_hover_sound_id)->stop();
                }
                sound::Manager::get(m_down_thrust_sound_id)->position({ position().x / 160.0F, position().y / 90.0F });
                sound::Manager::get(m_down_thrust_sound_id)->play();
            }
        }
    }

    if (!m_is_ducking && !m_is_on_ledge) {
        if (is_pressed(key_up)) {
            release(key_up);
            spawn(particle::Type::interact, transform()->position + Vec2F{ -8.0f, 0.0f }, transform()->velocity + transform()->moved_velocity);
        }
    }

    if (m_time_left_lever > 0) {
        --m_time_left_lever;
        set_anim(m_lever_anim_id);
        transform()->velocity = { 0.0F,0.0F };
    }


    if (m_is_on_ledge) {
        m_is_on_ground = false;
        m_num_jumps = 0;
        
        set_anim(m_ledge_grab_anim_id);

        sprite()->is_leftward = m_is_wall_to_left;

        transform()->deceleration.x = 0.0F;
        transform()->acceleration.x = 0.0F;
        transform()->velocity.x = 0.0F;
        transform()->velocity.y = 0.0F;

        if (is_pressed(key_up)) {
            release(key_up);
            sound::Manager::get(m_ledge_climb_sound_id)->position({ position().x / 160.0F, position().y / 90.0F });
            sound::Manager::get(m_ledge_climb_sound_id)->play();

            m_is_on_ledge = false;
            m_is_climbing_ledge = true;
            transform()->velocity.y = -2.8F;
            reset_anim(m_ledge_climb_anim_id);
            ++m_num_jumps;
            if (m_num_jumps > 2) {
                m_num_jumps = 2;
            }
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

        transform()->velocity = { 0.0F, 0.0F };
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
                transform()->velocity.x = m_is_wall_to_left ? 1.0F : -1.5F;
            } else {
                //Console::log("m_is_wall_to_left: ", m_is_wall_to_left, "\n");
                transform()->velocity.x = m_is_wall_to_left ? 1.5F : -1.0F;
            }
            //Console::log("velocity.y: ", transform()->velocity.y, "\n");
            m_is_on_ground = true;
            m_is_locked_jump = false;
            m_is_on_ledge = false;
        }
    }


    if (is_pressed(key_jump)) {
        if (m_is_on_ground && !m_is_locked_jump && !m_is_ducking) {            
            m_time_left_jump_again = m_time_to_jump_again;
            m_is_locked_jump = true;
            m_is_on_ground = false;
            m_is_on_slope = false;
            m_is_hovering = false;
            m_is_sliding_ground = false;
            m_time_left_holding_jump = m_time_to_hold_jump;
            m_time_left_rising = 0;
            //transform()->velocity += transform()->moved_velocity;
            //transform()->moved_velocity = 0.0F;

            /*if (transform()->velocity.x <= -m_ground_velocity_limit.x || transform()->velocity.x >= m_ground_velocity_limit.x) {
                m_velocity_at_jump = transform()->velocity.x;
                Console::log("velocity at jump: ", m_velocity_at_jump, "\n");
            }*/

            transform()->position.y -= 2.0F;            

            ++m_num_jumps;
            if (m_is_carrying) {
                m_num_jumps = 1;
            }
            if (m_num_jumps == 3) {
                m_time_left_skidding = 0;
                m_num_jumps = 0;
                transform()->velocity.y = -3.0F;
                reset_anim(m_jump_spin_anim_id);
                sound::Manager::get(m_jump_sound_id)->pitch(1.122454545454545f);
            } else {
                if (m_num_jumps == 2) {
                    transform()->velocity.y = -2.2F;
                    reset_anim(m_jump_anim_id);
                    sound::Manager::get(m_jump_sound_id)->pitch(1.059454545454545f);
                } else {
                    transform()->velocity.y = -2.0F;
                    reset_anim(m_jump_anim_id);
                    sound::Manager::get(m_jump_sound_id)->pitch(1.0F);
                }
                if (m_time_left_skidding > 0) {
                    m_time_left_skidding = 0;
                    reset_anim(m_jump_skid_anim_id);
                }
            }
            sound::Manager::get(m_jump_sound_id)->position({ position().x / 160.0F, position().y / 90.0F });
            sound::Manager::get(m_jump_sound_id)->play();
        } else {
            if (m_time_left_holding_jump > 0 && transform()->velocity.y < 0.0F) {
                --m_time_left_holding_jump;
                transform()->velocity.y -= 0.3F;
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
        //Console::log("vel: ", transform()->velocity.x, " ", anim::Manager::get(m_current_anim_id), "\n");
        if (transform()->velocity.x < 0.0F) {
            anim::Manager::get(m_current_anim_id)->speed = transform()->velocity.x / -5.0F;
        } else {
            anim::Manager::get(m_current_anim_id)->speed = transform()->velocity.x / 5.0F;
        }
        if (anim::Manager::get(m_current_anim_id)->speed < 0.2F) {
            anim::Manager::get(m_current_anim_id)->speed = 0.2F;
        }
    }

    

    if (m_is_hovering) {
        if (transform()->velocity.y >= 1.0F) {            
            transform()->velocity.y = 1.0F;
            set_anim(m_hover_anim_id);
        }
        if (sound::Manager::get(m_hover_sound_id)->is_stopped()) {
            sound::Manager::get(m_hover_sound_id)->position({ position().x / 160.0F, position().y / 90.0F });
            sound::Manager::get(m_hover_sound_id)->play();
        }
    } else {
        sound::Manager::get(m_hover_sound_id)->stop();
    }

    
    if (!m_is_sliding_wall && sound::Manager::get(m_slide_wall_sound_id)->is_playing()) {
        sound::Manager::get(m_slide_wall_sound_id)->stop();
    }
    if ((!is_pressed(key_left) && !is_pressed(key_right)) || m_time_left_jumping_wall > 0 || m_is_on_ground || m_is_on_slope ||
        transform()->velocity.x < -transform()->acceleration.x || transform()->velocity.x > transform()->acceleration.x) {
        m_is_sliding_wall = false;
    } else {
        if (m_is_sliding_wall && !m_is_on_ledge) {
            set_anim(m_slide_wall_anim_id);
            m_time_left_ducking = 0;
            m_time_left_rising = 0;
            if (sound::Manager::get(m_slide_wall_sound_id)->is_stopped()) {
                //Console::log("sliding wall\n");
                sound::Manager::get(m_slide_wall_sound_id)->play();
            }
            m_is_on_ground = false;
        }        
    }
    
       
    if (anim::Manager::get(m_jump_spin_anim_id)->source.x >= anim::Manager::get(m_jump_spin_anim_id)->source.w * 4) {
        anim::Manager::get(m_jump_spin_anim_id)->first_frame = 4;
    } else {
        anim::Manager::get(m_jump_spin_anim_id)->first_frame = 0;
    }


    if (m_current_anim_id == m_run_anim_id || m_current_anim_id == m_walk_anim_id) {
        if (anim::Manager::get(m_current_anim_id)->source.x == 128 || anim::Manager::get(m_current_anim_id)->source.x == 384) {
            sound::Manager::get(m_step_grass_sound_id)->position({ position().x / 160.0F, position().y / 90.0F });
            sound::Manager::get(m_step_grass_sound_id)->play();
        }
    }
        

    //Console::log(start::to_string(m_next_level), "\n");

    if (is_pressed(input::Key::num_5)) {
        release(input::Key::num_5);
        m_prev_state = m_state;
        m_next_state = entity::State::swim;
        m_time_left_in_state = 20;
    }


    /*if (m_velocity_at_jump < 0.0F && transform()->velocity.x < m_velocity_at_jump) {
        transform()->velocity.x = m_velocity_at_jump;
    }
    if (m_velocity_at_jump > 0.0F && transform()->velocity.x > m_velocity_at_jump) {
        transform()->velocity.x = m_velocity_at_jump;
    }*/

    //Console::log("swing_position: ", (transform()->position.x - m_swing_position.x), "\n");
    
    if (m_time_left_ducking > 0) {
        //Console::log("Player::update_run() time_left_ducking: ", (int)m_time_left_ducking, "\n");
        --m_time_left_ducking;

        if (!m_is_sliding_ground) {
            if (!is_pressed(key_left) && !is_pressed(key_right)) {
                m_is_ducking ? set_anim(m_duck_anim_id) : reset_anim(m_duck_anim_id);
            } else {
                set_anim(m_crawl_anim_id);
            }
        }

        m_is_ducking = m_time_left_ducking > 0;

        anim::Manager::get(m_rise_anim_id)->source.x = 0;
        m_time_left_rising = m_time_to_rise;
    }

    if (!m_is_ducking) {
        anim::Manager::get(m_duck_anim_id)->source.x = 0;
        if (m_time_left_rising > 0) {
            --m_time_left_rising;
            set_anim(m_rise_anim_id);
        }
    }
    
    if (m_next_state == entity::State::swim) {
        if (m_time_left_in_state > 0) --m_time_left_in_state;
        if (m_time_left_in_state == 0) {
            m_state = entity::State::swim;
            m_is_first_state_update = true;
            sprite()->is_upended = sprite()->is_leftward;
            sprite()->is_leftward ? sprite()->set_rotation(180.0F) : sprite()->set_rotation(0.0F);
        }
    }
    if (m_next_state == entity::State::swing && m_time_left_in_state == 0) {
        m_state = entity::State::swing;
        m_is_first_state_update = true;
    }
}