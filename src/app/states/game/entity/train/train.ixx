module;
#include <cmath>
#include <sstream>
export module entity.train;
import collider.aabb;
import anim;
import camera;
import console;
import entity;

export namespace entity {
    class Train : public Object {
        F32 m_speed       = 0.0F,
            m_start_speed = 1.0F;

        Vec2F m_prev_velocity = { 0.0F, 0.0F };
    public:
        //Train() {
            //m_speed = m_start_speed;
        //}
        void collide_x(aabb::cInfo our, aabb::cInfo other) override {
            if (!sprite() || !our.owner || !other.owner || !transform() || !aabb::Manager::get(our.id) || !aabb::Manager::get(other.id)) return;
            if (aabb::Manager::get(our.id)->points().size() < 4 || aabb::Manager::get(other.id)->points().size() < 4) return;

            aabb::cName our_aabb_name = aabb::Manager::get(our.id)->name;

            if (our_aabb_name != aabb::Name::bone) {
                return;
            }

            aabb::Name other_aabb_name = aabb::Manager::get(other.id)->name;

            //Console::log("aabb name: ", aabb::to_string(our_aabb_name), "\n");

            cRectF our_rect = { aabb::Manager::get(our.id)->points().at(0).x, aabb::Manager::get(our.id)->points().at(0).y,
                                aabb::Manager::get(our.id)->points().at(3).x, aabb::Manager::get(our.id)->points().at(3).y };

            entity::cType other_type = other.owner->type();
            cRectF        other_rect = { aabb::Manager::get(other.id)->points().at(0).x, aabb::Manager::get(other.id)->points().at(0).y,
                                         aabb::Manager::get(other.id)->points().at(3).x, aabb::Manager::get(other.id)->points().at(3).y };

            if (!entity::is_track(other_type) &&
                //other_type != entity::Type::player &&
                other_type != entity::Type::trigger &&
                other_type != entity::Type::track_trigger_UL) return;


            /*if (std::abs(transform()->velocity.y) > std::abs(transform()->velocity.x)) {
                collide_y(our, other);
                return;
            }*/

            cF32 diff_x = other_rect.x - our_rect.x;
            cF32 diff_y = other_rect.y - our_rect.y;

            cF32 overlap_x = our_rect.x < other_rect.x ? our_rect.w - other_rect.x : -(other_rect.w - our_rect.x);


            const bool is_larger_vel_x = std::abs(transform()->velocity.x) > std::abs(transform()->velocity.y);

            if (entity::is_track(other_type) || other_type == entity::Type::trigger) {
                m_is_powered = other.owner->is_powered();
                m_time_left_alive = other.owner->time_left_alive();
                //m_speed = m_start_speed;
                // 
                //Console::log("time_left_alive: ", other.owner->time_left_alive(), "\n");
                //Console::log("is_powered: ", m_is_powered, "\n");
            }

            if (other_type == entity::Type::track_U || other_type == entity::Type::track_D) {
                if (other_aabb_name == aabb::Name::left) {
                    if (our_rect.x > other_rect.w && transform()->velocity.x < 0.0F) {
                        transform()->velocity.x = 0.0F;
                        transform()->position.x -= overlap_x;
                    }
                }
                else if (other_aabb_name == aabb::Name::right) {
                    if (our_rect.w > other_rect.x && transform()->velocity.x > 0.0F) {
                        transform()->velocity.x = 0.0F;
                        transform()->position.x -= overlap_x;
                    }
                }
                else if (other_aabb_name == aabb::Name::track) {
                    if (transform()->velocity.y < 0.0F) {
                        transform()->velocity.y = -std::abs(m_speed);
                    } else if (transform()->velocity.y > 0.0F) {
                        transform()->velocity.y = std::abs(m_speed);
                    }
                    //if (transform()->velocity.x < 0.0F && our_rect.x < other_rect.x ||
                        //transform()->velocity.x > 0.0F && our_rect.w > other_rect.w) {
                    transform()->velocity.x = 0.0F;
                    transform()->velocity.x += diff_x * std::abs(m_speed);
                    //}
                }
            } else if (other_type == entity::Type::track_L || other_type == entity::Type::track_R ||
                       other_type == entity::Type::track_trigger_UL) {

                collide_y(our, other);
            } else if (other_type == entity::Type::track_UR || other_type == entity::Type::track_LD) {
                if (other_aabb_name == aabb::Name::right) {
                    if (our_rect.w > other_rect.x) {
                        transform()->velocity.x = 0.0F;
                        transform()->position.x -= overlap_x;
                    }
                } else if (other_aabb_name == aabb::Name::left) {
                    if (transform()->velocity.x < 0.0F) {
                        if (transform()->velocity.y < -0.1F) {
                            transform()->velocity.x = 0.0F;
                            transform()->position.x -= overlap_x;
                        } else if (transform()->velocity.y > -0.1F) {
                            if (our_rect.x < other_rect.w) {
                                transform()->velocity.x = 0.0F;
                                transform()->position.x -= overlap_x;
                            }
                            //transform()->velocity.x += diff_x * std::abs(m_speed);
                            transform()->velocity.y = std::abs(m_speed);
                            //aabb::Manager::get(our.id)->time_left_colliding = 10;
                        }
                    }
                } else if (other_aabb_name == aabb::Name::track) {
                    if (transform()->velocity.x < -0.1F) {
                        transform()->velocity.x = -std::abs(m_speed);
                    } else if (transform()->velocity.x > 0.1F) {
                        transform()->velocity.x = std::abs(m_speed);
                    }
                }
            } else if (other_type == entity::Type::track_UL || other_type == entity::Type::track_RD) {
                if (other_aabb_name == aabb::Name::left) {
                    if (transform()->velocity.x < 0.0F) {
                        if (transform()->velocity.y < -0.1F) {
                            transform()->velocity.x = 0.0F;
                            transform()->position.x -= overlap_x;
                        }
                    }
                } else if (other_aabb_name == aabb::Name::right) {
                    if (transform()->velocity.x > 0.0F) {
                        if (transform()->velocity.y < -0.1F) {
                            transform()->velocity.x = 0.0F;
                            transform()->position.x -= overlap_x;
                        } else {
                            if (our_rect.w >= other_rect.x) {
                                transform()->velocity.x = 0.0F;
                                transform()->position.x -= overlap_x;
                            }
                            //transform()->velocity.x += diff_x * std::abs(m_speed);
                            transform()->velocity.y = std::abs(m_speed);
                            //aabb::Manager::get(our.id)->time_left_colliding = 10;
                        }
                    }
                } else if (other_aabb_name == aabb::Name::track) {
                    if (transform()->velocity.x < -0.1F) {
                        transform()->velocity.x = -std::abs(m_speed);
                    } else if (transform()->velocity.x > 0.1F) {
                        transform()->velocity.x = std::abs(m_speed);
                    }
                }
            } else if (other_type == entity::Type::track_LU || other_type == entity::Type::track_DR) {
                if (other_aabb_name == aabb::Name::right) {
                    if (transform()->velocity.x > 0.0F) {
                        if (transform()->velocity.y > 0.1F) {
                            transform()->velocity.x = 0.0F;
                            transform()->position.x -= overlap_x;
                        }
                    }
                } else if (other_aabb_name == aabb::Name::left) {
                    if (transform()->velocity.x < 0.0F) {
                        if (transform()->velocity.y > 0.1F) {
                            transform()->velocity.x = 0.0F;
                            transform()->position.x -= overlap_x;
                        } else if (transform()->velocity.y < 0.1F) {
                            if (our_rect.x < other_rect.w) {
                                transform()->velocity.x = 0.0F;
                                transform()->position.x -= overlap_x;
                            }
                            transform()->velocity.y = -std::abs(m_speed);
                        }
                    }
                } else if (other_aabb_name == aabb::Name::track) {
                    if (transform()->velocity.x < -0.1F) {
                        transform()->velocity.x = -std::abs(m_speed);
                    } else if (transform()->velocity.x > 0.1F) {
                        transform()->velocity.x = std::abs(m_speed);
                    }
                }
            } else if (other_type == entity::Type::track_RU || other_type == entity::Type::track_DL) {
                if (other_aabb_name == aabb::Name::left) {
                    if (transform()->velocity.x < 0.0F) {
                        if (transform()->velocity.y > 0.1F) {
                            transform()->velocity.x = 0.0F;
                            transform()->position.x -= overlap_x;
                        }
                    }
                } else if (other_aabb_name == aabb::Name::right) {
                    if (transform()->velocity.x > 0.0F) {
                        if (transform()->velocity.y > 0.1F) {
                            transform()->velocity.x = 0.0F;
                            transform()->position.x -= overlap_x;
                        } else if (transform()->velocity.y < 0.1F) {
                            if (our_rect.x < other_rect.w) {
                                transform()->velocity.x = 0.0F;
                                transform()->position.x -= overlap_x;
                            }
                            transform()->velocity.y = -std::abs(m_speed);
                        }
                    }
                } else if (other_aabb_name == aabb::Name::track) {
                    if (transform()->velocity.x < -0.1F) {
                        transform()->velocity.x = -std::abs(m_speed);
                    } else if (transform()->velocity.x > 0.1F) {
                        transform()->velocity.x = std::abs(m_speed);
                    }
                }
            } else if (other_type == entity::Type::track_DL_1x1 || other_type == entity::Type::track_RU_1x1) {
                if (other_aabb_name == aabb::Name::up) {
                    if (transform()->velocity.y > 0.0F && our_rect.y >= other_rect.y) {
                        transform()->velocity.x = -std::abs(m_speed);
                        transform()->velocity.y = std::abs(m_speed);
                    } else if (transform()->velocity.x > 0.0F && our_rect.x >= other_rect.x) {
                        transform()->velocity.x = 0.0F;
                        transform()->velocity.y = -std::abs(m_speed);
                    }
                    Console::log(entity::to_string(other_type), " up\n");
                    return;
                }
                else if (other_aabb_name == aabb::Name::down) {
                    if (is_larger_vel_x && transform()->velocity.x > 0.0F && our_rect.w >= other_rect.w) {
                        transform()->velocity.x = std::abs(m_speed) * 0.71F;
                        transform()->velocity.y = -std::abs(m_speed) * 0.71F;
                    }
                    else if (!is_larger_vel_x && transform()->velocity.y > 0.0F && our_rect.y >= other_rect.y) {
                        transform()->velocity.y = 0.0F;
                        transform()->velocity.x = -std::abs(m_speed);
                    }
                    return;
                }
                if (is_larger_vel_x && transform()->velocity.x < 0.0F && our_rect.x <= other_rect.x) {
                    transform()->velocity.x = -std::abs(m_speed) * 0.71F;
                    transform()->velocity.y = std::abs(m_speed) * 0.71F;
                } else if (is_larger_vel_x && transform()->velocity.x > 0.0F && our_rect.w >= other_rect.w) {
                    transform()->velocity.x = std::abs(m_speed) * 0.71F;
                    transform()->velocity.y = -std::abs(m_speed) * 0.71F;
                } else if (!is_larger_vel_x && transform()->velocity.y < 0.0F && our_rect.y <= other_rect.h) {
                    transform()->velocity.x = std::abs(m_speed) * 0.71F;
                    transform()->velocity.y = -std::abs(m_speed) * 0.71F;
                } else if (!is_larger_vel_x && transform()->velocity.y > 0.0F && our_rect.y >= other_rect.y) {
                    transform()->velocity.x = -std::abs(m_speed) * 0.71F;
                    transform()->velocity.y = std::abs(m_speed) * 0.71F;
                }
            }
            else if (other_type == entity::Type::track_LD_1x1 || other_type == entity::Type::track_UR_1x1) {
                //if (other_aabb_name == aabb::Name::up) {
                //    if (transform()->velocity.x < 0.0F && our_rect.x <= other_rect.w) {
                //        transform()->velocity.x = -std::abs(m_speed) * 0.71F;
                //        transform()->velocity.y = std::abs(m_speed) * 0.71F;
                //    } else if (transform()->velocity.y < 0.0F && our_rect.y <= other_rect.h) {
                //        transform()->velocity.x = std::abs(m_speed) * 0.71F;
                //        transform()->velocity.y = std::abs(m_speed) * 0.71F;;
                //    }

                //    Console::log("train up\n");
                //    return;
                //}
                //else if (other_aabb_name == aabb::Name::down) {
                //    if (transform()->velocity.y < 0.0F && our_rect.y <= other_rect.h) {
                //        transform()->velocity.x = std::abs(m_speed) * 0.71F;
                //        transform()->velocity.y = -std::abs(m_speed) * 0.71F;
                //    }
                //    else if (transform()->velocity.y > 0.0F && our_rect.x >= other_rect.x) {
                //        transform()->velocity.x = std::abs(m_speed) * 0.71F;
                //        transform()->velocity.y = std::abs(m_speed) * 0.71F;
                //    }
                //    //else if (transform()->velocity.x < 0.0F && our_rect.w <= other_rect.w) {
                //    //    transform()->velocity.x = 0.0F;
                //    //    transform()->velocity.y = std::abs(m_speed);                        
                //    //}
                //    Console::log("train down\n");
                //    return;
                //}
                if (is_larger_vel_x) {
                    if (other_aabb_name == aabb::Name::left) {
                        if (transform()->velocity.x < 0.0F && our_rect.x < other_rect.w) {
                            transform()->position.x -= overlap_x;
                            if (transform()->velocity.x < 0.0F) {
                                transform()->velocity.y = -transform()->velocity.x;
                            }
                        }
                    }
                    else if (other_aabb_name == aabb::Name::right) {
                        if (transform()->velocity.x > 0.0F && our_rect.w > other_rect.x) {
                            transform()->position.x -= overlap_x;                            
                        }
                    } else if (other_aabb_name == aabb::Name::track) {
                        if (transform()->velocity.x < 0.0F && our_rect.x <= other_rect.x) {
                            transform()->velocity.x = -std::abs(m_speed) * 0.71F;
                            transform()->velocity.y = std::abs(m_speed) * 0.71F;
                            //transform()->velocity.x += diff_x / 16.0F;
                            transform()->velocity.x += diff_x / 16.0F * std::abs(m_speed);
                        } else if (transform()->velocity.x > 0.0F && our_rect.w >= other_rect.w) {
                            transform()->velocity.x = std::abs(m_speed) * 0.71F;
                            transform()->velocity.y = -std::abs(m_speed) * 0.71F;
                            transform()->velocity.x += diff_x / 16.0F * std::abs(m_speed);
                        }
                    }
                }
                //else {
                    //if (transform()->velocity.y < 0.0F && our_rect.y <= other_rect.h) {
                    //    transform()->velocity.x = std::abs(m_speed) * 0.71F;
                    //    transform()->velocity.y = -std::abs(m_speed) * 0.71F;
                    //    //transform()->velocity.x += diff_x / 16.0F;
                    //    transform()->velocity.y += diff_y / 16.0F * std::abs(m_speed);
                    //} else if (transform()->velocity.y > 0.0F && our_rect.y >= other_rect.y) {
                    //    transform()->velocity.x = -std::abs(m_speed) * 0.71F;
                    //    transform()->velocity.y = std::abs(m_speed) * 0.71F;
                    //    transform()->velocity.y += diff_y / 16.0F * std::abs(m_speed);
                    //}
                //}
            } else if (other_type == entity::Type::track_LU_1x1 || other_type == entity::Type::track_UL_1x1 ||
                       other_type == entity::Type::track_RD_1x1 || other_type == entity::Type::track_DR_1x1) {
                    //if (!is_larger_vel_x) return;

                    if (other_aabb_name == aabb::Name::left) {
                        if (transform()->velocity.x < 0.0F && our_rect.x < other_rect.w) {
                            transform()->position.x -= overlap_x;
                        }
                    } else if (other_aabb_name == aabb::Name::right) {
                        if (transform()->velocity.x > 0.0F && our_rect.w > other_rect.x) {
                            transform()->position.x -= overlap_x;
                        }
                    } else if (other_aabb_name == aabb::Name::track) {
                        if (is_larger_vel_x && transform()->velocity.x < 0.0F && our_rect.x <= other_rect.x) {
                            transform()->velocity.x = -std::abs(m_speed) * 0.71F;
                            transform()->velocity.y = -std::abs(m_speed) * 0.71F;

                            //transform()->velocity.x += diff_x / 16.0F;

                            //transform()->velocity.y += diff_y / 16.0F * std::abs(m_speed);
                        } else if (is_larger_vel_x && transform()->velocity.x > 0.0F && our_rect.x >= other_rect.x) {
                            transform()->velocity.x = std::abs(m_speed) * 0.71F;
                            transform()->velocity.y = std::abs(m_speed) * 0.71F;

                            //transform()->velocity.x += diff_x / 16.0F;

                            //transform()->velocity.y += diff_y / 16.0F * std::abs(m_speed);
                        } else if (!is_larger_vel_x && transform()->velocity.y < 0.0F && our_rect.y <= other_rect.y) {
                            transform()->velocity.x = -std::abs(m_speed) * 0.71F;
                            transform()->velocity.y = -std::abs(m_speed) * 0.71F;
                        } else if (!is_larger_vel_x && transform()->velocity.y > 0.0F && our_rect.y >= other_rect.y) {
                            transform()->velocity.x = std::abs(m_speed) * 0.71F;
                            transform()->velocity.y = std::abs(m_speed) * 0.71F;
                        }
                    }
            } else if (other_type == entity::Type::track_LU_2x1_0 || other_type == entity::Type::track_LU_2x1_1 ||
                       other_type == entity::Type::track_RD_2x1_0 || other_type == entity::Type::track_RD_2x1_1) {

                if (transform()->velocity.x < 0.0F) {
                    transform()->velocity.x = -std::abs(m_speed);
                } else if (transform()->velocity.x > 0.0F) {
                    transform()->velocity.x = std::abs(m_speed);
                }
                transform()->velocity.y = transform()->velocity.x / 2.0F;

                //transform()->velocity.x += diff_x / 16.0F;
                //transform()->velocity.y += diff_y / 16.0F * std::abs(m_speed);
            } else if (other_type == entity::Type::track_LD_2x1_0 || other_type == entity::Type::track_LD_2x1_1) {
                if (is_larger_vel_x && transform()->velocity.x < 0.0F || !is_larger_vel_x && transform()->velocity.y < 0.0F) {
                    transform()->velocity.x = -std::abs(m_speed);
                } else if (is_larger_vel_x && transform()->velocity.x > 0.0F && !is_larger_vel_x && transform()->velocity.y > 0.0F) {
                    transform()->velocity.x = std::abs(m_speed);
                }
                transform()->velocity.y = -transform()->velocity.x / 2.0F;
                

                //transform()->velocity.y = -transform()->velocity.x / 2.0F;
                //transform()->velocity.y += diff_y / 16.0F * std::abs(m_speed);


            } else if (other_type == entity::Type::track_RU_2x1_0 || other_type == entity::Type::track_RU_2x1_1) {
                if (transform()->velocity.x < 0.0F) {
                    transform()->velocity.x = -std::abs(m_speed);
                } else if (transform()->velocity.x > 0.0F) {
                    transform()->velocity.x = std::abs(m_speed);
                }
                transform()->velocity.y = -transform()->velocity.x / 2.0F;

                //transform()->velocity.x += diff_x / 16.0F;

                transform()->velocity.y += diff_y / 16.0F * std::abs(m_speed);

            } else if (other_type == entity::Type::track_UR_1x2_0 || other_type == entity::Type::track_UR_1x2_1 ||
                       other_type == entity::Type::track_DL_1x2_0 || other_type == entity::Type::track_DL_1x2_1) {

                /*if (!is_larger_vel_x) {
                    if (transform()->velocity.y < 0.0F) {
                        transform()->velocity.y = -std::abs(m_speed);
                    }
                    else if (transform()->velocity.y > 0.0F) {
                        transform()->velocity.y = std::abs(m_speed);
                    }
                } else {
                    if (transform()->velocity.x < 0.0F) {
                        transform()->velocity.x = -transform()->velocity.y / 2.0F;
                    } else if (transform()->velocity.x > 0.0F) {
                        transform()->velocity.x = transform()->velocity.y / 2.0F;
                    }
                }*/
                if (!is_larger_vel_x && transform()->velocity.y < 0.0F || is_larger_vel_x && transform()->velocity.x > 0.0F) {
                    if (our_rect.y <= other_rect.h - 4) {
                        transform()->velocity.y = -std::abs(m_speed);
                        //transform()->position.x += diff_x / 64.0F;
                        transform()->velocity.x = -transform()->velocity.y / 2.0F;
                        //transform()->velocity.x += diff_x / 16.0F;
                    }
                } else if (!is_larger_vel_x && transform()->velocity.y > 0.0F || is_larger_vel_x && transform()->velocity.x < 0.0F) {
                    if (our_rect.h >= other_rect.y + 4) {
                        transform()->velocity.y = std::abs(m_speed);
                        transform()->velocity.x = -transform()->velocity.y / 2.0F;
                        //transform()->velocity.x += diff_x / 16.0F;
                    }
                }
            } else if (other_type == entity::Type::track_UL_1x2_0 || other_type == entity::Type::track_UL_1x2_1 ||
                       other_type == entity::Type::track_DR_1x2_0 || other_type == entity::Type::track_DR_1x2_1) {
                
                if (other_aabb_name == aabb::Name::left) {
                    if (transform()->velocity.x < 0.0F && our_rect.x < other_rect.w) {
                        transform()->position.x -= overlap_x;                        
                    }
                } else if (other_aabb_name == aabb::Name::right) {
                    if (transform()->velocity.x > 0.0F && our_rect.w > other_rect.x) {
                        transform()->position.x -= overlap_x;
                    }
                } else if (other_aabb_name == aabb::Name::track) {
                    if (transform()->velocity.y < 0.0F && our_rect.y <= other_rect.y) {
                        transform()->velocity.y = -std::abs(m_speed);
                        transform()->velocity.x = transform()->velocity.y / 2.0F;
                    } else if (transform()->velocity.y > 0.0F && our_rect.y >= other_rect.y) {
                        transform()->velocity.y = std::abs(m_speed);
                        transform()->velocity.x = transform()->velocity.y / 2.0F;
                    }
                    //transform()->position.x += diff_x / 8.0F;
                    //transform()->velocity.x += diff_x / 16.0F * std::abs(m_speed);
                }
            }
        }
        void collide_y(aabb::cInfo our, aabb::cInfo other) override {
            if (!sprite() || !our.owner || !other.owner || !transform() || !aabb::Manager::get(our.id) || !aabb::Manager::get(other.id)) return;
            if (aabb::Manager::get(our.id)->points().size() < 4 || aabb::Manager::get(other.id)->points().size() < 4) return;



            aabb::cName our_aabb_name = aabb::Manager::get(our.id)->name;

            if (our_aabb_name != aabb::Name::bone) {
                return;
            }

            cRectF our_rect = { aabb::Manager::get(our.id)->points().at(0).x, aabb::Manager::get(our.id)->points().at(0).y,
                                aabb::Manager::get(our.id)->points().at(3).x, aabb::Manager::get(our.id)->points().at(3).y };

            entity::cType other_type = other.owner->type();
            cRectF        other_rect = { aabb::Manager::get(other.id)->points().at(0).x, aabb::Manager::get(other.id)->points().at(0).y,
                                         aabb::Manager::get(other.id)->points().at(3).x, aabb::Manager::get(other.id)->points().at(3).y };
            aabb::cName other_aabb_name = aabb::Manager::get(other.id)->name;

            if (!entity::is_track(other_type) &&
                //other_type != entity::Type::player &&
                other_type != entity::Type::trigger &&
                other_type != entity::Type::track_trigger_UL) return;


            /*if (std::abs(transform()->velocity.x) > std::abs(transform()->velocity.y)) {
                collide_x(our, other);
                return;
            }*/
                

            cF32 diff_x = other_rect.x - our_rect.x;
            cF32 diff_y = other_rect.y - our_rect.y;

            cF32 overlap_y = our_rect.y < other_rect.y ? our_rect.h - other_rect.y : -(other_rect.h - our_rect.y);

            bool is_larger_vel_y = std::abs(transform()->velocity.y) > std::abs(transform()->velocity.x);

            //m_speed = 0.0F;
            if (entity::is_track(other_type) || other_type == entity::Type::trigger) {
                m_is_powered = other.owner->is_powered();
                m_time_left_alive = other.owner->time_left_alive();
                //m_speed = m_start_speed;
                //Console::log("time_left_alive: ", other.owner->time_left_alive(), "\n");
                //Console::log("is_powered: ", m_is_powered, "\n");
            }

            if (other_type == entity::Type::track_U || other_type == entity::Type::track_D) {
                //collide_x(our, other);
                if (transform()->velocity.y < 0.0F && our_rect.h < other_rect.h) {
                    transform()->velocity.y = -std::abs(m_speed);
                } else if (transform()->velocity.y > 0.0F && our_rect.y > other_rect.y) {
                    transform()->velocity.y = std::abs(m_speed);
                }
            } else if (other_type == entity::Type::track_L || other_type == entity::Type::track_R || other_type == entity::Type::track_trigger_UL) {
                if (other_aabb_name == aabb::Name::up) {
                    if (our_rect.y < other_rect.h) {
                        transform()->velocity.y = 0.0F;
                        transform()->position.y -= overlap_y;
                    }
                } else if (other_aabb_name == aabb::Name::down) {
                    if (our_rect.h > other_rect.y) {
                        transform()->velocity.y = 0.0F;
                        transform()->position.y -= overlap_y;
                    }
                } else if (other_aabb_name == aabb::Name::track) {
                    if (transform()->velocity.x < 0.0F) {
                        transform()->velocity.x = -std::abs(m_speed);
                    } else if (transform()->velocity.x > 0.0F) {
                        transform()->velocity.x = std::abs(m_speed);
                    } else {
                        if (m_prev_velocity.x < 0.0F) {
                            transform()->velocity.x = -std::abs(m_speed);
                        } else {
                            transform()->velocity.x = std::abs(m_speed);
                        }
                    }
                    transform()->velocity.y = diff_y / 16.0F * std::abs(m_speed);
                }
            } else if (other_type == entity::Type::track_UR || other_type == entity::Type::track_LD) {
                if (other_aabb_name == aabb::Name::up) {
                    if (transform()->velocity.y < 0.0F) {
                        if (our_rect.y < other_rect.h) {
                            transform()->velocity.y = 0.0F;
                            transform()->velocity.x = std::abs(m_speed);
                        }
                    }
                } else if (other_aabb_name == aabb::Name::track) {
                    if (transform()->velocity.y < -0.1F) {
                        transform()->velocity.y = -std::abs(m_speed);
                    } else if (transform()->velocity.y > 0.1F) {
                        transform()->velocity.y = std::abs(m_speed);
                    }
                }
            } else if (other_type == entity::Type::track_UL || other_type == entity::Type::track_RD) {
                if (other_aabb_name == aabb::Name::up) {
                    if (transform()->velocity.y < 0.0F) {
                        if (our_rect.y < other_rect.h) {
                            transform()->velocity.y = 0.0F;
                            transform()->velocity.x = -std::abs(m_speed);
                        }
                    }
                } else if (other_aabb_name == aabb::Name::track) {
                    if (transform()->velocity.y < -0.1F) {
                        transform()->velocity.y = -std::abs(m_speed);
                    } else if (transform()->velocity.y > 0.1F) {
                        transform()->velocity.y = std::abs(m_speed);
                    }
                }
            } else if (other_type == entity::Type::track_LU || other_type == entity::Type::track_DR) {
                if (other_aabb_name == aabb::Name::down) {
                    if (transform()->velocity.y > 0.0F) {
                        if (our_rect.h > other_rect.y) {
                            transform()->velocity.y = 0.0F;
                            transform()->velocity.x = std::abs(m_speed);
                        }
                    }
                } else if (other_aabb_name == aabb::Name::track) {
                    if (transform()->velocity.y < -0.1F) {
                        transform()->velocity.y = -std::abs(m_speed);
                    } else if (transform()->velocity.y > 0.1F) {
                        transform()->velocity.y = std::abs(m_speed);
                    }
                }
            } else if (other_type == entity::Type::track_RU || other_type == entity::Type::track_DL) {
                if (other_aabb_name == aabb::Name::down) {
                    if (transform()->velocity.y > 0.0F) {
                        if (our_rect.h > other_rect.y) {
                            transform()->velocity.y = 0.0F;
                            transform()->velocity.x = -std::abs(m_speed);
                        }
                    }
                } else if (other_aabb_name == aabb::Name::track) {
                    if (transform()->velocity.y < -0.1F) {
                        transform()->velocity.y = -std::abs(m_speed);
                    } else if (transform()->velocity.y > 0.1F) {
                        transform()->velocity.y = std::abs(m_speed);
                    }
                }
            }             
            else if (other_type == entity::Type::track_LD_1x1 || other_type == entity::Type::track_DL_1x1 ||
                     other_type == entity::Type::track_RU_1x1 || other_type == entity::Type::track_UR_1x1) {
                if (!is_larger_vel_y) {
                    //collide_x(our, other);
                    return;
                }
                if (other_aabb_name == aabb::Name::left) {
                    if (our_rect.y < other_rect.h) {                        
                        transform()->position.y -= overlap_y;
                    }
                } else if (other_aabb_name == aabb::Name::right) {
                    if (our_rect.h > other_rect.y) {                        
                        transform()->position.y -= overlap_y;
                    }
                } else if (other_aabb_name == aabb::Name::track) {
                    if (transform()->velocity.y < 0.0F && our_rect.y <= other_rect.h) {
                        transform()->velocity.x = std::abs(m_speed) * 0.71F;
                        transform()->velocity.y = -std::abs(m_speed) * 0.71F;

                        //transform()->velocity.x += diff_x / 16.0F;
                        transform()->velocity.y += diff_y / 16.0F * std::abs(m_speed);
                    } else if (transform()->velocity.y > 0.0F && our_rect.y >= other_rect.y) {
                        transform()->velocity.x = -std::abs(m_speed) * 0.71F;
                        transform()->velocity.y = std::abs(m_speed) * 0.71F;

                        transform()->velocity.y += diff_y / 16.0F * std::abs(m_speed);
                    }
                }
            } else if (other_type == entity::Type::track_RD_1x1 || other_type == entity::Type::track_DR_1x1 ||
                       other_type == entity::Type::track_LU_1x1 || other_type == entity::Type::track_UL_1x1) {
                if (!is_larger_vel_y) return;

                if (other_aabb_name == aabb::Name::left) {
                    if (transform()->velocity.y > 0.0f && our_rect.h > other_rect.y) {
                        transform()->position.y -= overlap_y;
                    }
                } else if (other_aabb_name == aabb::Name::right) {
                    if (transform()->velocity.y < 0.0f && our_rect.y < other_rect.h) {
                        transform()->position.y -= overlap_y;
                    }
                } else if (other_aabb_name == aabb::Name::track) {
                    //Console::log("velocity: ", transform()->velocity.x, " ", transform()->velocity.y, "\n");
                    if (transform()->velocity.x < 0.0F && transform()->velocity.y < 0.0F) {
                        transform()->velocity.x = -std::abs(m_speed) * 0.71F;
                        transform()->velocity.y = -std::abs(m_speed) * 0.71F;
                    } else if (transform()->velocity.x > 0.0F && transform()->velocity.y > 0.0F) {
                        transform()->velocity.x = std::abs(m_speed) * 0.71F;
                        transform()->velocity.y = std::abs(m_speed) * 0.71F;
                    }
                }
            } else if (other_type == entity::Type::track_UL_1x2_0 || other_type == entity::Type::track_UL_1x2_1 ||
                       other_type == entity::Type::track_DR_1x2_0 || other_type == entity::Type::track_DR_1x2_1 ||
                       other_type == entity::Type::track_UR_1x2_0 || other_type == entity::Type::track_UR_1x2_1 ||
                       other_type == entity::Type::track_DL_1x2_0 || other_type == entity::Type::track_DL_1x2_1) {
                /*if (transform()->velocity.y < 0.0F && our_rect.y < other_rect.y) {
                    transform()->velocity.y = -std::abs(m_speed);
                } else if(transform()->velocity.y > 0.0F && our_rect.h > other_rect.h) {
                    transform()->velocity.y = std::abs(m_speed);
                }*/
                collide_x(our, other);


            } else if (other_type == entity::Type::track_LU_2x1_0 || other_type == entity::Type::track_LU_2x1_1 ||
                       other_type == entity::Type::track_RD_2x1_0 || other_type == entity::Type::track_RD_2x1_1) {
                //collide_x(our, other);

                if (transform()->velocity.y < 0.0F) {
                    transform()->velocity.x = -std::abs(m_speed);
                } else if (transform()->velocity.y > 0.0F) {
                    transform()->velocity.x = std::abs(m_speed);
                }
                transform()->velocity.y = transform()->velocity.x / 2.0F;
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

            
            if (transform()->velocity.x < -0.05F || transform()->velocity.x > 0.05F) {
                m_prev_velocity.x = transform()->velocity.x;
            }
            if (transform()->velocity.y < -0.05F || transform()->velocity.y > 0.05F) {
                m_prev_velocity.y = transform()->velocity.y;
            }

            if (m_time_left_alive > 0) {
                if (m_start_speed > 0.0F && m_speed < m_start_speed) {
                    m_speed += 0.2F;
                } else if (m_start_speed < 0.0F && m_speed > m_start_speed) {
                    m_speed -= 0.2F;
                }
                if (m_start_speed > 0.0F && m_speed > m_start_speed ||
                    m_start_speed < 0.0F && m_speed < m_start_speed) {
                    m_speed = m_start_speed;
                }
                if (transform()->velocity.x >= -0.05F && transform()->velocity.x <= 0.05F &&
                    transform()->velocity.y >= -0.05F && transform()->velocity.y <= 0.05F) {
                    //Console::log("entity::Train at zero\n");
                    transform()->velocity = m_prev_velocity;
                }
            } else {
                m_speed *= 0.9f;
                if (m_speed > 0.0F && m_speed < 0.01f) {
                    m_speed = 0.0F;          
                } else if (m_speed < 0.0F && m_speed > -0.01f) {
                    m_speed = 0.0F;
                }
            }
            //Console::log("speed: ", m_speed, "\n");
            
            /*if (!m_is_second_update) {
                m_is_second_update = true;
                for (auto& i : m_aabb_ids) {
                    Console::log("aabb ", i, " name: ", aabb::name_as_string(aabb::Manager::get(i)->name), "\n");
                }
            }*/
            
            

            //transform()->velocity.x = m_speed.x;
            //transform()->velocity.y = m_speed.y;
        }
    };
}