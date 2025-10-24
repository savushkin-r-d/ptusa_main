# Define the source folder and the symlink name
set(SOURCE_FOLDER "${CMAKE_CURRENT_LIST_DIR}/luassert/src")
set(COPY_FOLDER "${PROJECT_BINARY_DIR}/$<CONFIG>/luassert")

add_custom_target(luassert_copy ALL
    COMMAND ${CMAKE_COMMAND} -E echo "luassert -> ${COPY_FOLDER}"
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${COPY_FOLDER}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${COPY_FOLDER}"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${SOURCE_FOLDER}" "${COPY_FOLDER}" VERBATIM )
