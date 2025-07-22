#include "FileManager.h"

#include <cstddef>
#include <gtk/gtk.h>
#include <memory>

#include "DataTypes.h"


MainWindow *ParentWindow;
GtkFileDialog *FileDia;

using namespace std;

void InitFileManager(MainWindow *parent){
    g_print("init File manager\n");
    ParentWindow = parent;
    FileDia = gtk_file_dialog_new();
}

void OpenFileChooser(bool FileOrDir){
    // Pass True if select files. Pass False if select folder
    if(ParentWindow->Window == NULL){
        g_print("Use 'SetParentWindow()' to set the parent first");
        return;
    }

    if(FileOrDir){
        // open one file
        gtk_file_dialog_set_title(FileDia, "Choose Files");
        gtk_file_dialog_open (FileDia, ParentWindow->Window, NULL, FileSelected, NULL);
    }else{
        // open one folder
        gtk_file_dialog_set_title(FileDia, "Choose Folder");
        gtk_file_dialog_select_folder(FileDia, ParentWindow->Window, NULL, FolderSelected, NULL);
    }
}

void FileSelected(GObject *source, GAsyncResult *result, void *data){
    GFile *File;
    GError *err = NULL;
    File = gtk_file_dialog_open_finish(GTK_FILE_DIALOG(source), result, &err);
    if(File == NULL) {
        // cancelled
        g_print("Cancelled\n");
        return;
    }
    OpenFile(File);
}


void FolderSelected(GObject *source, GAsyncResult *result, void *data){
    GFile *File;
    GError *err = NULL;
    File = gtk_file_dialog_select_folder_finish(GTK_FILE_DIALOG(source), result, &err);
    if(File == NULL) {
        // cancelled
        g_print("Cancelled\n");
        return;
    }

    ReadFolder(File, true);
}

void ReadFolder(GFile *Folder,bool isRoot){

    GFileEnumerator *FileEnum = g_file_enumerate_children(Folder, "", GFileQueryInfoFlags::G_FILE_QUERY_INFO_NONE, NULL, NULL);
    while (true) {
        GFileInfo *info = g_file_enumerator_next_file(FileEnum, NULL,NULL);
        if(info == NULL){
            // All file read
            break;
        }

        if(g_file_info_get_file_type(info) == G_FILE_TYPE_DIRECTORY){
            if (isRoot){
               // FP->NewFolder(FP->FileTree,g_file_enumerator_get_child(FileEnum, info),Folder);
            }else{
               // FP->NewFolder(g_file_enumerator_get_child(FileEnum, info),Folder);
            }
            ReadFolder(g_file_enumerator_get_child(FileEnum, info),false);
        }else if(g_file_info_get_file_type(info) == G_FILE_TYPE_REGULAR){
            g_print("File: %s\n", g_file_info_get_name(info));
            OpenFile(g_file_enumerator_get_child(FileEnum, info));
        }
    }
}

void OpenFile(GFile *File){
    GtkBuilder *b = gtk_builder_new_from_file("UI/FilePanel.ui");
    shared_ptr<EditArea> ea;
    ea = SectionData::GetEditAreaFromFileAbsoPath(g_file_get_path(File));

    if(ea == NULL){
        // Not Opened
        ea = make_shared<EditArea>(File);
        SectionData::AddEditArea(ea);
        //gtk_stack_add_child(ParentWindow->EAHolder->Container, GTK_WIDGET(ea->BaseGrid));
    }else{
        // Opened
    }
    ParentWindow->EAHolder->Show(ea);

    ea->UnrefBuilder();
}
