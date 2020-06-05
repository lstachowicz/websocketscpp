set(HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/../include)
set(SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/../src)

add_library(websocketscpp SHARED
	${HEADER_DIR}/Server.h
	${SOURCE_DIR}/Server.cpp
	${SOURCE_DIR}/ClientConnection.cpp
	${SOURCE_DIR}/ClientConnection.h
	${SOURCE_DIR}/Parser.cpp
	${SOURCE_DIR}/Parser.h
	${SOURCE_DIR}/Select.cpp
	${SOURCE_DIR}/ServerPrivate.cpp
	${SOURCE_DIR}/ServerPrivate.h
)

target_include_directories(websocketscpp
    PUBLIC ${HEADER_DIR}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    base64
    sha1
)


