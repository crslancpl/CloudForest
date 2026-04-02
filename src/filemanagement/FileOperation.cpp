#include "src/filemanagement/FileOperation.h"
#include "FileManagement_if.h"
#include "FileCallback.h"
#include "src/gui/Gui_if.h"
#include "src/gui/windows/MainWindow.h"

#include <gio/gio.h>

static GtkFileDialog *file_dialog;

void FileOperationInit(){
    file_dialog = gtk_file_dialog_new();
}

static void FileSelected(GObject *source, GAsyncResult *result, void *data){
    GFile *file;
    GFileInfo *fileinfo;
    GError *err = nullptr;
    file = gtk_file_dialog_open_finish(GTK_FILE_DIALOG(source), result, &err);
    if(file == nullptr) {
        // cancelled
        return;
    }
    fileinfo = g_file_query_info(file, "*", G_FILE_QUERY_INFO_NONE, nullptr, &err);
    RunCallback(filemanagement::FILE_EVENT_FILE_CHOOSEN, file, fileinfo);
}

static void FolderSelected(GObject *source, GAsyncResult *result, void *data){
    GFile *file;
    GFileInfo *fileinfo;
    GError *err = nullptr;
    file = gtk_file_dialog_select_folder_finish(GTK_FILE_DIALOG(source), result, &err);
    if(file == nullptr) {
        // cancelled
        return;
    }
    fileinfo = g_file_query_info(file, "*", G_FILE_QUERY_INFO_NONE, nullptr, &err);
    RunCallback(filemanagement::FILE_EVENT_FOLDER_CHOOSEN, file, fileinfo);
}

static void FileSaved(GObject *source, GAsyncResult *result, void* content){
    GFile *file = gtk_file_dialog_save_finish(GTK_FILE_DIALOG(source), result, nullptr);

    if(file != nullptr){
        //filemanag::Save(file, nullptr);
    }else{
        //Callback(nullptr, nullptr);
    }
}

void filemanagement::ChooseFile(){
    gtk_file_dialog_set_title(file_dialog, "choose a file");
    gtk_file_dialog_open(file_dialog, gui::g_mainwindow->m_window, nullptr, FileSelected, nullptr);
}

void filemanagement::ChooseFolder(){
    gtk_file_dialog_set_title(file_dialog, "Choose a folder");
    gtk_file_dialog_select_folder(file_dialog, gui::g_mainwindow->m_window, nullptr, FolderSelected, nullptr);
}

/*
void filemanagement::SaveFile(){
    gtk_file_dialog_set_title(file_dialog, "Save file");
    gtk_file_dialog_save(file_dialog, gui::g_mainwindow->m_window, nullptr, FileSaved, gpointer user_data)
}
*/
void filemanagement::SaveFile(GFile *file, char *content, void (*savedcallback)(GFile*)){
    if(!file){
        return;
    }

    g_file_replace_contents(
        file,
        content,
        strlen(content),
        nullptr,
        false,
        GFileCreateFlags::G_FILE_CREATE_REPLACE_DESTINATION,
        nullptr,
        nullptr,
        nullptr);

    savedcallback(file);
}
