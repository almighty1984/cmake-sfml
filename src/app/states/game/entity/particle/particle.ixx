export module entity.particle;
import anim;
import console;
import entity;
import collider.aabb;

export namespace entity {
    class Particle : public Object {
    protected:
        bool m_is_to_erase = false;
    public:        
        bool is_to_erase() const { return m_is_to_erase; }

        virtual void update_idle() {}
        virtual void update_dead() {}

        void update() override {
            if (m_is_first_update) {
                m_is_first_update = false;
                m_time_left_alive = m_time_to_be_alive;
                //m_start_offset = position_on_level();
            }

            /*if (m_time_left_alive == 0 && m_time_left_dead == 0) {
                m_is_to_erase = true;
                return;
            }*/

            

            if (m_time_left_hurt > 0) {
                --m_time_left_hurt;
            }
            
            if (m_next_state != m_state) {
                m_prev_state = m_state;
                m_state = m_next_state;
                m_is_first_state_update = true;
            }

            switch (m_state) {
                case entity::State::idle:
                    update_idle();
                    /*if (m_type == entity::Type::particle_bubble) {
                        Console::log("yo\n");
                    }*/
                    break;
                case entity::State::dead: update_dead(); break;
                default:                                 break;
            }

            if (m_time_left_alive > 0) {
                --m_time_left_alive;
                if (m_time_left_alive == 0) {
                    m_time_left_dead = m_time_to_be_dead;
                    if (m_time_to_be_dead == 0) {
                        m_is_to_erase = true;
                    }
                    //else {
                        //if (sprite()) sprite()->source_rect.x = 0;
                    //}
                }
                if (m_current_anim_id != m_idle_anim_id && m_idle_anim_id != -1) {
                    m_current_anim_id = m_idle_anim_id;
                    if (anim::Manager::get(m_idle_anim_id)) {
                        anim::Manager::get(m_idle_anim_id)->source.x = anim::Manager::get(m_idle_anim_id)->first_frame;
                    }
                }
            }
            else if (m_time_left_dead > 0) {
                transform()->velocity = {};

                //aabb::Manager::get(m_aabb_ids.back())->color({});
                //if (transform()) {
                    //transform()->velocity = {};
                //}
                //Console::log("m_time_left_dead: ", (int)m_time_left_dead, "\n");
                if (m_time_left_dead != U16_MAX) {
                    --m_time_left_dead;
                    if (m_time_left_dead == 0) {
                        m_is_to_erase = true;
                    }
                }                
                if (m_current_anim_id != m_dead_anim_id && m_dead_anim_id != -1) {
                    m_current_anim_id = m_dead_anim_id;
                    if (anim::Manager::get(m_dead_anim_id)) {
                        anim::Manager::get(m_dead_anim_id)->source.x = anim::Manager::get(m_dead_anim_id)->first_frame;
                    }
                }
            }

            if (sprite() && anim::Manager::get(m_current_anim_id)) {
                sprite()->source_rect.x = anim::Manager::get(m_current_anim_id)->source.x;
                sprite()->source_rect.y = anim::Manager::get(m_current_anim_id)->source.y;
            }
        }
    };
}