import app;
import config;
import state;

int main() {
    Config::load("main.cfg");

    App app(320, 180, Config::scale(), "cmake-sfml");
    app.add_state(Config::state_type());
    //app.add_state(state::Type::edit);
    app.add_state(state::Type::overlay);
    
    app.run();
    return 0;
}