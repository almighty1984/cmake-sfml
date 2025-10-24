module;
#include <SFML/Graphics.hpp>

export module window;
import console;
import line;
import sprite;
import transform;
import types;
import shader;

export class Window {
    sf::RenderWindow m_sf_window;
    RectF m_view = { 0.0F, 0.0F, 320.0F, 180.0F };
    U8  m_scale = 1;
    U16 m_w = 0,
        m_h = 0;


    shader::Object test_shader;

public:
    Window() = delete;
    Window(cU16 w, cU16 h, cU8 scale, const char* title) {
        Console::log("Window::init\n");
        m_w = w, m_h = h;
        m_scale = scale;
        m_view = { 0.0F, 0.0F, (F32)w, (F32)h };
        U32 video_w = (U32)(w * scale);
        U32 video_h = (U32)(h * scale);
        m_sf_window = sf::RenderWindow(sf::VideoMode({ video_w, video_h }), title);
        m_sf_window.setView(sf::View((sf::FloatRect(sf::Vector2f(0.0F, 0.0F), sf::Vector2f((F32)video_w, (F32)video_h)))));

        test_shader.load("res/shaders/test.vert", "res/shaders/test.frag");
    }
    cRectF view()  const { return m_view;  }
    cU8    scale() const { return m_scale; }
    cU16   w()     const { return m_w;     }
    cU16   h()     const { return m_h;     }

    void view(cRectF view) {
        Console::log("Window::view: ", view.x, " ", view.y, " ", view.w, " ", view.h, "\n");
        m_view = view;
        m_sf_window.setView(sf::View((sf::FloatRect(sf::Vector2f(view.x * m_scale, view.y * m_scale), sf::Vector2f(view.w * m_scale, view.h * m_scale)))));
        //m_sf_window.setView(sf::View({ view.x * m_scale, view.y * m_scale }, { view.w * m_scale, view.h * m_scale }));        
    }
    void clear()   { m_sf_window.clear();   }
    void close()   { m_sf_window.close();   }
    void display() { m_sf_window.display(); }
    void draw(line::Object* line) {
        if (!line || line->is_hidden/* || line->length < 1.0F*/) return;
        cVec2F offset = transform::Manager::get(line->transform_id) ? transform::Manager::get(line->transform_id)->position + line->offset : Vec2F{};
        if (offset.x + line->start.x < 0.0F        && offset.x + line->end.x < 0.0F       ||
            offset.x + line->start.x > m_w * 1.0F  && offset.x + line->end.x > m_w * 1.0F ||
            offset.y + line->start.y < 0.0F        && offset.y + line->end.y < 0.0F       ||
            offset.y + line->start.y > m_h * 1.0F  && offset.y + line->end.y > m_h * 1.0F) {
            return;
        }
        //sf::RenderStates states;
        //states.shader = test_shader.get_sfml_shader();
        //m_sf_window.draw(line->sf_vertices, 4, sf::PrimitiveType::TriangleFan, states);

        m_sf_window.draw(line->sf_vertices, 4, sf::PrimitiveType::TriangleFan);
    }
    void draw(sprite::Object* sprite) {
        if (!sprite || sprite->is_hidden || !transform::Manager::get(sprite->transform_id)) return;
        if (transform::Manager::get(sprite->transform_id)->position.x + sprite->offset.x < m_view.x - sprite->source_rect.w ||
            transform::Manager::get(sprite->transform_id)->position.x + sprite->offset.x > m_view.w ||
            transform::Manager::get(sprite->transform_id)->position.y + sprite->offset.y < m_view.y - sprite->source_rect.h ||
            transform::Manager::get(sprite->transform_id)->position.y + sprite->offset.y > m_view.h) {
            return;
        }
        sf::RenderStates states;
        states.shader = test_shader.get_sfml_shader();
        m_sf_window.draw(sprite->sf_sprite, states);

        //m_sf_window.draw(sprite->sf_sprite);
    }
    bool is_open() const { return m_sf_window.isOpen(); }
    std::optional<sf::Event> poll_event() { return m_sf_window.pollEvent(); }
};