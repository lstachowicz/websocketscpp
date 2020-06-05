set(SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/../sample/server)

add_executable(websocketscpp_sample
	${SOURCE_DIR}/main.cpp
	${SOURCE_DIR}/ServerInstance.cpp
	${SOURCE_DIR}/ServerInstance.h
)

target_link_libraries(websocketscpp_sample
	websocketscpp
)
