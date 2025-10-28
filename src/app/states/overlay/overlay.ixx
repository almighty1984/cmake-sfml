module;
#include <string>
#include <sstream>
export module state.overlay;
import state;
import console;
import bitmap_text;
import input;
import line;
import transform;
import sprite;

export namespace state {
    class Overlay : public Object {
        I32 //m_line_transform_id = -1,
            m_transform_id = -1;

        //I32 m_line_id = -1;

        BitmapText m_fps_text;

        Vec2F m_fps_offset = { 0.0F, 0.0F };

        //U8 m_timer = 0;
    public:
        Overlay(cU16 window_w, cU16 window_h) {
            m_current_state = m_next_state = state::Type::overlay;

            m_input_id = input::Manager::make();

            m_transform_id = transform::Manager::make();
            //transform::Manager::get(m_transform_id)->velocity.x = 2.0f;
            //transform::Manager::get(m_transform_id)->velocity.y = 2.0f;

            //transform::Manager::get(m_transform_id)->deceleration.x = 0.0f;
            //Console::log("Overlay() transform id: ", m_transform_id, "\n");

            //m_line_transform_id = transform::Manager::make();
            //m_line_id = line::Manager::make({}, { 320.0F,180.0F });
            //line::Manager::get(m_line_id)->transform_id = m_line_transform_id;
            //line::Manager::get(m_line_id)->size = 4;

            m_fps_text.transform_id = m_transform_id;
            m_fps_text.layer = NUM_VISIBLE_LAYERS - 1;

            m_fps_text.texture_path = "res/texture/font_5_white.png";
            m_fps_text.font_size = 5;

            m_visible_layers.insert(m_fps_text.layer);
        }
        ~Overlay() {
            Console::log("~Overlay()\n");
            input::Manager::erase(m_input_id);
            transform::Manager::erase(m_transform_id);
            //transform::Manager::erase(m_line_transform_id);
            //line::Manager::erase(m_line_id);
            m_fps_text.clear_text();
        }
        virtual void update(cF32 ts) override {
            if (!input() || !transform::Manager::get(m_transform_id) /*|| !transform::Manager::get(m_line_transform_id)*/) return;

            /*++m_timer;
            if (m_timer < 2) {
                return;
            }
            m_timer = 0;*/
            //Console::log("overlay update ", m_transform_id, " ", transform::Manager::get(m_transform_id)->velocity.x, " ", transform::Manager::get(m_transform_id)->velocity.y, "\n");

            cF32 current_fps = 1.0F / ts;

            //Console::log("Overlay::update ", current_fps, "\n");

            m_fps_text.transform_id = m_transform_id;
            m_fps_text.set_text(std::to_string((int)current_fps));

            if (input()->is_pressed(input::Key::ctrl)) {
                if (input()->is_pressed(input::Key::up)) {
                    m_fps_offset.y -= 1.0f;                    
                }
                if (input()->is_pressed(input::Key::down)) {
                    m_fps_offset.y += 1.0f;
                }
                if (input()->is_pressed(input::Key::left)) {
                    m_fps_offset.x -= 1.0f;
                }
                if (input()->is_pressed(input::Key::right)) {
                    m_fps_offset.x += 1.0f;
                }
            }

            //if (input()->is_pressed(input::Button::left)) {
            //    input()->release(input::Button::left);
            //    line::Manager::get(m_line_id)->start = input::Manager::mouse;
            //}

            //line::Manager::get(m_line_id)->end = input::Manager::mouse;
            //transform::Manager::get(m_line_transform_id)->position = m_fps_offset;

            transform::Manager::get(m_transform_id)->position.x = m_fps_offset.x + (F32)(view().w - m_fps_text.get_text().size() * m_fps_text.font_size);
            transform::Manager::get(m_transform_id)->position.y = m_fps_offset.y + (F32)(view().h - m_fps_text.font_size);
        }
        void draw(std::unique_ptr<Window>& window, cU8 layer) override {
            //Console::log("fps text layer: ", (int)m_fps_text.layer, " ", (int)layer, "\n");
            if (m_fps_text.layer == layer) {
                m_fps_text.draw(window);
            }
            //window->draw(line::Manager::get(m_line_id));
        }
    };
}