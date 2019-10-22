add_executable (enjamb driver.cpp)
target_link_libraries(enjamb libenjamb)
set_property(TARGET enjamb PROPERTY CXX_STANDARD 17)