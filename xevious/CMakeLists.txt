# Force CMake version 3.1 or above
cmake_minimum_required (VERSION 3.1)

macro(configure_files srcDir destDir)
    message(STATUS "Configuring directory ${destDir}")
    make_directory(${destDir})

    file(GLOB templateFiles  RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/${srcDir} ${srcDir}/*)
    foreach(templateFile ${templateFiles})
        message(STATUS "Configuring path ${templateFile}")
        set(srcTemplatePath ${srcDir}/${templateFile})

        if(NOT IS_DIRECTORY ${srcTemplatePath})
            message(STATUS "Configuring file ${templateFile}")
            configure_file(
                    ${srcTemplatePath}
                    ${destDir}/${templateFile}
                    COPYONLY
                    )
        endif(NOT IS_DIRECTORY ${srcTemplatePath})
    endforeach(templateFile)
endmacro(configure_files)

project (xavious)

file(GLOB all_files
        "*.h"
        "*.cpp"
        )

add_executable(xavious ${all_files})


# Define C++11 standard
set_property(TARGET xavious PROPERTY CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra")

target_include_directories(xavious PUBLIC libraries/glm  libraries/tinyobjloader/ libraries/  )

target_link_libraries (xavious PUBLIC GL GLEW glfw GLU)
configure_files(shaders shaders)
configure_files(resources resources)
#file(COPY resources DESTINATION ./)
#file(COPY shaders DESTINATION ./)
