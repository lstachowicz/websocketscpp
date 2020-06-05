set(HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/../include)
set(SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/../src)
set(TESTS_DIR ${CMAKE_CURRENT_LIST_DIR}/../test)

# websocketscpp
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

# websocketscpp_tests
add_executable(websocketscpp_tests
	${TESTS_DIR}/parser_tests.cpp
)

target_link_libraries(websocketscpp_tests
	PRIVATE gmock gtest gtest_main websocketscpp
)

add_test(NAME websocketscpp_tests
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
	COMMAND websocketscpp_tests --output-on-failure
) 

# websocketscpp_coverage
setup_target_for_coverage_lcov(NAME websocketscpp_coverage
	EXECUTABLE websocketscpp_tests
	EXCLUDE "${ROOT_DIR}/ext-lib/*" "/Applications/*" "BUILD_DIR*/*" "${TESTS_DIR}/*"
)
