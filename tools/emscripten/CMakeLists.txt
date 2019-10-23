add_executable(enjamb-js entrypoint.cpp)
set_property(TARGET enjamb-js PROPERTY CXX_STANDARD 17)
target_link_libraries(enjamb-js libenjamb)
set_target_properties(enjamb-js PROPERTIES OUTPUT_NAME "enjamb")
# We need extra memory to link ICU, and need to export cwrap so we can call C++ functions from JS
set_target_properties(enjamb-js PROPERTIES
                      LINK_FLAGS "-s TOTAL_MEMORY=33619968 -s EXTRA_EXPORTED_RUNTIME_METHODS='cwrap' -s DISABLE_EXCEPTION_CATCHING=0")
                      
install(TARGETS enjamb-js RUNTIME DESTINATION usr/share/enjamb)
install(FILES index.html tln.css tln.js DESTINATION usr/share/enjamb)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/enjamb.wasm DESTINATION usr/share/enjamb)