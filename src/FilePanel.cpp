#include "FileManager.h"
#include <cstddef>
#include <cstdlib>
#include <gio/gio.h>
#include <glib-object.h>
#include <glib.h>
#include <glib/gprintf.h>
#include <gtk/gtk.h>
#include <gtk/gtkshortcut.h>
#include <memory>
#include <string>

#include "FilePanel.h"
#include "EditArea.h"
#include "Core.h"
#include "MainWindow.h"

/*
 * FilePanel class
 */
GFile *RootFolder;

int FilePanel::OffSet;

void FilePanel::init(){
    GtkBuilder *builder = gtk_builder_new_from_file("UI/FilePanel.ui");

    BaseGrid = GTK_GRID(gtk_builder_get_object(builder, "BaseGrid"));
    FileTree = GTK_BOX(gtk_builder_get_object(builder, "FileTree"));
    gtk_widget_set_size_request(GTK_WIDGET(BaseGrid), 270, 20);
    gtk_widget_set_hexpand(GTK_WIDGET(BaseGrid), false);
    gtk_box_set_spacing(FileTree, 5);
}

void FilePanel::SetParent(GFile *file){
    RootFolder = file;
}

FPFolderButton& FilePanel::NewFolder(GFile *file,GFile *ParentFolder,FPFolderButton& Parent){
    shared_ptr<FPFolderButton> Child = NewFolderButton();
    Child->init(*file, ParentFolder,Parent.Level+1);
    Parent.AddChildFolder(*Child.get());
    return *Child.get();
}

FPFileButton& FilePanel::NewFile(GFile *file, FPFolderButton& Parent){
    shared_ptr<FPFileButton> childfile = NewFileButton();
    childfile->init(file,Parent.Level+1);
    Parent.AddChildFile(*childfile.get());
    return *childfile.get();
}




/*
 * FPFolderButton class
 */
void FPFolderButton::init(GFile &folder,GFile *parentfolder,int level){
    builder = gtk_builder_new_from_file("UI/FilePanel.ui");
    Folder = &folder;
    Level=level;
    BaseBox = GTK_BOX(gtk_builder_get_object(builder, "FolderBaseBox"));
    FolderToggleBut = GTK_BUTTON(gtk_builder_get_object(builder, "FolderToggleBut"));
    Content = GTK_BOX(gtk_builder_get_object(builder, "Content"));

    if(parentfolder==NULL){
        FolderName = g_file_get_basename(&folder);
    }else {
        FolderName = g_file_get_relative_path(parentfolder,&folder);
    }
    gtk_widget_add_css_class(GTK_WIDGET(FolderToggleBut), string("FolderButton").c_str());
    GtkLabel *FileLab = GTK_LABEL(gtk_label_new(FolderName));

    gtk_widget_set_margin_start(GTK_WIDGET(FileLab), FilePanel::OffSet * Level);
    gtk_label_set_justify(FileLab, GTK_JUSTIFY_LEFT);
    gtk_widget_set_halign(GTK_WIDGET(FileLab), GTK_ALIGN_START);
    gtk_widget_set_valign(GTK_WIDGET(FileLab), GTK_ALIGN_CENTER);
    gtk_button_set_child(FolderToggleBut, GTK_WIDGET(FileLab));
    gtk_widget_set_visible(GTK_WIDGET(Content), false);
    Inited = true;

    g_signal_connect(FolderToggleBut, "clicked", G_CALLBACK(ToggleFolder),this);
}

void FPFolderButton::AddChildFolder(FPFolderButton& Child){
    if(!Inited){
        g_print("Folder::AddChildFolder: init folder first\n");
        return;
    }
    gtk_box_append(Content, GTK_WIDGET(Child.BaseBox));
}

void FPFolderButton::AddChildFile(FPFileButton& Child){
    gtk_box_append(Content, GTK_WIDGET(Child.Button));
}

void FPFolderButton::SetAsRoot(GtkBox *Box){
    gtk_widget_add_css_class(GTK_WIDGET(BaseBox), string("rootfolder").c_str());
    gtk_box_append(Box, GTK_WIDGET(BaseBox));
}

void ToggleFolder(GtkButton* self,FPFolderButton *filefolderbut){
    if(!filefolderbut->ChildLoaded){
        ReadFolder(*filefolderbut->Folder, *filefolderbut);
        filefolderbut->ChildLoaded = true;
    }

    gtk_widget_set_visible(GTK_WIDGET(filefolderbut->Content) , !filefolderbut->IsOpen);
    filefolderbut->IsOpen = !filefolderbut->IsOpen;
}




/*
 * FPFileButton class
 */
void FPFileButton::init(GFile *FileGFile,int level){
    Button = GTK_BUTTON(gtk_button_new());

    file = FileGFile;
    FileAbsoPath = g_file_get_path(file);
    FileName = g_file_get_basename(file);
    GtkLabel *L = GTK_LABEL(gtk_label_new(FileName));
    gtk_button_set_child(Button,GTK_WIDGET(L));
    gtk_widget_set_margin_start(GTK_WIDGET(L), FilePanel::OffSet * level);
    gtk_widget_set_halign(GTK_WIDGET(L), GTK_ALIGN_START);
    gtk_widget_add_css_class(GTK_WIDGET(L), string("FileButton").c_str());
    g_signal_connect(Button, "clicked", G_CALLBACK(FileButtonClick),this);
}

void FPFileButton::Open(){
    if(ea == NULL){
        ea = NewEditArea(file, this);
    }
    GetAppWindow().EAHolder->Show(ea);
}

void FileButtonClick(GtkButton *self,FPFileButton &Parent){
    Parent.Open();
}
