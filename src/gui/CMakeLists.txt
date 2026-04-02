project(GUILIB)

set(COMPONENTS
    components/TextArea.cpp
    components/CfContent.cpp
)

set(EDITAREA
    editarea/LangPanel.cpp
    editarea/LspPopovers_if.cpp
    editarea/LspPopovers.cpp
    editarea/SearchReplaceDialog.cpp
    editarea/EditArea_if.cpp
    editarea/EditArea.cpp
)

set(FILEPANEL
    filepanel/FilePanel_if.cpp
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

set(MAINWINDOW
    windows/MainWindow.cpp
    windows/MainWindow_if.cpp
)

set(SETTINGPANEL
    settingpanel/SettingPanel.cpp
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
    ${MAINWINDOW}
    #${SETTINGPANEL}
    ${STYLE}

    Gui_if.cpp
)


target_link_libraries(${PROJECT_NAME}
    PkgConfig::GTK4 Python::Python
)

target_include_directories(${PROJECT_NAME} PUBLIC ${CLOUDFOREST_ROOT_DIR})
