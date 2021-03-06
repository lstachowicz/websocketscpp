set(HEADER_DIR ${ROOT_DIR}/ext-lib/sha1)
set(SOURCE_DIR ${ROOT_DIR}/ext-lib/sha1)

add_library(sha1 SHARED
	${SOURCE_DIR}/sha1.cpp
    ${HEADER_DIR}/sha1.h
)

target_include_directories(sha1
    PUBLIC ${HEADER_DIR}
)

