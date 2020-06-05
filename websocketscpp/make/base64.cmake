set(HEADER_DIR ${ROOT_DIR}/ext-lib/base64)
set(SOURCE_DIR ${ROOT_DIR}/ext-lib/base64)

add_library(base64 SHARED
	${SOURCE_DIR}/base64.cpp
	${SOURCE_DIR}/base64.h
)

target_include_directories(base64
    PUBLIC ${HEADER_DIR}
)
