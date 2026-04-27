#include "FileOperation.h"

#include "FileCallback.h"
#include "FileOperation.h"
#include "src/gui/Gui_if.h"
#include "src/gui/windows/MainWindow.h"

#include <gio/gio.h>
#include <gtk/gtk.h>

static GtkFileDialog *file_dialog;
static char *file_content;
static void (*file_saved_callback)(GFile*);

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
        filemanagement::SaveFile(file, file_content, file_saved_callback);
    }else{
        //Callback(nullptr, nullptr);
    }
}

void filemanagement::ChooseFile(){
    gtk_file_dialog_set_title(file_dialog, "choose a file");
    gtk_file_dialog_open(file_dialog, gui::g_mainwindow->GetGtkWindow(), nullptr, FileSelected, nullptr);
}

void filemanagement::ChooseFolder(){
    gtk_file_dialog_set_title(file_dialog, "Choose a folder");
    gtk_file_dialog_select_folder(file_dialog, gui::g_mainwindow->GetGtkWindow(), nullptr, FolderSelected, nullptr);
}

/*
void filemanagement::CreateFile(GFile* file){
    gtk_file_dialog_set_title(file_dialog, "Save file");
    gtk_file_dialog_save(file_dialog, gui::g_mainwindow->m_window, nullptr, FileSaved, nullptr);
}
*/


void filemanagement::SaveFile(GFile *file, char *content, void (*savedcallback)(GFile*)){
    if(!file){
        file_content = content;
        file_saved_callback = savedcallback;
        gtk_file_dialog_set_title(file_dialog, "Save file");
        gtk_file_dialog_save(file_dialog, gui::g_mainwindow->GetGtkWindow(), nullptr, FileSaved, nullptr);
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

    if(savedcallback){
        savedcallback(file);
    }
}
