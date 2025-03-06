export module state.game_init;

import state;
import bitmap_text;
import console;
import inputs;
import lines;
import sprites;
import transforms;
import types;

import player;

export namespace state {
    class GameInit : public State {
        BitmapText m_test_text;        
        size_t m_transform_id = -1,
               m_input_id     = -1,
               m_line_0       = -1;

        u32 m_timer = 0;

        Player m_player;

    public:
        GameInit() {
            next = current = Type::GameInit;

            m_transform_id = Transforms::add(new Transform);
            Transforms::at(m_transform_id)->velocity.x = 5.0f;
            Transforms::at(m_transform_id)->deceleration.x = 0.1f;
            Console::log("GameInit transform_id: ", m_transform_id, "\n");

            m_test_text.transform_id = m_transform_id;
            m_test_text.set_text("GameInit");


            m_input_id = Inputs::add(new Input);

            m_line_0 = Lines::add(new Line({ 32, 32 }, { 64, 128 }));
            Lines::at(m_line_0)->size = 8;
            Lines::at(m_line_0)->color = { 255, 127, 0 };


            //Console::log("Sprites size: ", Sprites::size(), "\n");
        }
        ~GameInit() {
            Console::log("~GameInit()\n");
            Transforms::erase(m_transform_id);

            Lines::erase(m_line_0);

            //Inputs::clear();
            //Lines::clear();
            //Transforms::clear();
            Sprites::clear();
            

            /*Sprites::erase(test_sprite_id);

            Inputs::erase(input_id);*/
        }
        void update() override {
            if (
                //!Sprites::at(test_sprite_id) ||
                m_line_0 == -1 || !Lines::at(m_line_0)
                //|| !Inputs::at(input_id)
                ) {
                return;
            }

            Lines::at(m_line_0)->start = Inputs::mouse_prev;
            Lines::at(m_line_0)->end = Inputs::mouse;

            if (Inputs::at(m_input_id)->is_pressed(input::Key::up)) {
                Inputs::at(m_input_id)->release(input::Key::up);
                Console::log("GameInit::update pressed up\n");

            }
            if (Inputs::at(m_input_id)->is_pressed(input::Key::esc)) {
                Inputs::at(m_input_id)->release(input::Key::esc);
                set_next(Type::Edit);
            }

            m_test_text.update();

            m_player.update();

            //Console::log("GameInit::update ", timer, "\n");
            /*if (timer != -1 && timer > 50) {
                timer = -1;
                set_next(Type::Edit);
            } else {
                timer++;
            }*/



            //Lines::draw(0, m_window);
            //Sprites::at(test_sprite_id)->update();
        }
    };
}