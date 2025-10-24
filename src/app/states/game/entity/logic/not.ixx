export module entity.logic_not;
import anim;
import console;
import entity;

export namespace entity {
    class LogicNot : public Object {
    public:
        LogicNot() {
            m_input_limit = 1;
        }
        void update() override {
            if (!sprite()) return;

            //Console::log("LogicNot is_powered: ", m_is_powered, "\n");
            for (auto& i : input_objects) {
                m_time_left_alive = i->time_left_alive() > 0 ? 0 : U16_MAX;
                if (i->is_powered()) {
                    m_is_powered = true;
                }
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