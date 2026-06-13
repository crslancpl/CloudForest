project(PYTHONBACKENDLIB)

set(EDITAREA
    editarea/editarea_mod_Py.cpp
    editarea/editarea_class_Py.cpp
)

add_library(${PROJECT_NAME} STATIC
    cloudforest_mod_Py.cpp
    language_mod_Py.cpp
    setting_mod_Py.cpp
    python_if.cpp
    python_tool.cpp
    ${EDITAREA}
)

target_include_directories(${PROJECT_NAME} PUBLIC
  ${CLOUDFOREST_ROOT_DIR}
  ${CLOUDFOREST_ROOT_DIR}/src/gui
)

target_link_libraries(${PROJECT_NAME} Python::Python PkgConfig::GTK4)
