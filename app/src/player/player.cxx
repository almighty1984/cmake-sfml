module;
#include <sstream>

module player;

import console;
import input;
import inputs;
import sprites;
import transform;
import transforms;
import types;

Player::Player() {
    Console::log("Player()\n");
    m_input_id = Inputs::make();

    m_transform_id = Transforms::make();
    Transforms::at(m_transform_id)->acceleration = { 0.3f, 0.3f };
    Transforms::at(m_transform_id)->deceleration = { 0.1f, 0.1f };
    Transforms::at(m_transform_id)->velocity_limit = { 4.0f, 4.0f };

    m_sprite_id = Sprites::make("res/textures/tile_selection.png");
    Sprites::at(m_sprite_id)->transform_id = m_transform_id;
}
Player::~Player() {
    Console::log("~Player()\n");
    Console::log("~Player() input\n");
    Inputs::erase(m_input_id);
    Console::log("~Player() transform\n");
    Transforms::erase(m_transform_id);
    Console::log("~Player() sprite\n");
    Sprites::erase(m_sprite_id);
    m_input_id = m_transform_id = m_sprite_id = -1;
}