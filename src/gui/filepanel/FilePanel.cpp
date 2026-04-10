#include "FilePanel.h"


#include <cstdlib>
#include <gio/gio.h>
#include <glib-object.h>
#include <glib.h>
#include <glib/gprintf.h>
#include <gtk/gtk.h>
#include <gtk/gtkshortcut.h>

#include "FilePanelButtons.h"

/*
 * FilePanel class
 */
unsigned short FilePanel::Offset = 20;

FilePanel::FilePanel(){
    GtkBuilder *builder =  gtk_builder_new_from_file("data/ui/FilePanel.ui");
    m_fileTree = GTK_BOX(gtk_builder_get_object(builder, "FileTree"));

    SetDefaultSize(270, 20);
    SetHorizontalExpand(false);
    SetVerticalExpand(true);
    gtk_widget_set_hexpand(GTK_WIDGET(m_fileTree), true);
    gtk_widget_set_vexpand(GTK_WIDGET(m_fileTree), true);
    gtk_box_set_spacing(m_fileTree, 5);
    gtk_widget_add_css_class(GTK_WIDGET(m_fileTree), "file-tree");
    SetContentWidget(GTK_WIDGET(m_fileTree));
    g_object_unref(builder);
}



FPFolderButton* FilePanel::FindFolderBut_GFile(GFile *folder){
    char* foldername = g_file_get_basename(folder);
    /*
    for(std::shared_ptr<FPFolderButton> &folderbut: m_folderButtons){
        if(strcmp(folderbut->m_fileData->FileName, foldername)== 0){
            return folderbut.get();
        }
    }
    */
    return nullptr;
}

FPFileButton* FilePanel::FindFileBut_GFile(GFile *file){
    /*
    for(std::shared_ptr<FPFileButton> &filebut: m_fileButtons){
        if(filebut->m_fileData->File == file){
            return filebut.get();
        }
    }
    */
    return nullptr;
}

void FilePanel::AddNewRoot(FPFolderButton* folderbutton){
    gtk_box_append(m_fileTree,GTK_WIDGET(folderbutton->m_baseBox));
}

void FilePanel::AddNewRoot(FPFileButton* filebutton){
    gtk_box_append(m_fileTree,GTK_WIDGET(filebutton->m_button));
}

void FilePanel::NewFolder(GFile *folder,GFile *parentfolder,FPFolderButton *parentfolderbut){
    if(parentfolderbut){
        parentfolderbut->AddChildFolder(new FPFolderButton(folder, parentfolderbut->Level+1));
    }else{
        AddNewRoot(new FPFolderButton(folder, 0));
    }
}

void FilePanel::NewFile(GFile *file, FPFolderButton* parentfolderbut){
    if(parentfolderbut){
        parentfolderbut->AddChildFile(new FPFileButton(file, parentfolderbut->Level+1));
    }else{
        AddNewRoot(new FPFileButton(file, 0));
    }
}
