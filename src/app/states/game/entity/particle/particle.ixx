export module entity.particle;
import anim;
import console;
import entity;
import collider.aabb;

export namespace entity {
    class Particle : public Object {
        bool m_is_to_erase = false;
    public:          
        bool is_to_erase() const { return m_is_to_erase; }

        void update() override {
            if (m_is_first_update) {
                m_is_first_update = false;
                m_time_left_alive = m_time_to_be_alive;
            }
            
            if (sprite() && anim::Set::at(m_current_anim_id)) {
                sprite()->source_rect.x = anim::Set::at(m_current_anim_id)->source.x;
                sprite()->source_rect.y = anim::Set::at(m_current_anim_id)->source.y;
            }

            if (m_time_left_alive > 0) {
                --m_time_left_alive;
                if (m_time_left_alive == 0) {
                    m_time_left_dead = m_time_to_be_dead;
                    if (m_time_left_dead == 0) {
                        m_is_to_erase = true;                        
                    }
                }
                m_current_anim_id = m_idle_anim_id;
            }
            else if (m_time_left_dead > 0) {
                //Console::log("m_time_left_dead: ", (int)m_time_left_dead, "\n");
                if (m_time_left_dead != U16_MAX) { // u16 max, remain dead
                    --m_time_left_dead;
                    if (m_time_left_dead == 0) {
                        m_is_to_erase = true;
                    }
                }
                m_current_anim_id = m_dead_anim_id;
            }
        }
    };
}