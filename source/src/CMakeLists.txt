cmake_minimum_required(VERSION 3.6)

include_directories(
    ${CMAKE_SOURCE_DIR}/libs/flatbuffers}
    ${TENSORFLOW_LITE_INC}
    inc
)

aux_source_directory (./ DIR_USER_SRCS)

file(GLOB C_SOURCE
    ${OTHER_SRC_FILES}
#Add the local and project specific files
    ${DIR_USER_SRCS}
)

add_executable(${PROJECT_NAME}
    ${C_SOURCE}
    ${EXTERNAL_EXECUTABLES}
)

if (EXTERNAL_DEPENDENCIES)
    add_dependencies(${PROJECT_NAME}
        ${EXTERNAL_DEPENDENCIES}
    )
endif()

if (EXTERNAL_LIBS)
    target_link_libraries(${PROJECT_NAME}
        ${EXTERNAL_LIBS}
    )
endif()
