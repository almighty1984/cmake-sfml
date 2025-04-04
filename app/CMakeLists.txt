add_library(app)
target_compile_features(app PRIVATE cxx_std_23)

file(GLOB_RECURSE APP_CXX_FILES "src/*.cxx")
file(GLOB_RECURSE APP_IXX_FILES "src/*.ixx")

target_sources(app PRIVATE ${APP_CXX_FILES} PUBLIC FILE_SET CXX_MODULES FILES ${APP_IXX_FILES})
target_link_libraries(app PRIVATE core)