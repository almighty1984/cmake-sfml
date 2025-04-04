module player;

void Player::update() {
    if (!Inputs::is_valid(m_input_id) || !Transforms::is_valid(m_transform_id) || !Sprites::at(m_sprite_id)) return;

    //if      (transform()->velocity.x < 0.0f)    transform()->velocity.x += 0.01f;
    //else if (transform()->velocity.x > 0.0f)    transform()->velocity.x -= 0.01f;        
        
    if (is_pressed(key_up)) {
        //release(key_up);
        //Console::log("input ", m_input_id, " pressed up\n");
        transform()->velocity.y -= transform()->acceleration.y;
    }
    if (is_pressed(key_down)) {
        //release(key_down);
        //Console::log("input ", input_id, " pressed down\n");
        transform()->velocity.y += transform()->acceleration.y;
    }    
    if (is_pressed(key_left)) {
        //release(key_left);    
        transform()->velocity.x -= transform()->acceleration.x;
    }
    if (is_pressed(key_right)) {
        //release(key_right);
        //Console::log("input ", input_id, " pressed right\n");
        transform()->velocity.x += transform()->acceleration.x;
    }
    //transform()->update();

    //Transforms::at(transform_id)->update();

    //Sprites::at(sprite_id)->position = Transforms::at(transform_id)->position;
    //Sprites::at(sprite_id)->update();        
}