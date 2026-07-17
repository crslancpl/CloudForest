project(LANGMANAGLIB)

add_library( ${PROJECT_NAME}
    LanguageGroup.cpp
    LanguageManager_if.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC
  ${CLOUDFOREST_ROOT_DIR}
  ${CLOUDFOREST_ROOT_DIR}/src/gui
)

target_link_libraries(${PROJECT_NAME} PUBLIC
  PkgConfig::GTK4
  Python::Python
)
