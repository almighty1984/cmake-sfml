export module entity.conduit;
import anim;
import console;
import entity;
import collider.aabb;

export namespace entity {
    class Conduit : public Object {
    public:
        void update() override {
            if (!sprite()) return;

            m_input_limit = 1;
            m_time_left_alive = 0;

            //Console::log("num inputs: ", input_objects.size(), "\n");
            m_is_powered = false;
            m_time_left_alive = 0;
            for (auto& i : input_objects) {
                if (i->is_powered()) {
                    m_is_powered = true;
                    if (i->time_left_alive() > 0) {
                        m_time_left_alive = U16_MAX;
                        break;
                    }
                }
            }
            if (!m_is_powered) {
                m_time_left_alive = 0;
            }
            
            if (m_time_left_alive > 0) {
                m_time_left_dead = U16_MAX;
                m_current_anim_id = m_idle_anim_id;
                
                if (m_time_left_alive != U16_MAX) {
                    --m_time_left_alive;
                }
            } else {
                m_current_anim_id = m_dead_anim_id;
            }
            
            if (anim::Manager::get(m_current_anim_id)) {
                sprite()->source_rect.x = anim::Manager::get(m_current_anim_id)->source.x;
                sprite()->source_rect.y = anim::Manager::get(m_current_anim_id)->source.y;
            }

        }
    };
}