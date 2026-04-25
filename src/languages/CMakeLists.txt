project(LANGMANAGLIB)

add_library( ${PROJECT_NAME}
    LanguageManager_if.cpp
    LanguageListener.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC ${CLOUDFOREST_ROOT_DIR})

target_link_libraries(${PROJECT_NAME} PUBLIC PkgConfig::GTK4 Python::Python)
