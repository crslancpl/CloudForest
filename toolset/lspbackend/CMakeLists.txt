project(lspbackend)

add_library(${PROJECT_NAME} STATIC
  LspClient.cpp
  MessageWriter.cpp
  MessageReader.cpp
  ChildProcess.cpp
)

add_subdirectory(messages)

target_link_libraries(${PROJECT_NAME} PUBLIC
  messages
)
