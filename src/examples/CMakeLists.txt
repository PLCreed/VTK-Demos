exec_program("find ${CMAKE_CURRENT_SOURCE_DIR}/ -maxdepth 1 -mindepth 1 -type d | xargs -n 1 basename | xargs" OUTPUT_VARIABLE DIR_STRING)
set(DIR_LIST ${DIR_STRING})
separate_arguments(DIR_LIST)
list(LENGTH DIR_LIST LIST_LENGTH)
message("-- LIST_LENGTH=${LIST_LENGTH}, DIR_LIST=${DIR_LIST}")

foreach(SOURCE_DIR ${DIR_LIST})
    add_subdirectory(${SOURCE_DIR})
endforeach()