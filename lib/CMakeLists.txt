add_library(libenjamb enjamb.cpp)
set_target_properties(libenjamb PROPERTIES OUTPUT_NAME "enjamb")

target_link_libraries(libenjamb PUBLIC ICU::uc ICU::dt)
target_include_directories(libenjamb PUBLIC include)
set_property(TARGET libenjamb PROPERTY CXX_STANDARD 17)