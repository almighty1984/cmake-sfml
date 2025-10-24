module;
#include <cmath>
#include <sstream>

export module entity.water_line;
import anim;
//import camera;
import console;
import entity;
import collider.aabb;
//import entity.particle;
import line;
import random;

export namespace entity {
    class WaterLine : public Object {
        F32 m_start_y,
            m_line_offset_y;
        I32 m_top_line_id,
            m_bottom_line_id;
        U8 m_timer,
           m_top_line_size,
           m_bottom_line_size;

    public:
        WaterLine() : m_start_y(0.0F),
                      m_line_offset_y(4.0F),
                      m_top_line_id(-1),
                      m_bottom_line_id(-1),
                      m_timer(0),
                      m_top_line_size(2),
                      m_bottom_line_size(3) {
            m_input_limit = 2;
        }
        ~WaterLine() {
            //Console::log("~WaterLine()\n\n\n");
            line::Manager::erase(m_top_line_id);        m_top_line_id    = -1;
            line::Manager::erase(m_bottom_line_id);     m_bottom_line_id = -1;
        }
        void collide_x(aabb::cInfo our, aabb::cInfo other) override {
            //if (other.owner && other.owner->type() == entity::Type::player) {
                collide_y(our, other);
            //}
        }
        void collide_y(aabb::cInfo our, aabb::cInfo other) override {
            if (!our.owner || !other.owner || !transform() || !aabb::Manager::get(our.id) || !aabb::Manager::get(other.id)) return;
            if (aabb::Manager::get(our.id)->points().size() < 4 || aabb::Manager::get(other.id)->points().size() < 4) return;
            
            cRectF our_rect = { aabb::Manager::get(our.id)->points().at(0).x, aabb::Manager::get(our.id)->points().at(0).y,
                                aabb::Manager::get(our.id)->points().at(3).x, aabb::Manager::get(our.id)->points().at(3).y };

            entity::cType other_type = other.owner->type();

            if (other_type != entity::Type::player) {
                return;
            }

            cRectF        other_rect = { aabb::Manager::get(other.id)->points().at(0).x, aabb::Manager::get(other.id)->points().at(0).y,
                                         aabb::Manager::get(other.id)->points().at(3).x, aabb::Manager::get(other.id)->points().at(3).y };                        
                        
            cF32 overlap_y = our_rect.y < other_rect.y ? our_rect.h - other_rect.y : -(other_rect.h - our_rect.y);

            
            if (other_type == entity::Type::player) {                

                cF32 start_diff_y = m_start_y - position_on_level().y;
                if (std::abs(start_diff_y) > 16.0F) {
                    return;
                }
                //Console::log("start_diff_y: ", start_diff_y, "\n");

                //if (transform()->velocity.y < 0.0F)
                    //transform()->velocity.y = other.owner->velocity().y * 1.0F;
                //else
                    transform()->velocity.y = other.owner->velocity().y * 0.3F;

                //if (std::abs(start_diff_y) < 8.0F/* && std::abs(other.owner->transform()->velocity.y) > 0.5F*/) {
                    //transform()->velocity.y = other.owner->velocity().y * 0.9F;
                    //transform()->moved_velocity.y = other.owner->velocity().y * 1.0F;
                //}
                    //transform()->velocity.y = other.owner->velocity().y / std::abs(start_diff_y);
                    //transform()->velocity.y = other.owner->velocity().y * 1.0F;

                    if (transform()->velocity.y < 0.0F && transform()->velocity.y < -transform()->max_velocity.y) {
                        transform()->velocity.y = -transform()->max_velocity.y;
                    } else if (transform()->velocity.y > 0.0F && transform()->velocity.y > transform()->max_velocity.y) {
                        transform()->velocity.y = transform()->max_velocity.y;
                    }

                    /*if (input_objects.front()) {
                        input_objects.front()->transform()->velocity.y = transform()->velocity.y * 0.3F + input_objects.front()->transform()->velocity.y * 0.5F;
                        if (input_objects.front()->input_objects.front()) {
                            input_objects.front()->input_objects.front()->transform()->velocity.y = input_objects.front()->transform()->velocity.y;
                        }
                    }
                    if (input_objects.back()) {
                        input_objects.back()->transform()->velocity.y = transform()->velocity.y * 0.3F + input_objects.back()->transform()->velocity.y * 0.5F;
                        if (input_objects.back()->input_objects.back()) {
                            input_objects.back()->input_objects.back()->transform()->velocity.y = input_objects.back()->transform()->velocity.y;
                        }
                    }*/

                    
                //}
            }
        }

        virtual void draw(std::unique_ptr<Window>& window) override {
            window->draw(line::Manager::get(m_top_line_id));
            window->draw(line::Manager::get(m_bottom_line_id));
        }

