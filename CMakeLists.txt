cmake_minimum_required(VERSION 3.8)
project(MRBasic)

set(CMAKE_CXX_STANDARD 11)

find_package(OpenCV REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -framework OpenGL -lGLEW -lglfw")

include_directories(Dependencies
        src/
        Utility/
        Dependencies/glm/
        /opt/local/include/SOIL/
        )

set(SOURCE_FILES main.cpp src/Renderer/Renderer.cpp src/Renderer/Renderer.h src/App/App.cpp src/App/App.h Utility/Shader/Shader.cpp Utility/Shader/Shader.h Utility/GLInclude.h Utility/Shader/ShaderProgram.cpp Utility/Shader/ShaderProgram.h Dependencies/GLGemeotryBase.h Dependencies/OBJLoader.h Dependencies/MaterialLoader.h src/ScreenShot/ScreenShot.cpp src/ScreenShot/ScreenShot.h Dependencies/Loader/tiny_obj_loader.h Dependencies/Loader/Loader.cpp Dependencies/Loader/Loader.h)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(MRBasic ${SOURCE_FILES})

target_link_libraries(MRBasic ${OpenCV_LIBS} /opt/local/lib/libSOIL.dylib)