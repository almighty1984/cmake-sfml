module;
#include <cmath>
export module entity.train;
import collider.aabb;
import anim;
import camera;
import console;
import entity;

export namespace entity {
    class Train : public Object {
        f32 m_speed = 0.0f, m_start_speed = 1.0f;

        Vec2f m_prev_velocity = { 0.0f, 0.0f };
    public:
        Train() {
            //m_speed = m_start_speed;
        }
        void collide_x(aabb::Infoc our, aabb::Infoc other) override {
            if (!sprite() || !our.owner || !other.owner || !transform() || !aabb::Set::at(our.id) || !aabb::Set::at(other.id)) return;
            if (aabb::Set::at(our.id)->points().size() < 4 || aabb::Set::at(other.id)->points().size() < 4) return;
            
            aabb::Namec our_aabb_name = aabb::Set::at(our.id)->name;

            if (our_aabb_name != aabb::Name::bone) {
                return;
            }

            aabb::Name other_aabb_name = aabb::Set::at(other.id)->name;

            //Console::log("aabb name: ", aabb::name_to_string(our_aabb_name), "\n");

            Rectfc our_rect = { aabb::Set::at(our.id)->points().at(0).x, aabb::Set::at(our.id)->points().at(0).y,
                                aabb::Set::at(our.id)->points().at(3).x, aabb::Set::at(our.id)->points().at(3).y };
            
            entity::Typec other_type = other.owner->type();
            Rectfc        other_rect = { aabb::Set::at(other.id)->points().at(0).x, aabb::Set::at(other.id)->points().at(0).y,
                                         aabb::Set::at(other.id)->points().at(3).x, aabb::Set::at(other.id)->points().at(3).y };
            
            if (!entity::is_track(other_type) && other_type != entity::Type::player) return;

            f32c diff_x = other_rect.x - our_rect.x;
            f32c diff_y = other_rect.y - our_rect.y;

            f32c overlap_x = our_rect.x < other_rect.x ? our_rect.w - other_rect.x : -(other_rect.w - our_rect.x);


            const bool is_vel_x_larger = std::abs(transform()->velocity.x) > std::abs(transform()->velocity.y);

            if (entity::is_track(other_type)) {
                m_is_powered = other.owner->is_powered();                                
                m_time_left_alive = other.owner->time_left_alive();
                //m_speed = m_start_speed;
                // 
                //Console::log("time_left_alive: ", other.owner->time_left_alive(), "\n");
                //Console::log("is_powered: ", m_is_powered, "\n");
            }
                        
            if (other_type == entity::Type::track_U || other_type == entity::Type::track_D) {                
                transform()->velocity.x = 0.0f;
                transform()->velocity.x += diff_x * std::abs(m_speed);

                if (transform()->velocity.y < 0.0f) {
                    transform()->velocity.y = -std::abs(m_speed);
                } else if (transform()->velocity.y > 0.0f) {
                    transform()->velocity.y = std::abs(m_speed);
                }
            } else if (other_type == entity::Type::track_L || other_type == entity::Type::track_R) { 
                transform()->velocity.y = 0.0f;

                if (transform()->velocity.x < 0.0f) {
                    transform()->velocity.x = -std::abs(m_speed);
                } else if (transform()->velocity.x > 0.0f) {
                    transform()->velocity.x = std::abs(m_speed);
                } else {
                    if (m_prev_velocity.x < 0.0f) {
                        transform()->velocity.x = -std::abs(m_speed);
                    } else {
                        transform()->velocity.x = std::abs(m_speed);
                    }
                }
                transform()->velocity.y += diff_y / 16.0f * std::abs(m_speed);
            } else if (other_type == entity::Type::track_UR || other_type == entity::Type::track_LD) {
                if (other_aabb_name == aabb::Name::right) {
                    if (our_rect.w > other_rect.x) {
                        transform()->velocity.x = 0.0f;
                        transform()->position.x -= overlap_x;
                    }
                }
                else if (other_aabb_name == aabb::Name::left) {
                    if (transform()->velocity.x < 0.0f) {
                        if (transform()->velocity.y < -0.1f) {
                            transform()->velocity.x = 0.0f;
                            transform()->position.x -= overlap_x;
                        } else if (transform()->velocity.y > -0.1f) {
                            if (our_rect.x < other_rect.w) {
                                transform()->velocity.x = 0.0f;
                                transform()->position.x -= overlap_x;
                            }
                            //transform()->velocity.x += diff_x * std::abs(m_speed);
                            transform()->velocity.y = std::abs(m_speed);
                            //aabb::Set::at(our.id)->time_left_colliding = 10;
                        }
                    }
                }
            } else if (other_type == entity::Type::track_UL || other_type == entity::Type::track_RD) {
                if (other_aabb_name == aabb::Name::left) {
                    if (transform()->velocity.x < 0.0f) {
                        if (transform()->velocity.y < -0.1f) {
                            transform()->velocity.x = 0.0f;
                            transform()->position.x -= overlap_x;
                        }
                    }
                }
                else if (other_aabb_name == aabb::Name::right) {
                    if (transform()->velocity.x > 0.0f) {
                        if (transform()->velocity.y < -0.1f) {
                            transform()->velocity.x = 0.0f;
                            transform()->position.x -= overlap_x;
                        } else if (transform()->velocity.y > -0.1f) {
                            if (our_rect.w > other_rect.x) {
                                transform()->velocity.x = 0.0f;
                                transform()->position.x -= overlap_x;
                            }
                            //transform()->velocity.x += diff_x * std::abs(m_speed);
                            transform()->velocity.y = std::abs(m_speed);
                            //aabb::Set::at(our.id)->time_left_colliding = 10;
                        }
                    }
                }
            } else if (other_type == entity::Type::track_LU || other_type == entity::Type::track_DR) {
                if (other_aabb_name == aabb::Name::right) {
                    if (transform()->velocity.x > 0.0f) {
                        if (transform()->velocity.y > 0.1f) {
                            transform()->velocity.x = 0.0f;
                            transform()->position.x -= overlap_x;
                        }
                    }
                }
                else if (other_aabb_name == aabb::Name::left) {
                    if (transform()->velocity.x < 0.0f) {
                        if (transform()->velocity.y > 0.1f) {
                            transform()->velocity.x = 0.0f;
                            transform()->position.x -= overlap_x;
                        } else if (transform()->velocity.y < 0.1f) {
                            if (our_rect.x < other_rect.w) {
                                transform()->velocity.x = 0.0f;
                                transform()->position.x -= overlap_x;
                            }
                            transform()->velocity.y = -std::abs(m_speed);
                        }
                    }
                }                
            } else if (other_type == entity::Type::track_RU || other_type == entity::Type::track_DL) {
                if (other_aabb_name == aabb::Name::left) {
                    if (transform()->velocity.x < 0.0f) {
                        if (transform()->velocity.y > 0.1f) {
                            transform()->velocity.x = 0.0f;
                            transform()->position.x -= overlap_x;
                        }
                    }
                } else if (other_aabb_name == aabb::Name::right) {
                    if (transform()->velocity.x > 0.0f) {
                        if (transform()->velocity.y > 0.1f) {
                            transform()->velocity.x = 0.0f;
                            transform()->position.x -= overlap_x;
                        } else if (transform()->velocity.y < 0.1f) {
                            if (our_rect.x < other_rect.w) {
                                transform()->velocity.x = 0.0f;
                                transform()->position.x -= overlap_x;
                            }
                            transform()->velocity.y = -std::abs(m_speed);
                        }
                    }
                }
            } else if (other_type == entity::Type::track_LD_1x1 || other_type == entity::Type::track_DL_1x1 ||
                       other_type == entity::Type::track_UR_1x1 || other_type == entity::Type::track_RU_1x1) {
                if (is_vel_x_larger && transform()->velocity.x < 0.0f && our_rect.x <= other_rect.x) {
                    transform()->velocity.x = -std::abs(m_speed) * 0.71f;
                    transform()->velocity.y =  std::abs(m_speed) * 0.71f;
                    //transform()->velocity.x += diff_x / 16.0f;
                    //transform()->velocity.y += diff_y / 16.0f * std::abs(m_speed);                    
                }
                else if (is_vel_x_larger && transform()->velocity.x > 0.0f && our_rect.w >= other_rect.w) {
                    transform()->velocity.x = std::abs(m_speed) * 0.71f;
                    transform()->velocity.y = -std::abs(m_speed) * 0.71f;
                }
                else if (!is_vel_x_larger && transform()->velocity.y < 0.0f && our_rect.y <= other_rect.h) {
                    transform()->velocity.x =  std::abs(m_speed) * 0.71f;
                    transform()->velocity.y = -std::abs(m_speed) * 0.71f;
                    //transform()->velocity.x += diff_x / 16.0f;
                    //transform()->velocity.y += diff_y / 16.0f * std::abs(m_speed);
                }
                else if (!is_vel_x_larger && transform()->velocity.y > 0.0f && our_rect.y >= other_rect.y) {
                    transform()->velocity.x = -std::abs(m_speed) * 0.71f;
                    transform()->velocity.y = std::abs(m_speed) * 0.71f;
                }
                
            } else if (other_type == entity::Type::track_LU_1x1 || other_type == entity::Type::track_UL_1x1 ||
                       other_type == entity::Type::track_RD_1x1 || other_type == entity::Type::track_DR_1x1) {
                
                if (is_vel_x_larger && transform()->velocity.x < 0.0f && our_rect.x <= other_rect.x) {
                    transform()->velocity.x = -std::abs(m_speed) * 0.71f;
                    transform()->velocity.y = -std::abs(m_speed) * 0.71f;

                    //transform()->velocity.x += diff_x / 16.0f;

                    //transform()->velocity.y += diff_y / 16.0f * std::abs(m_speed);
                } else if (is_vel_x_larger && transform()->velocity.x > 0.0f && our_rect.x >= other_rect.x) {
                    transform()->velocity.x = std::abs(m_speed) * 0.71f;
                    transform()->velocity.y = std::abs(m_speed) * 0.71f;

                    //transform()->velocity.x += diff_x / 16.0f;

                    //transform()->velocity.y += diff_y / 16.0f * std::abs(m_speed);
                } else if (!is_vel_x_larger && transform()->velocity.y < 0.0f && our_rect.y <= other_rect.y) {
                    transform()->velocity.x = -std::abs(m_speed) * 0.71f;
                    transform()->velocity.y = -std::abs(m_speed) * 0.71f;
                } else if (!is_vel_x_larger && transform()->velocity.y > 0.0f && our_rect.y >= other_rect.y) {
                    transform()->velocity.x = std::abs(m_speed) * 0.71f;
                    transform()->velocity.y = std::abs(m_speed) * 0.71f;
                }
                
            } else if (other_type == entity::Type::track_LU_2x1_0 || other_type == entity::Type::track_LU_2x1_1 ||
                       other_type == entity::Type::track_RD_2x1_0 || other_type == entity::Type::track_RD_2x1_1) {

                if (is_vel_x_larger && transform()->velocity.x < 0.0f || !is_vel_x_larger && transform()->velocity.y < 0.0f) {
                    transform()->velocity.x = -std::abs(m_speed);
                } else if (is_vel_x_larger && transform()->velocity.x > 0.0f && !is_vel_x_larger && transform()->velocity.y > 0.0f) {
                    transform()->velocity.x = std::abs(m_speed);
                }
                transform()->velocity.y = transform()->velocity.x / 2.0f;    

                //transform()->velocity.x += diff_x / 16.0f;
                //transform()->velocity.y += diff_y / 16.0f * std::abs(m_speed);
            } else if (other_type == entity::Type::track_LD_2x1_0 || other_type == entity::Type::track_LD_2x1_1) {
                if (is_vel_x_larger && transform()->velocity.x < 0.0f || !is_vel_x_larger && transform()->velocity.y < 0.0f) {
                    transform()->velocity.x = -std::abs(m_speed);
                } else if (is_vel_x_larger && transform()->velocity.x > 0.0f && !is_vel_x_larger && transform()->velocity.y > 0.0f) {
                    transform()->velocity.x = std::abs(m_speed);
                }
                transform()->velocity.y = -transform()->velocity.x / 2.0f;
                

                //transform()->velocity.y = -transform()->velocity.x / 2.0f;
                //transform()->velocity.y += diff_y / 16.0f * std::abs(m_speed);


            } else if (other_type == entity::Type::track_RU_2x1_0 || other_type == entity::Type::track_RU_2x1_1) {
                if (transform()->velocity.x < 0.0f) {
                    transform()->velocity.x = -std::abs(m_speed);
                } else if (transform()->velocity.x > 0.0f) {
                    transform()->velocity.x = std::abs(m_speed);
                }
                transform()->velocity.y = -transform()->velocity.x / 2.0f;

                //transform()->velocity.x += diff_x / 16.0f;

                transform()->velocity.y += diff_y / 16.0f * std::abs(m_speed);

            } else if (other_type == entity::Type::track_UR_1x2_0 || other_type == entity::Type::track_UR_1x2_1 ||
                       other_type == entity::Type::track_DL_1x2_0 || other_type == entity::Type::track_DL_1x2_1) {
                if (!is_vel_x_larger && transform()->velocity.y < 0.0f || is_vel_x_larger && transform()->velocity.x > 0.0f) {
                    if (our_rect.y <= other_rect.h) {
                        transform()->velocity.y = -std::abs(m_speed);
                        //transform()->position.x += diff_x / 64.0f;
                        transform()->velocity.x = -transform()->velocity.y / 2.0f;
                        //transform()->velocity.x += diff_x / 16.0f;
                    }
                } else if (!is_vel_x_larger && transform()->velocity.y > 0.0f || is_vel_x_larger && transform()->velocity.x < 0.0f) {
                    if (our_rect.h >= other_rect.y) {
                        transform()->velocity.y = std::abs(m_speed);
                        transform()->velocity.x = -transform()->velocity.y / 2.0f;
                        //transform()->velocity.x += diff_x / 16.0f;
                    }
                }
            } else if (other_type == entity::Type::track_UL_1x2_0 || other_type == entity::Type::track_UL_1x2_1 ||
                       other_type == entity::Type::track_DR_1x2_0 || other_type == entity::Type::track_DR_1x2_1) {

                if (!is_vel_x_larger && transform()->velocity.y < 0.0f/* || is_vel_x_larger && transform()->velocity.x < 0.0f*/) {
                    transform()->velocity.y = -std::abs(m_speed);
                } else if (!is_vel_x_larger && transform()->velocity.y > 0.0f/* || is_vel_x_larger && transform()->velocity.x > 0.0f*/) {
                    transform()->velocity.y = std::abs(m_speed);
                }
                transform()->velocity.x = transform()->velocity.y / 2.0f;
                //transform()->position.x += diff_x / 8.0f;
                //transform()->velocity.x += diff_x / 16.0f * std::abs(m_speed);
            }
        }
        void collide_y(aabb::Infoc our, aabb::Infoc other) override {
            if (!sprite() || !our.owner || !other.owner || !transform() || !aabb::Set::at(our.id) || !aabb::Set::at(other.id)) return;
            if (aabb::Set::at(our.id)->points().size() < 4 || aabb::Set::at(other.id)->points().size() < 4) return;

            aabb::Namec our_aabb_name = aabb::Set::at(our.id)->name;

            if (our_aabb_name != aabb::Name::bone) {
                return;
            }

            Rectfc our_rect = { aabb::Set::at(our.id)->points().at(0).x, aabb::Set::at(our.id)->points().at(0).y,
                                aabb::Set::at(our.id)->points().at(3).x, aabb::Set::at(our.id)->points().at(3).y };

            entity::Typec other_type = other.owner->type();
            Rectfc        other_rect = { aabb::Set::at(other.id)->points().at(0).x, aabb::Set::at(other.id)->points().at(0).y,
                                         aabb::Set::at(other.id)->points().at(3).x, aabb::Set::at(other.id)->points().at(3).y };
            aabb::Namec other_aabb_name = aabb::Set::at(other.id)->name;

            if (!entity::is_track(other_type) && other_type != entity::Type::player) return;
                

            f32c diff_x = other_rect.x - our_rect.x;
            f32c diff_y = other_rect.y - our_rect.y;

            f32c overlap_y = our_rect.y < other_rect.y ? our_rect.h - other_rect.y : -(other_rect.h - our_rect.y);

            //m_speed = 0.0f;
            if (entity::is_track(other_type)) {
                m_is_powered = other.owner->is_powered();
                m_time_left_alive = other.owner->time_left_alive();
                //m_speed = m_start_speed;
                //Console::log("time_left_alive: ", other.owner->time_left_alive(), "\n");
                //Console::log("is_powered: ", m_is_powered, "\n");
            }

            if (other_type == entity::Type::track_U || other_type == entity::Type::track_D) {
                collide_x(our, other);                     
            } else if (other_type == entity::Type::track_L || other_type == entity::Type::track_R) {                
                collide_x(our, other);
            } else if (other_type == entity::Type::track_UR || other_type == entity::Type::track_LD) {
                if (other_aabb_name == aabb::Name::up) {
                    if (transform()->velocity.y < -0.1f) {
                        if (our_rect.y < other_rect.h) {
                            transform()->velocity.y = 0.0f;
                            transform()->velocity.x = std::abs(m_speed);
                        }
                    }
                }
            } else if (other_type == entity::Type::track_UL || other_type == entity::Type::track_RD) {
                if (other_aabb_name == aabb::Name::up) {
                    if (transform()->velocity.y < -0.1f) {
                        if (our_rect.y < other_rect.h) {
                            transform()->velocity.y = 0.0f;
                            transform()->velocity.x = -std::abs(m_speed);
                        }
                    }
                }
            } else if (other_type == entity::Type::track_LU || other_type == entity::Type::track_DR) {
                if (other_aabb_name == aabb::Name::down) {
                    if (transform()->velocity.y > 0.1f) {
                        if (our_rect.h > other_rect.y) {
                            transform()->velocity.y = 0.0f;
                            transform()->velocity.x = std::abs(m_speed);
                        }
                    }
                }
            } else if (other_type == entity::Type::track_RU || other_type == entity::Type::track_DL) {
                if (other_aabb_name == aabb::Name::down) {
                    if (transform()->velocity.y > 0.1f) {
                        if (our_rect.h > other_rect.y) {
                            transform()->velocity.y = 0.0f;
                            transform()->velocity.x = -std::abs(m_speed);
                        }
                    }
                }
            } else if (other_type == entity::Type::track_RU_1x1 || other_type == entity::Type::track_UR_1x1) {                
                collide_x(our, other);
            } else if (other_type == entity::Type::track_LD_1x1 || other_type == entity::Type::track_DL_1x1) {                
                if (transform()->velocity.y > 0.0f && our_rect.y >= other_rect.y) {
                    transform()->velocity.x = -std::abs(m_speed) * 0.71f;
                    transform()->velocity.y = std::abs(m_speed) * 0.71f;
                    //transform()->position.x -= 1.0f;
                    transform()->velocity.y += diff_y / 16.0f * std::abs(m_speed);
                }
            } else if (other_type == entity::Type::track_RD_1x1 || other_type == entity::Type::track_DR_1x1 ||
                       other_type == entity::Type::track_LU_1x1 || other_type == entity::Type::track_UL_1x1) {
                collide_x(our, other);
            } else if (other_type == entity::Type::track_UL_1x2_0 || other_type == entity::Type::track_UL_1x2_1 ||
                       other_type == entity::Type::track_DR_1x2_0 || other_type == entity::Type::track_DR_1x2_1 ||                       
                       other_type == entity::Type::track_UR_1x2_0 || other_type == entity::Type::track_UR_1x2_1 ||
                       other_type == entity::Type::track_DL_1x2_0 || other_type == entity::Type::track_DL_1x2_1) {
                collide_x(our, other);                
            } else if (other_type == entity::Type::track_LU_2x1_0 || other_type == entity::Type::track_LU_2x1_1 ||
                       other_type == entity::Type::track_RD_2x1_0 || other_type == entity::Type::track_RD_2x1_1) {
                collide_x(our, other);
            } else if (other_type == entity::Type::track_LD_2x1_0 || other_type == entity::Type::track_LD_2x1_1 ||
                       other_type == entity::Type::track_RU_2x1_0 || other_type == entity::Type::track_RU_2x1_1) {
                collide_x(our, other);
            }
        }

