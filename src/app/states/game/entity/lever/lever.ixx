module;
#include <algorithm>
export module entity.lever;
import anim;
import console;
import entity;
import collider.aabb;

export namespace entity {
    class Lever : public Object {

        void flip_it() {
            if (m_time_left_dead == 0) {
                m_time_left_dead = U16_MAX;
                anim::Set::at(m_dead_anim_id)->source.x = 0;
                anim::Set::at(m_dead_anim_id)->speed = 0.5f;                
            } else {
                m_time_left_dead = 0;
                anim::Set::at(m_idle_anim_id)->source.x = 0;
                anim::Set::at(m_idle_anim_id)->speed = 0.5f;
            }

            /*for (auto& i : input_objects) {                
                i->time_left_dead(m_time_left_dead);
            }*/
            
        }
    public:
        Lever() {
            m_is_powered = true;
            m_time_left_dead = U16_MAX;
            m_time_left_alive = 0;
            //m_time_to_collide = 20;
            //load_config("res/entity/lever.cfg");
        }
        void collide_x(aabb::Infoc our, aabb::Infoc other) override {
            if (!our.owner || !other.owner || !aabb::Set::at(other.id)) return;
            //Console::log("Lever collide_x ", (int)m_time_left_dead, "\n");
            entity::Typec other_type = other.owner->type();
            aabb::Name other_aabb_name = aabb::Set::at(other.id)->name;

            if (other_aabb_name != aabb::Name::interact) {
                return;
            }
            
            if (other_type == entity::Type::player) {
                /*if (std::find(m_aabb_ids_to_ignore.begin(), m_aabb_ids_to_ignore.end(), other.id) != m_aabb_ids_to_ignore.end()) {
                    return;
                }*/
                //aabb::Set::at(other.id)->is_inactive(true);

                if (aabb::Set::at(other.id)) {
                    aabb::Set::at(other.id)->time_left_colliding = 10;
                }

                flip_it();
            }
        }
        void collide_y(aabb::Infoc our, aabb::Infoc other) override {
            //collide_x(our, other);
        }
        void update() override {
            if (m_current_anim_id != -1 && anim::Set::at(m_current_anim_id)) {
                //anim::Set::at(m_current_anim_id)->update();
                sprite()->source_rect.x = anim::Set::at(m_current_anim_id)->source.x;
                sprite()->source_rect.y = anim::Set::at(m_current_anim_id)->source.y;
            }
            /*if (m_time_left_colliding > 0) {
                Console::log("m_time_left_colliding: ", (int)m_time_left_colliding, "\n");
                --m_time_left_colliding;
            }*/
            //Console::log("num outputs: ", output_objects.size(), "\n");
            if (!input_objects.empty()) {
                //Console::log("trigger ", m_start_offset.x, " ", m_start_offset.y, " has ", input_objects.size(), " inputs\n");

                /*if (input_objects.back()->input_objects.empty()) {
                    input_objects.back()->add_input_object(std::shared_ptr<Object>(this));
                }*/
                
                //m_time_left_dead = input_objects.back()->is_dead() ? -1 : 0;
                if (input_objects.back()->is_dead() && m_time_left_dead == 0 ||
                    !input_objects.back()->is_dead() && m_time_left_dead > 0) {
                    flip_it();
                }
            }

            

            if (m_time_left_dead > 0) {
                m_time_left_alive = 0;
                //Console::log("m_time_left_dead: ", (int)m_time_left_dead, "\n");
                //Console::log("dead\n");
                if (m_time_left_dead > 0) {
                    if (m_time_left_dead != U16_MAX) { // u16 max
                        --m_time_left_dead;
                    }
                }
                m_current_anim_id = m_dead_anim_id; 
            } else {
                //Console::log("alive\n");
                m_time_left_alive = U16_MAX;
                m_current_anim_id = m_idle_anim_id;
            }


            /*for (auto& i : output_objects) {
                i->time_left_alive(m_time_left_alive);
            }*/

            if (anim::Set::at(m_current_anim_id)->source.x ==
                    (anim::Set::at(m_current_anim_id)->source.w * anim::Set::at(m_current_anim_id)->texture_size.x / anim::Set::at(m_current_anim_id)->source.w)
                    - anim::Set::at(m_current_anim_id)->source.w) {
                anim::Set::at(m_current_anim_id)->speed = 0.0f;
                // stop at last frame
            }

           
            if (m_is_first_state_update) {
                m_is_first_state_update = false;
                anim::Set::at(m_current_anim_id)->speed = 0.0f;
                anim::Set::at(m_current_anim_id)->source.x =
                    (anim::Set::at(m_current_anim_id)->source.w *
                    anim::Set::at(m_current_anim_id)->texture_size.x / anim::Set::at(m_current_anim_id)->source.w)
                    - anim::Set::at(m_current_anim_id)->source.w;
                // start at last frame
            }

        }
        
    };
}