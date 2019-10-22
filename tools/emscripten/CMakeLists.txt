add_executable(enjamb-js entrypoint.cpp)
set_property(TARGET enjamb-js PROPERTY CXX_STANDARD 17)
target_link_libraries(enjamb-js libenjamb)
set_target_properties(enjamb-js PROPERTIES OUTPUT_NAME "enjamb")
# We need extra memory to link ICU, and need to export cwrap so we can call C++ functions from JS
set_target_properties(enjamb-js PROPERTIES
                      LINK_FLAGS "-s TOTAL_MEMORY=33619968 -s EXTRA_EXPORTED_RUNTIME_METHODS='cwrap'")

# Build to docs for GitHub pages
set(EMSCRIPTEN_OUTPUT_DIR ${PROJECT_SOURCE_DIR}/docs)
set_target_properties(enjamb-js
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${EMSCRIPTEN_OUTPUT_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${EMSCRIPTEN_OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${EMSCRIPTEN_OUTPUT_DIR}
)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/index.html ${EMSCRIPTEN_OUTPUT_DIR}/index.html COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/tln.js ${EMSCRIPTEN_OUTPUT_DIR}/tln.js COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/tln.css ${EMSCRIPTEN_OUTPUT_DIR}/tln.css COPYONLY)