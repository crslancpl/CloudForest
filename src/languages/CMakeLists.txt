project(LANGMANAGLIB)

add_library( ${PROJECT_NAME}
    LanguageManager_if.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC ${CLOUDFOREST_ROOT_DIR})
