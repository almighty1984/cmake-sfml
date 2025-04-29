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
    u16 m_w = 0,
        m_h = 0;
    u8  m_scale = 1;
public:    
    u16c w()     const { return m_w;     }
    u16c h()     const { return m_h;     }
    u8c  scale() const { return m_scale; }

    void init(u16c w, u16c h, u8c scale, const char* title) {
        Console::log("Window::init\n");
        m_w = w, m_h = h;
        m_scale = scale;
        u32 video_w = (u32)(w * scale);
        u32 video_h = (u32)(h * scale);
        m_sf_window = sf::RenderWindow(sf::VideoMode({ video_w, video_h }), title);
        m_sf_window.setView(sf::View((sf::FloatRect(sf::Vector2f(0.0f, 0.0f), sf::Vector2f((f32)video_w, (f32)video_h)))));
    }
    void view(Rect<f32> rect) {
        m_sf_window.setView(sf::View(sf::FloatRect({ (f32)rect.x, (f32)rect.y }, { (f32)rect.w, (f32)rect.h })));
    }
    void clear()   { m_sf_window.clear();   }
    void close()   { m_sf_window.close();   }
    void display() { m_sf_window.display(); }
    void draw(line::Object* line) {
        if (!line) return;
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
        if (transform::Set::at(sprite->transform_id)->position.x + sprite->offset.x < -sprite->source_rect.w ||
            transform::Set::at(sprite->transform_id)->position.x + sprite->offset.x > m_w ||
            transform::Set::at(sprite->transform_id)->position.y + sprite->offset.y < -sprite->source_rect.h ||
            transform::Set::at(sprite->transform_id)->position.y + sprite->offset.y > m_h) {
            return;
        }
        m_sf_window.draw(sprite->sf_sprite);
    }
    bool is_open() const { return m_sf_window.isOpen(); }
    std::optional<sf::Event> poll_event() { return m_sf_window.pollEvent(); }
};