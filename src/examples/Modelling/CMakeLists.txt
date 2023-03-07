project(Modelling)

# Build all .cxx files in the directory
file(GLOB ALL_FILES *.cxx)

exec_program("basename ${CMAKE_CURRENT_SOURCE_DIR}" OUTPUT_VARIABLE DIR_NAME)

foreach(SOURCE_FILE ${ALL_FILES})
  string(REPLACE ".cxx" "" TMP ${SOURCE_FILE})
  string(REPLACE ${CMAKE_CURRENT_SOURCE_DIR}/ "" EXAMPLE ${TMP})

  add_executable(${EXAMPLE} ${EXECUTABLE_FLAG} ${EXAMPLE}.cxx)
  target_link_libraries(${EXAMPLE} PRIVATE ${VTK_LIBRARIES} extend)
  install(TARGETS ${EXAMPLE} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/${DIR_NAME})
endforeach()
