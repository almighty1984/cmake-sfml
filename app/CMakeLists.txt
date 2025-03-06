add_library(app)
target_compile_features(app PRIVATE cxx_std_23)
target_sources(app
               PUBLIC FILE_SET CXX_MODULES
               FILES
                   "src/app.ixx"
                   "src/player.ixx"
                   "src/states/edit.ixx"              
                   "src/states/game_init.ixx" )
target_compile_features(app PRIVATE cxx_std_23)
target_link_libraries(app PRIVATE core)