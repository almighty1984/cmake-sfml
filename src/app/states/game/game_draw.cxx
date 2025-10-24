module state.game;

namespace state {
    void Game::draw(std::unique_ptr<Window>& window, cU8 layer) {
        for (auto& i : m_level_sprite_ids) {
            if (sprite::Manager::get(i) && sprite::Manager::get(i)->layer == layer) {
                window->draw(sprite::Manager::get(i));
            }
        }
        if (m_player.sprite()->layer == layer) {
            m_player.draw(window);
            if (is_drawing_debug) {
                m_player.draw_aabb(window);
            }
        }
        for (auto& i : m_entity_objects) {
            if (i && i->sprite() && i->sprite()->layer == layer) {
                i->draw(window);
                if (is_drawing_debug) {
                    i->draw_aabb(window);
                }
            }
        }
        for (auto& i : m_water_entity_objects) {
            if (i && i->start_layer() == layer) {
                i->draw(window);
                if (is_drawing_debug) {
                    i->draw_aabb(window);
                }
            }
        }
        particle::Manager::draw(window, layer);
        if (is_drawing_debug) {
            particle::Manager::draw_aabb(window, layer);
        }
        if (is_drawing_debug && layer == 0) {
            for (auto& i : m_level_quad_nodes) {
                i.second->draw(window);
            }
        }
        health::Manager::draw(window, layer);
    }
}