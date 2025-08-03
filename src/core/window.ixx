module;
#include <SFML/Graphics.hpp>

export module window;

import console;
import line;
import sprite;
import transform;
import types;

export class Window {
    sf::RenderWindow m_sf_window;
    Rectf m_view = { 0.0f, 0.0f, 320.0f, 180.0f };
    u8  m_scale = 1;
    u16 m_w = 0,
        m_h = 0;

public:
    Rectfc view()  const { return m_view;  }
    u8c    scale() const { return m_scale; }
    u16c   w()     const { return m_w;     }
    u16c   h()     const { return m_h;     }

    void init(u16c w, u16c h, u8c scale, const char* title) {
        Console::log("Window::init\n");
        m_w = w, m_h = h;
        m_scale = scale;
        m_view = { 0.0f, 0.0f, (f32)w, (f32)h };
        u32 video_w = (u32)(w * scale);
        u32 video_h = (u32)(h * scale);
        m_sf_window = sf::RenderWindow(sf::VideoMode({ video_w, video_h }), title);
        m_sf_window.setView(sf::View((sf::FloatRect(sf::Vector2f(0.0f, 0.0f), sf::Vector2f((f32)video_w, (f32)video_h)))));
    }
    void view(Rectfc view) {
        m_view = view;
        m_sf_window.setView(sf::View((sf::FloatRect(sf::Vector2f(view.x * m_scale, view.y * m_scale), sf::Vector2f(view.w * m_scale, view.h * m_scale)))));
        //m_sf_window.setView(sf::View({ view.x * m_scale, view.y * m_scale }, { view.w * m_scale, view.h * m_scale }));        
    }
    void clear()   { m_sf_window.clear();   }
    void close()   { m_sf_window.close();   }
    void display() { m_sf_window.display(); }
    void draw(line::Object* line) {
        if (!line || line->is_hidden) return;
        /*if (line->is_debug) {
            if (line->start.x < 0.0f && line->end.x < 0.0f ||
                line->start.x > (f32)m_w && line->end.x >(f32)m_w ||
                line->start.y < 0.0f && line->end.y < 0.0f ||
                line->start.y >(f32)m_h && line->end.y >(f32)m_h) {
                return;
            }
        }*/
        m_sf_window.draw(line->sf_vertices, 4, sf::PrimitiveType::TriangleFan);
    }
    void draw(sprite::Object* sprite) {
        if (!sprite || !transform::Set::at(sprite->transform_id)) return;
        if (transform::Set::at(sprite->transform_id)->position.x + sprite->offset.x < m_view.x - sprite->source_rect.w ||
            transform::Set::at(sprite->transform_id)->position.x + sprite->offset.x > m_view.w ||
            transform::Set::at(sprite->transform_id)->position.y + sprite->offset.y < m_view.y - sprite->source_rect.h ||
            transform::Set::at(sprite->transform_id)->position.y + sprite->offset.y > m_view.h) {
            return;
        }

        m_sf_window.draw(sprite->sf_sprite);
    }
    bool is_open() const { return m_sf_window.isOpen(); }
    std::optional<sf::Event> poll_event() { return m_sf_window.pollEvent(); }
};