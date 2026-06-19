project(SESSIONLIB)

add_library(${PROJECT_NAME}
  EditAreaData.cpp
  FileData.cpp
  SessionEvent.cpp
  TabData.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC
  ${CLOUDFOREST_ROOT_DIR}
  ${CLOUDFOREST_ROOT_DIR}/src/gui
)

target_link_libraries(${PROJECT_NAME} PUBLIC
  PkgConfig::GTK4
)
