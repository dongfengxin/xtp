cmake_minimum_required(VERSION 2.6)

SET(INCLUDE_INSTALL_DIR "${PROJECT_SOURCE_DIR}/include")

install(DIRECTORY include/rapidjson
    DESTINATION "${INCLUDE_INSTALL_DIR}"
    COMPONENT dev)
include_directories(${INCLUDE_INSTALL_DIR})
include_directories(${PROJECT_SOURCE_DIR}/api)
link_directories(${PROJECT_SOURCE_DIR}/api)


file(GLOB APP_HEAD *.h)
file(GLOB APP_SRC *.cpp)
set(XTP_BASE_LIBS 
	  debug xtptraderapi debug xtpquoteapi
	   optimized xtptraderapi optimized xtpquoteapi
	)

# set output path
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/api)
add_executable(XTPApiDemo ${APP_SRC} ${APP_HEAD})
target_link_libraries(XTPApiDemo ${XTP_BASE_LIBS})