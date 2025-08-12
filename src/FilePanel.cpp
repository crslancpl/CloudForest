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
    builder =  gtk_builder_new_from_file("UI/FilePanel.ui");

    BaseGrid = GTK_GRID(gtk_builder_get_object(builder, "BaseGrid"));
    FileTree = GTK_BOX(gtk_builder_get_object(builder, "FileTree"));
    gtk_widget_set_size_request(GTK_WIDGET(BaseGrid), 270, 20);// height is set to expand
    gtk_widget_set_hexpand(GTK_WIDGET(BaseGrid), false);
    gtk_box_set_spacing(FileTree, 5);
}

FPFolderButton& FilePanel::NewFolder(GFile *folder,GFile *ParentFolder,FPFolderButton& Parent){
    shared_ptr<FPFolderButton>& Child = NewFolderButton();
    Child->init(*folder, ParentFolder,Parent.Level+1);
    Parent.AddChildFolder(*Child.get());
    return *Child.get();
}

FPFileButton& FilePanel::NewFile(GFile *file, FPFolderButton& Parent){
    shared_ptr<FPFileButton>& childfile = NewFileButton();
    childfile->init(file,Parent.Level+1);
    Parent.AddChildFile(*childfile.get());
    return *childfile.get();
}

void FilePanel::UnrefBuilder(){
    g_object_unref(G_OBJECT(builder));
}


/*
 * FPFolderButton class
 */
void FPFolderButton::init(GFile &folder,GFile *parentfolder,int level){
    Folder = &folder;
    Level=level;
    /* binding */
    builder = gtk_builder_new_from_file("UI/FilePanel.ui");
    BaseBox = GTK_BOX(gtk_builder_get_object(builder, "FolderBaseBox"));
    FolderToggleBut = GTK_BUTTON(gtk_builder_get_object(builder, "FolderToggleBut"));
    Content = GTK_BOX(gtk_builder_get_object(builder, "Content"));
    FolderName = g_file_get_basename(&folder);// name of folder

    GtkLabel *FileLab = GTK_LABEL(gtk_label_new(FolderName));
    /* set styles */
    gtk_widget_add_css_class(GTK_WIDGET(FolderToggleBut), string("FolderButton").c_str());
    gtk_widget_set_margin_start(GTK_WIDGET(FileLab), FilePanel::OffSet * Level);
    gtk_label_set_justify(FileLab, GTK_JUSTIFY_LEFT);
    gtk_widget_set_halign(GTK_WIDGET(FileLab), GTK_ALIGN_START);
    gtk_widget_set_valign(GTK_WIDGET(FileLab), GTK_ALIGN_CENTER);
    gtk_button_set_child(FolderToggleBut, GTK_WIDGET(FileLab));
    gtk_widget_set_visible(GTK_WIDGET(Content), false);

    g_signal_connect(FolderToggleBut, "clicked", G_CALLBACK(ToggleFolder),this);// expand and collapse folder

    /* Since the program is not finished yet, we use Inited to reduce memory problems */
    Inited = true;
}

void FPFolderButton::AddChildFolder(FPFolderButton& Child){
    if(!Inited){
        /* Check if the Child is inited, or Child.BaseBox will be null and result segmentation fault */
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

void FPFolderButton::UnrefBuilder(){
    g_object_unref(G_OBJECT(builder));
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
    if(ea == nullptr){
        // no corresponding EditArea so create a new one
        ea = NewEditArea(file, this);
    }
    /* It can be shown on any EditAreaHolder, but now only AppWindow's EAHolder exist */
    GetAppWindow().EAHolder->Show(ea);
}

void FileButtonClick(GtkButton *self,FPFileButton &Parent){
    Parent.Open();
}
