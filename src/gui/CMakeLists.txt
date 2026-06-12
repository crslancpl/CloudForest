project(GUILIB)

set(COMPONENTS
    components/CfContent.cpp
    components/Flyout.cpp
    components/TextArea.cpp
    components/ToggleBlock.cpp
)

set(EDITAREA
    editarea/LangPanel.cpp
    editarea/DiagnosticPanel.cpp
    editarea/Popovers.cpp
    editarea/SearchReplaceDialog.cpp
    editarea/EditArea_if.cpp
    editarea/EditArea.cpp
)

set(FILEPANEL
    filepanel/FilePanel.cpp
    filepanel/FilePanelButtons.cpp
)

set(HEADERBAR
    headerbar/Headerbar.cpp
)

set(LAYOUTS
    layouts/layout/CfLayout.cpp
    layouts/tab/CfTabLayout.cpp
    layouts/tab/CfTab_if.cpp
    layouts/tab/CfTabSwitcher.cpp
)

set(WINDOWS
    windows/Window.cpp
    windows/MainWindow.cpp
)

set(SETTINGPANEL
    settingpanel/SettingPanel.cpp
    settingpanel/SettingPage.cpp
    settingpanel/ExtensionPage.cpp
)

set(STYLE
    style/Style.cpp
)

add_library(${PROJECT_NAME} STATIC
    ${COMPONENTS}
    ${EDITAREA}
    ${FILEPANEL}
    ${HEADERBAR}
    ${LAYOUTS}
    ${WINDOWS}
    ${SETTINGPANEL}
    ${STYLE}

    Gui_if.cpp
)


target_link_libraries(${PROJECT_NAME}
    PkgConfig::GTK4 Python::Python
)

target_include_directories(${PROJECT_NAME} PRIVATE
  ${CLOUDFOREST_ROOT_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
)
