project(PYTHONBACKENDLIB)

set(EDITAREA
    editarea/editarea_mod_Py.cpp
    editarea/editarea_class_Py.cpp
)

add_library(${PROJECT_NAME} STATIC
    cloudforest_mod_Py.cpp
    language_mod_Py.cpp
    python_if.cpp
    ${EDITAREA}
)

target_include_directories(${PROJECT_NAME} PUBLIC ${CLOUDFOREST_ROOT_DIR})

target_link_libraries(${PROJECT_NAME} Python::Python PkgConfig::GTK4)
