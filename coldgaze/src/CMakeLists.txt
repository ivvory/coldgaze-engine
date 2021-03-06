SET(PROJECT_NAME Coldgaze)
SET(PROJECT_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/framework)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/framework)

message(STATUS "Generating project file for ${PROJECT_FOLDER}")
# Main
file(GLOB_RECURSE HEADERS *.hpp *.h)
file(GLOB_RECURSE SOURCE ${HEADERS} "*.cpp")
file(GLOB IMGUI_SOURCE "../include/imgui/*.cpp")
file(GLOB NATIVEFILEDIALOG_SOURCE 
    "../include/nativefiledialog/src/nfd_common.c"
    "../include/nativefiledialog/src/nfd_win.cpp"
)

set(MAIN_CPP ${PROJECT_FOLDER}/main.cpp)

# Add shaders
set(SHADER_DIR "../data/shaders/${PROJECT_NAME}")
file(GLOB SHADERS "${SHADER_DIR}/*.vert" "${SHADER_DIR}/*.frag" "${SHADER_DIR}/*.comp" "${SHADER_DIR}/*.geom" "${SHADER_DIR}/*.tesc" "${SHADER_DIR}/*.tese" "${SHADER_DIR}/*.mesh" "${SHADER_DIR}/*.task" "${SHADER_DIR}/*.rgen" "${SHADER_DIR}/*.rchit" "${SHADER_DIR}/*.rmiss")
source_group("Shaders" FILES ${SHADERS})

if(MSVC)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
endif()

if(WIN32)
    set(COLDGAZE_LIBRARIES "")

    add_executable(${PROJECT_NAME} WIN32 ${MAIN_CPP} ${SOURCE} ${SHADERS} ${IMGUI_SOURCE} ${NATIVEFILEDIALOG_SOURCE})
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE})
    target_link_libraries(${PROJECT_NAME} ${Vulkan_LIBRARY} ${ASSIMP_LIBRARIES} ${WINLIBS} ${GLFW_LIBRARIES} ${COLDGAZE_LIBRARIES} ${SDL2_LIBRARY})
else(WIN32)
    add_executable(${PROJECT_NAME} ${MAIN_CPP} ${SOURCE} ${SHADERS} ${SDL2_LIBRARY} ${IMGUI_SOURCE} ${NATIVEFILEDIALOG_SOURCE})
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE})
    target_link_libraries(${PROJECT_NAME})
endif(WIN32)

set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# ugly hack, usually we have to build library and application separate, so this code should be inside engine CMakeLists.txt

# BEGIN ENGINE CODE

if (WIN32)
    file(GLOB_RECURSE DLLS "${COLDGAZE_LIBS_PATH}/*.dll")
    add_custom_command(TARGET ${NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            ${DLLS}
            $<TARGET_FILE_DIR:${NAME}>)
endif()

add_custom_command(TARGET ${NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${SHADERS_SPIRV_DIR})

file(GLOB_RECURSE SHADER_SRC RELATIVE ${SHADERS_GLSL_DIR} "${SHADERS_GLSL_DIR}/*.vert" "${SHADERS_GLSL_DIR}/*.frag")
foreach(SHADER ${SHADER_SRC})
    add_custom_command(TARGET ${NAME} POST_BUILD
        COMMAND ${GLSLC_TOOL} ${SHADERS_GLSL_DIR}/${SHADER} -o ${SHADERS_SPIRV_DIR}/${SHADER}.spv)
endforeach()

file(GLOB_RECURSE SHADER_RTX_SRC RELATIVE ${SHADERS_GLSL_DIR} "${SHADERS_GLSL_DIR}/*.rchit" "${SHADERS_GLSL_DIR}/*.rmiss" "${SHADERS_GLSL_DIR}/*.rgen")
foreach(SHADER ${SHADER_RTX_SRC})
    add_custom_command(TARGET ${NAME} POST_BUILD
        COMMAND ${GLSLVALIDATOR_TOOL} -V -o ${SHADERS_SPIRV_DIR}/${SHADER}.spv ${SHADERS_GLSL_DIR}/${SHADER})
endforeach()

# END ENGINE CODE

if(RESOURCE_INSTALL_DIR)
    install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()