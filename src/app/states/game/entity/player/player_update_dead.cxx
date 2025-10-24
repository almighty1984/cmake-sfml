module player;
import console;
import health;

void player::Object::update_dead() {
    if (m_is_first_state_update) {
        m_is_first_state_update = false;
        sprite()->is_hidden = true;        
        m_state = m_next_state = entity::State::dead;
        //reset_anim(m_dead_anim_id);
        spawn_fan(0.0F, 360.0F, 8, particle::Type::drop_blood, transform()->position + Vec2F{ 0.0F, -8.0F }, transform()->velocity, 3.0F);
        spawn(particle::Type::health, transform()->position + Vec2F{ 4.0F, 4.0F }, {});
        m_time_left_dead = 100;

        Console::log("player::Object::update_dead\n");


        if (sound::Manager::get(m_hover_sound_id)->is_playing()) {
            sound::Manager::get(m_hover_sound_id)->stop();
        }
    }

    transform()->velocity = {};
    transform()->moved_velocity = {};    
    
    --m_time_left_dead;
    if (m_time_left_dead == 0) {
        m_next_state = entity::State::run;
        health::Manager::get(m_health_id)->reset();
    }
}