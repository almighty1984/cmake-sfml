module;
#include <vector>
#include <string>
#include <list>
#include <memory>
#include <sstream>

export module particle;
import console;
import types;
import entity;
import entity.particle;
import entity.particle.brick;
import entity.particle.bubble;
import entity.particle.down_thrust;
import entity.particle.health;
import entity.particle.interact;
import entity.particle.melee;
import entity.particle.sense;
import entity.particle.drop;
import window;

export namespace particle {
    struct Spawn {
        entity::Object* parent = nullptr;
        Type type = Type::none;
        Vec2F       position = { 0.0F, 0.0F },
                    velocity = { 0.0F, 0.0F };
    };
    using cSpawn = const Spawn;

    class Manager {
        static inline std::vector<Spawn> s_to_spawn;
        static inline std::list<entity::Particle*> s_entity_objects;

    public:
        static void draw(std::unique_ptr<Window>& window, cU8 layer) {
            for (auto& i : s_entity_objects) {
                if (i && i->start_layer() == layer) {
                    i->draw(window);
                }
            }
        }
        static void draw_aabb(std::unique_ptr<Window>& window, cU8 layer) {
            for (auto& i : s_entity_objects) {
                if (i && i->start_layer() == layer) {
                    i->draw_aabb(window);
                }
            }
        }
        static void clear() {
            s_to_spawn.clear();
            /*for (auto it = s_entity_objects.begin(); it != s_entity_objects.end(); ++it) {
                if (!(*it)) continue;
                it = s_entity_objects.erase(it);
                if (it == s_entity_objects.end()) {
                    break;
                }
            }*/
            for (auto it = s_entity_objects.begin(); it != s_entity_objects.end(); ++it) {
                if (!(*it)) continue;
                delete (*it);
            }
            s_entity_objects.clear();
        }
        static void spawn(cSpawn particle) {
            s_to_spawn.emplace_back(particle);
        }
        static void update() {
            for (auto it = s_entity_objects.begin(); it != s_entity_objects.end(); ++it) {
                if (!(*it)) continue;
                (*it)->update();                
            }
        }
        static void check_to_erase() {            
            for (auto it = s_entity_objects.begin(); it != s_entity_objects.end(); ++it) {
                if (!(*it)) continue;
                if ((*it)->is_to_erase()) {
                    delete (*it);
                    it = s_entity_objects.erase(it);
                    if (it == s_entity_objects.end()) {
                        break;
                    }
                }
            }
        }
        static void check_to_spawn() {
            //Console::log("particles: ", s_entity_objects.size(), "\n");

            for (auto& particle : s_to_spawn) {
                Vec2F position = particle.position;
                Vec2F velocity = particle.velocity;

                if (particle.type == Type::bubble) {
                    s_entity_objects.emplace_back(new entity::ParticleBubble);
                    s_entity_objects.back()->load_config("res/entity/particle/bubble.cfg");
                    //m_particle_entity_objects.back()->sprite()->is_leftward = random::Generator::number(0,1);
                    //m_entity_objects.back()->direction({ (F32)random::Generator::number(0, 1), 0.0F });
                } else if (particle.type == Type::health) {
                    s_entity_objects.emplace_back(new entity::ParticleHealth);
                    s_entity_objects.back()->load_config("res/entity/particle/health.cfg");
                } else if (particle.type == Type::hit) {
                    s_entity_objects.emplace_back(new entity::Particle);
                    s_entity_objects.back()->load_config("res/entity/particle/hit.cfg");
                    //m_entity_objects.back()->sprite()->set_rotation(random::Generator::real_number(0.0F, 359.0F));                    
                    //Console::log("rotation: ", m_particle_entity_objects.back()->sprite()->rotation, "\n");
                    velocity = {};
                } else if (particle.type == Type::interact) {
                    s_entity_objects.emplace_back(new entity::ParticleInteract);
                    s_entity_objects.back()->load_config("res/entity/particle/interact_" + entity::to_string(particle.parent->type()) + ".cfg");
                } else if (particle.type == Type::melee) {
                    s_entity_objects.emplace_back(new entity::ParticleMelee);
                    s_entity_objects.back()->load_config("res/entity/particle/melee_" + entity::to_string(particle.parent->type()) + ".cfg");
                    Console::log("time left alive: ", s_entity_objects.back()->time_left_alive(), "\n");
                } else if (particle.type == Type::sense) {
                    s_entity_objects.emplace_back(new entity::ParticleSense);
                    s_entity_objects.back()->load_config("res/entity/particle/sense_" + entity::to_string(particle.parent->type()) + ".cfg");
                    s_entity_objects.back()->start_layer(NUM_VISIBLE_LAYERS - 1);
                } else if (particle.type == Type::brick       ||
                           particle.type == Type::down_thrust ||
                           particle.type == Type::drop        ||
                           particle.type == Type::drop_blood  ||
                           particle.type == Type::dust) {
                    if (particle.type == Type::brick) {
                        s_entity_objects.emplace_back(new entity::ParticleBrick);
                    } else if (particle.type == Type::down_thrust) {
                        s_entity_objects.emplace_back(new entity::ParticleDownThrust);
                    } else if (particle.type == Type::drop_blood || particle.type == Type::drop) {
                        s_entity_objects.emplace_back(new entity::ParticleDrop);
                    } else {
                        s_entity_objects.emplace_back(new entity::Particle);
                    }
                    s_entity_objects.back()->load_config("res/entity/particle/" + particle::to_string(particle.type) + ".cfg");
                }

                if (s_entity_objects.back()->transform()) {
                    s_entity_objects.back()->transform()->position = position;
                    s_entity_objects.back()->transform()->velocity = velocity;
                }
                //if (s_entity_objects.back()->sprite() && particle.parent->sprite()) {
                    //s_entity_objects.back()->sprite()->layer = particle.parent->sprite()->layer;
                //}
                if (particle.parent) {
                    s_entity_objects.back()->parent(particle.parent);
                    if (s_entity_objects.back()->sprite()) {
                        //s_entity_objects.back()->sprite()->layer = particle.parent->start_layer();
                        s_entity_objects.back()->sprite()->layer = NUM_VISIBLE_LAYERS - 1;
                        s_entity_objects.back()->start_layer(s_entity_objects.back()->sprite()->layer);
                    }
                }
                //s_entity_objects.back()->update();
            }
            s_to_spawn.clear();
        }
    };
    
}
