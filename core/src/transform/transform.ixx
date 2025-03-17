export module transform;

import console;
import types;

export struct Transform {
    size_t id             = -1;
    Vec2f  position       = { 0.0f, 0.0f },
           start_position = { 0.0f, 0.0f },
           velocity       = { 0.0f, 0.0f },
           velocity_limit = { 0.0f, 0.0f },
           acceleration   = { 0.0f, 0.0f },
           deceleration   = { 0.0f, 0.0f };

    Transform() : id(-1),
                  position(), start_position(), velocity(),
                  acceleration(0.0f, 0.0f),
                  deceleration(0.0f, 0.0f),
                  velocity_limit(0.0f, 0.0f) {
    }
    Transform(Vec2f p) : Transform() {
        start_position = p;
    }
    ~Transform() {
        Console::log("~Transform()\n");
        id = -1;
    }
    void update() {
        velocity = decelerate(velocity, deceleration);
        velocity = clamp(velocity, velocity_limit);
        position += velocity;
    }

private:
    Vec2f clamp(Vec2f v, Vec2f limit) {
        if (limit == Vec2f{0.0f, 0.0f}) return v;
        if (v.x < -limit.x) {
            v.x = -limit.x;
        } else if (v.x > limit.x) {
            v.x = limit.x;
        }
        if (v.y < -limit.y) {
            v.y = -limit.y;
        } else if (v.y > limit.y) {
            v.y = limit.y;
        }
        return v;
    }
    Vec2f decelerate(Vec2f v, Vec2f amount) {
        if (v.x != 0.0f && amount.x != 0.0f) {
            if (v.x > 0.0f && v.x < amount.x || v.x < 0.0f && v.x > -amount.x) {
                v.x = 0.0f;
            } else {
                if (v.x > 0.0f) {
                    v.x -= amount.x;
                } else if (v.x < 0.0f) {
                    v.x += amount.x;
                }
            }
        }
        if (v.y != 0.0f && amount.y != 0.0f) {
            if (v.y > 0.0f && v.y < amount.y || v.y < 0.0f && v.y > -amount.y) {
                v.y = 0.0f;
            } else {
                if (v.y > 0.0f) {
                    v.y -= amount.y;
                } else if (v.y < 0.0f) {
                    v.y += amount.y;
                }
            }
        }
        return v;
    }
};