        void update() override {
            if (!transform()) return;

            if (m_is_first_update) {
                m_is_first_update = false;
                //transform()->velocity = m_start_speed;
                m_prev_velocity.x = m_start_speed;
            }
            //Console::log("train position: ", transform()->position.x, " ", transform()->position.y, "\n");
            //Console::log("is_powered: ", m_is_powered, "\n");
            //Console::log("time_left_alive: ", m_time_left_alive, "\n");

            //Console::log("speed: ", m_speed, "\n");

            /*if (m_is_powered) {
                m_time_left_alive = U16_MAX;
            } else {
                m_time_left_alive = 0;
            }*/

            if (transform()->velocity.x < -0.1f || transform()->velocity.x > 0.1f) {
                m_prev_velocity.x = transform()->velocity.x;
            }
            if (transform()->velocity.y < -0.1f || transform()->velocity.y > 0.1f) {
                m_prev_velocity.y = transform()->velocity.y;
            }


            if (m_time_left_alive > 0) {
                if (m_start_speed > 0.0f && m_speed < m_start_speed) {
                    m_speed += 0.1f;
                } else if (m_start_speed < 0.0f && m_speed > m_start_speed) {
                    m_speed -= 0.1f;
                }
                if (m_start_speed > 0.0f && m_speed > m_start_speed ||
                    m_start_speed < 0.0f && m_speed < m_start_speed) {
                    m_speed = m_start_speed;
                }
                if (transform()->velocity.x == 0.0f && transform()->velocity.y == 0.0f) {
                    //Console::log("yo at zero\n");
                    transform()->velocity = m_prev_velocity;
                }                
            } else {
                m_speed *= 0.95f;
                if (m_speed > 0.0f && m_speed < 0.01f) {
                    m_speed = 0.0f;          
                } else if (m_speed < 0.0f && m_speed > -0.01f) {
                    m_speed = 0.0f;
                }
            }
            //Console::log("speed: ", m_speed, "\n");
            
            /*if (!m_is_second_update) {
                m_is_second_update = true;
                for (auto& i : m_aabb_ids) {
                    Console::log("aabb ", i, " name: ", aabb::name_as_string(aabb::Set::at(i)->name), "\n");
                }
            }*/
            
            

            //transform()->velocity.x = m_speed.x;
            //transform()->velocity.y = m_speed.y;
        }
    };
}