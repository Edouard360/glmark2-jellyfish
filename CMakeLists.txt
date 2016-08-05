# CMake entry point
cmake_minimum_required (VERSION 2.6)
project (Glmark2-Jellyfish)

#find_package(PNG REQUIRED)
find_package(OpenGL REQUIRED)

if( CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR )
    message( FATAL_ERROR "Please select another Build Directory ! (and give it a clever name, like bin_Visual2012_64bits/)" )
endif()
if( CMAKE_SOURCE_DIR MATCHES " " )
	message( "Your Source Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()
if( CMAKE_BINARY_DIR MATCHES " " )
	message( "Your Build Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()

# Compile external dependencies
add_subdirectory (external)

# On Visual 2005 and above, this module can set the debug working directory
cmake_policy(SET CMP0026 OLD)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/external/rpavlik-cmake-modules-fe2273")
include(CreateLaunchers)
include(MSVCMultipleProcessCompile) # /MP

if(INCLUDE_DISTRIB)
	add_subdirectory(distrib)
endif(INCLUDE_DISTRIB)

include_directories(
	${PNG_INCLUDE_DIR}
	external/glfw-3.1.2/include/GLFW/
	external/glm-0.9.7.1/
	external/glew-1.13.0/include/
	.
)


set(ALL_LIBS
	${OPENGL_LIBRARY}
	${PNG_LIBRARY}
	glfw
	GLEW_1130
)

add_definitions(
	-DTW_STATIC
	-DTW_NO_LIB_PRAGMA
	-DTW_NO_DIRECT3D
	-DGLEW_STATIC
	-D_CRT_SECURE_NO_WARNINGS
)

# Jellyfish
add_executable(jellyfish
	jellyfish/main.cpp

	jellyfish/data/conf.h
	jellyfish/data/jellyfish_attributes.h

	jellyfish/src/AbstractJellyfish.h
	jellyfish/src/AbstractJellyfish.cpp
	jellyfish/src/InstancedJellyfish.h
	jellyfish/src/InstancedJellyfish.cpp
	jellyfish/src/SingleJellyfish.h
	jellyfish/src/SingleJellyfish.cpp
	jellyfish/src/gradient.h
	jellyfish/src/gradient.cpp

	jellyfish/shaders/gradient.frag
	jellyfish/shaders/gradient.vert
	jellyfish/shaders/jellyfish.frag
	jellyfish/shaders/jellyfish.vert
	jellyfish/shaders/jellyfishInstanced.vert

	jellyfish/util/time.h
	jellyfish/util/json.hpp

	common/shader.cpp
	common/shader.hpp
	common/texture.cpp
	common/texture.hpp
)

target_link_libraries(jellyfish
	${ALL_LIBS}
)

###################
set_target_properties(jellyfish PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
    COMPILE_DEFINITIONS "$<$<CXX_COMPILER_ID:MSVC>:_SCL_SECURE_NO_WARNINGS>"
    COMPILE_OPTIONS "$<$<CXX_COMPILER_ID:MSVC>:/EHsc;$<$<CONFIG:Release>:/Od>>"
)

###################


# Xcode and Visual working directories
set_target_properties(jellyfish PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/jellyfish/")
create_target_launcher(jellyfish WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/jellyfish/")
create_default_target_launcher(jellyfish WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/jellyfish/") # tut 1 is not the default or people would complain that tut 2 doesn't work

SOURCE_GROUP(common REGULAR_EXPRESSION      ".*/common/.*")
SOURCE_GROUP(data REGULAR_EXPRESSION		".*/.*/data/.*")
SOURCE_GROUP(src REGULAR_EXPRESSION         ".*/src/.*")
SOURCE_GROUP(shaders REGULAR_EXPRESSION     ".*/shaders/.*")
SOURCE_GROUP(util REGULAR_EXPRESSION        ".*/util/.*")

## Another example of regular expression we could use
## SOURCE_GROUP(shader REGULAR_EXPRESSION ".*/.*shader$" )

if (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )
add_custom_command(
   TARGET jellyfish POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/jellyfish${CMAKE_EXECUTABLE_SUFFIX}" "${CMAKE_CURRENT_SOURCE_DIR}/jellyfish/"
)
elseif (${CMAKE_GENERATOR} MATCHES "Xcode" )

endif (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )
