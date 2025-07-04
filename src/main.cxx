import app;
import config;
import state;

int main() {
    Config::load("main.cfg");

    App app(320, 180, Config::scale(), "cmake-sfml");
    app.set_state(Config::state_type());
    app.run();
    return 0;
}