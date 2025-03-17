add_library(core)
target_compile_features(core PRIVATE cxx_std_23)
target_sources(core
               PUBLIC FILE_SET CXX_MODULES
               FILES                   
                   "src/bitmap_text.ixx"
                   "src/config.ixx"
                   "src/console.ixx"
                   "src/input/input.ixx"
                   "src/input/inputs.ixx"
                   "src/line/line.ixx"
                   "src/line/lines.ixx"                   
                   "src/sprite/sprite.ixx"
                   "src/sprite/sprites.ixx"
                   "src/state.ixx"
                   "src/textures.ixx"
                   "src/transform/transform.ixx"
                   "src/transform/transforms.ixx"
                   "src/types.ixx"
                   "src/window.ixx")
target_link_libraries(core PRIVATE SFML::Graphics SFML::Window)
