export module entity.logic_and;
import anim;
import console;
import entity;

export namespace entity {
    class LogicAnd : public Object {
    public:
        LogicAnd() {
            m_input_limit = 2;
        }
        void update() override {
            if (!sprite()) return;


            m_time_left_alive = U16_MAX;
            if (input_objects.size() == 2) {
                for (auto& i : input_objects) {
                    if (i->time_left_alive() == 0) {
                        m_time_left_alive = 0;
                        break;
                    }
                }
            } else {
                m_time_left_alive = 0;
            }
            
            if (m_time_left_alive > 0) {
                m_time_left_dead = 0;
                m_current_anim_id = m_idle_anim_id;

                if (m_time_left_alive != U16_MAX) {
                    --m_time_left_alive;
                }
            } else {                
                m_current_anim_id = m_dead_anim_id;
            }
            if (m_time_left_dead > 0) {
                m_time_left_alive = 0;
                if (m_time_left_dead != U16_MAX) {
                    --m_time_left_dead;
                }
            }
            if (anim::Manager::get(m_current_anim_id)) {
                sprite()->source_rect.x = anim::Manager::get(m_current_anim_id)->source.x;
                sprite()->source_rect.y = anim::Manager::get(m_current_anim_id)->source.y;
            }
        }
    };
}