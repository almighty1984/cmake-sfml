module;
#include <SFML/Graphics.hpp>

export module window;

import config;
import console;
import line;
import sprite;
import types;

export class Window {
    sf::RenderWindow m_sf_window;
    u16 m_w, m_h;
public:
    Window() : m_w(320), m_h(240) {}
    u16c w() const { return m_w; }
    u16c h() const { return m_h; }
    void init(u16c w, u16c h, const char* title) {
        Console::log("Window::init\n");
        m_w = w, m_h = h;
        u32 video_w = (u32)(w * Config::scale());
        u32 video_h = (u32)(h * Config::scale());
        m_sf_window = sf::RenderWindow(sf::VideoMode({ video_w, video_h }), title);
        m_sf_window.setView(sf::View((sf::FloatRect(sf::Vector2f(0.0f, 0.0f), sf::Vector2f((f32)video_w, (f32)video_h)))));
    }    
    void clear()   { m_sf_window.clear();   }
    void close()   { m_sf_window.close();   }
    void display() { m_sf_window.display(); }
    void draw(Line* line) {
        if (!line) return;
        m_sf_window.draw(line->sf_vertices, 4, sf::PrimitiveType::TriangleFan);
    }
    void draw(Sprite* sprite) {
        if (!sprite) return;
        m_sf_window.draw(sprite->sf_sprite);
    }
    bool is_open() const { return m_sf_window.isOpen(); }
    std::optional<sf::Event> poll_event() { return m_sf_window.pollEvent(); }
};