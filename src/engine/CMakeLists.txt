
add_library(engine INTERFACE)
set_target_properties(engine PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(engine INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
