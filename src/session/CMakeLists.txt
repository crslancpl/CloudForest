project(SESSIONLIB)

add_library(${PROJECT_NAME}
  SessionManager.cpp
  EditorsManager.cpp
  TabManager.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC
  ${CLOUDFOREST_ROOT_DIR}
  ${CLOUDFOREST_ROOT_DIR}/src/gui
)

target_link_libraries(${PROJECT_NAME} PUBLIC
  PkgConfig::GTK4 GUILIB
)