        void update() override {
            if (!transform()) return;

            if (input_objects.empty()) return;

            if (m_is_first_update) {
                m_is_first_update = false;

                transform()->velocity.y = random::Generator::real_number(-0.1, 0.1);

                transform()->max_velocity = { 0.0F, 1.2F };
                //Console::log("water input: ", input_objects.size(), "\n");

                m_start_y = position_on_level().y;

                m_top_line_id = line::Manager::make(position() + Vec2F{ 0.0F, 2.0F },
                    input_objects.back()->position() + Vec2F{ 0.0F, m_line_offset_y });                    
                line::Manager::get(m_top_line_id)->size = m_top_line_size;
                line::Manager::get(m_top_line_id)->color = { 52, 255, 255 };
                //line::Manager::get(m_top_line_id)->layer = NUM_VISIBLE_LAYERS - 3;
                line::Manager::get(m_top_line_id)->layer = m_start_layer;


                m_bottom_line_id = line::Manager::make(position() + Vec2F{ 0.0F, 4.0F},
                                            input_objects.back()->position()  + Vec2F{  0.0F, m_line_offset_y });                    
                line::Manager::get(m_bottom_line_id)->size = m_bottom_line_size;
                line::Manager::get(m_bottom_line_id)->color = { 52, 206, 206 };
                //line::Manager::get(m_bottom_line_id)->layer = NUM_VISIBLE_LAYERS - 2;
                line::Manager::get(m_bottom_line_id)->layer = m_start_layer;

            }


            /*if (transform()->velocity.y < 0.0F && transform()->velocity.y < -transform()->max_velocity.y) {
                transform()->velocity.y = -transform()->max_velocity.y;
                Console::log("velocity y: ", transform()->velocity.y, "\n");
            } else if (transform()->velocity.y > 0.0F && transform()->velocity.y > transform()->max_velocity.y) {
                transform()->velocity.y = transform()->max_velocity.y;
            }*/
            


            cF32 diff_y = position_on_level().y - m_start_y;
            transform()->velocity.y -= diff_y * 0.025f;

            //if (transform()->velocity.y < -1.0F || transform()->velocity.y > 1.0F)
                //transform()->velocity.y *= 0.9F;

            if (std::abs(transform()->velocity.y) > 0.1F) {
                transform()->velocity.y *= 0.93f;
            } else {
                /*if (diff_y < 0.0F && diff_y > -0.04f) {
                    diff_y = -0.04f;
                } else if (diff_y >= 0.0F && diff_y < 0.04f) {
                    diff_y = 0.04f;
                }*/
            }

            if (m_timer < 2) {
                ++m_timer;
            }
            else {
                m_timer = 0;

                if (input_objects.size() == 2) {
                    if (std::abs(transform()->velocity.y) > std::abs(input_objects.front()->transform()->velocity.y)) {
                        //input_objects.front()->transform()->velocity.y = transform()->velocity.y * 0.95f + input_objects.front()->transform()->velocity.y * 0.1F;
                        input_objects.front()->transform()->velocity.y = transform()->velocity.y * 0.99f;
                    }
                    if (std::abs(transform()->velocity.y) > std::abs(input_objects.back()->transform()->velocity.y)) {
                        //input_objects.back()->transform()->velocity.y = transform()->velocity.y * 0.95f + input_objects.back()->transform()->velocity.y * 0.1F;
                        input_objects.back()->transform()->velocity.y = transform()->velocity.y * 0.99f;
                    }
                }


                
            }

            //transform()->moved_velocity.y -= diff_y * 0.1F;

            //transform()->velocity.y *= 0.9F;
            
            //transform()->velocity.y *= 0.9F;

            /*if (diff_y < 0.0F && diff_y < -16.0F) {
                transform()->position.y += ((m_start_y - 8.0F) - position_on_level().y);
            }
            if (diff_y > 0.0F && diff_y > 16.0F) {
                transform()->position.y += ((m_start_y + 8.0F) - position_on_level().y);
            }*/

            if (line::Manager::get(m_top_line_id) && line::Manager::get(m_bottom_line_id)) {
                //line::Manager::get(m_line_id)->start = input_objects.front()->position() + Vec2F{ -0.5F, 8.0F };
                
                cF32 half_w = 1.0F;

                cF32 size_offset = m_top_line_size / 4.0F;

                
                //Console::log("water line size offset: ", size_offset, "\n\n");
                

                Vec2F start = position(), 
                      end   = position() + Vec2F{ 0.0F, -8.0F };
                if (input_objects.size() == 1) {
                    if (position().x > input_objects.back()->position().x) {
                        // R
                        start = input_objects.back()->position() + Vec2F{ half_w - size_offset, m_line_offset_y };
                        end   = position() + Vec2F{ half_w * 2 + size_offset + 1.5F, m_line_offset_y };                        
                    } else {
                        // L
                        start = position() + Vec2F{ -0.1F, m_line_offset_y };
                        end   = input_objects.back()->position() + Vec2F{ half_w + size_offset, m_line_offset_y };                        
                    }
                } else if (input_objects.size() == 2) {
                    start = input_objects.front()->position() + Vec2F{ half_w - size_offset, m_line_offset_y };
                    end   = input_objects.back()->position()  + Vec2F{ half_w + size_offset, m_line_offset_y };
                }

                line::Manager::get(m_top_line_id)->start = start;
                line::Manager::get(m_top_line_id)->end   = end;

                line::Manager::get(m_bottom_line_id)->start = start + Vec2F{ 0.0F, size_offset * 1.0F };
                line::Manager::get(m_bottom_line_id)->end   = end   + Vec2F{ 0.0F, size_offset * 1.0F};
                

                //line::Manager::get(m_top_line_id)->update();
                //line::Manager::get(m_bottom_line_id)->update();
            }

            
        }
    };
}