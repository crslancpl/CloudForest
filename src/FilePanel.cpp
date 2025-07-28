#include "DataTypes.h"
#include <cstddef>
#include <cstdlib>
#include <exception>
#include <gio/gio.h>
#include <glib-object.h>
#include <glib.h>
#include <glib/gprintf.h>
#include <gtk/gtk.h>
#include <gtk/gtkshortcut.h>
#include <memory>
#include "FilePanel.h"

GFile *RootFolder;

void FilePanel::SetParent(GFile *File){
    RootFolder = File;
}

shared_ptr<Folder> FilePanel::NewFolder(GFile *File,GFile *ParentFolder,shared_ptr<Folder> Parent){
    shared_ptr<Folder> Child=make_shared<Folder>();
    Child->init(File, ParentFolder,Parent->Level+1);
    Parent->AddChildFolder(Child);
    SectionData::AddFolder(Child);
    return Child;
}

void FilePanel::NewFile(GFile *File, shared_ptr<Folder> Parent){

}

void FilePanel::init(){
    GtkBuilder *builder = gtk_builder_new_from_file("UI/FilePanel.ui");

    BaseGrid = GTK_GRID(gtk_builder_get_object(builder, "BaseGrid"));
    FileTree = GTK_BOX(gtk_builder_get_object(builder, "FileTree"));
    gtk_widget_set_size_request(GTK_WIDGET(BaseGrid), 250, 20);
    gtk_widget_set_hexpand(GTK_WIDGET(BaseGrid), false);
    gtk_box_set_spacing(FileTree, 5);
}

int Folder::OffSet;

void Folder::init(GFile *Folder,GFile *Parent,int level){
    builder = gtk_builder_new_from_file("UI/FilePanel.ui");

    Level=level;
    BaseBox = GTK_BOX(gtk_builder_get_object(builder, "FolderBaseBox"));
    FolderToggleBut = GTK_BUTTON(gtk_builder_get_object(builder, "FolderToggleBut"));
    Content = GTK_BOX(gtk_builder_get_object(builder, "Content"));

    if(Parent==NULL){
        FolderName = g_file_get_basename(Folder);
    }else {
        FolderName = g_file_get_relative_path(Parent,Folder);
    }
    gtk_widget_add_css_class(GTK_WIDGET(FolderToggleBut), string("FolderButton").c_str());
    GtkLabel *FileLab = GTK_LABEL(gtk_label_new(FolderName));

    gtk_widget_set_margin_start(GTK_WIDGET(FileLab), OffSet * Level);
    gtk_label_set_justify(FileLab, GTK_JUSTIFY_LEFT);
    gtk_widget_set_halign(GTK_WIDGET(FileLab), GTK_ALIGN_START);
    gtk_widget_set_valign(GTK_WIDGET(FileLab), GTK_ALIGN_CENTER);
    gtk_button_set_child(FolderToggleBut, GTK_WIDGET(FileLab));
    Inited = true;

    g_signal_connect(FolderToggleBut, "clicked", G_CALLBACK(ToggleFolder),this);
}

void ToggleFolder(GtkButton* self,Folder *F){
    gtk_widget_set_visible(GTK_WIDGET(F->Content) , !F->IsOpen);
    F->IsOpen = !F->IsOpen;
}

void Folder::AddChildFolder(shared_ptr<Folder> Child){

    if(!Inited){
        g_print("init folder first\n");
        return;
    }
    gtk_box_append(Content, GTK_WIDGET(Child->BaseBox));
}

void Folder::SetAsRoot(GtkBox *Box){
    gtk_box_append(Box, GTK_WIDGET(BaseBox));
}

void File::init(GFile *FileGFile){
    file = FileGFile;
    FileAbsoPath = g_file_get_path(file);
    FileName = g_file_get_basename(file);
}

void File::Open(){
    if(ea == NULL){
        ea = make_shared<EditArea>(file);
        SectionData::AddEditArea(ea);
    }
}
