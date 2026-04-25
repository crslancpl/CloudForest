#include "FilePanelButtons.h"

#include "../editarea/EditArea_if.h"
#include "datatypes/file.h"

#include "FilePanel.h"
#include "../Gui_if.h"
#include "src/filemanagement/FileManagement_if.h"
#include <gtk/gtk.h>

/*
 * For FPFileButton and FPFolderButton. It will load the icon and the file name of
 * the GFile *file and place it into GtkButton *button.
 */
static void ButtonLoadFileNameAndIcon(GtkButton *button,datatypes::FileData *filedata, int level){
    GtkBox *box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5));

    GtkWidget *image = gtk_image_new_from_gicon(filedata->icon);
    gtk_image_set_pixel_size(GTK_IMAGE(image), 16);

    GtkWidget *label = gtk_label_new(filedata->fileName);

    gtk_box_append(box, image);
    gtk_box_append(box, label);
    gtk_button_set_child(button, GTK_WIDGET(box));

    gtk_widget_set_margin_start(image, FilePanel::Offset * level);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
}



/*
 * FPFolderButton class
 */

static FPFolderButton* folder_button_to_enumerate;
static void ChildEnumerated(GFile *file, GFileInfo *info){
     if (file == nullptr || info == nullptr){
         folder_button_to_enumerate = nullptr;
         return;
     };//ended

     GFile *parent = g_file_get_parent(file);

     GFileType type = g_file_info_get_file_type(info);
     if(type == G_FILE_TYPE_DIRECTORY){
         gui::g_filepanel->NewFolder(file, parent, folder_button_to_enumerate);
     }else if(type == G_FILE_TYPE_REGULAR){
         gui::g_filepanel->NewFile(file, folder_button_to_enumerate);
     }
 }

static void ToggleFolder(GtkButton* self,FPFolderButton *filefolderbut){
    if(!filefolderbut->ChildLoaded){
        folder_button_to_enumerate = filefolderbut;
        filemanagement::EnumerateFolderChild(filefolderbut->m_fileData->file, ChildEnumerated);
        filefolderbut->ChildLoaded = true;
    }

    gtk_widget_set_visible(GTK_WIDGET(filefolderbut->m_content) , !filefolderbut->IsOpened);
    filefolderbut->IsOpened = !filefolderbut->IsOpened;
}


FPFolderButton::FPFolderButton(GFile *folder, int level){
    Level=level;
    /* binding */
    builder = gtk_builder_new_from_file("data/ui/FPFolderButton.ui");
    m_baseBox = GTK_BOX(gtk_builder_get_object(builder, "FolderBaseBox"));
    m_folderToggleBut = GTK_BUTTON(gtk_builder_get_object(builder, "FolderToggleBut"));
    m_content = GTK_BOX(gtk_builder_get_object(builder, "Content"));
    m_fileData = filemanagement::LoadFileData(folder);

    ButtonLoadFileNameAndIcon(m_folderToggleBut, m_fileData , level);

    gtk_widget_add_css_class(GTK_WIDGET(m_folderToggleBut), std::string("FolderButton").c_str());
    gtk_widget_set_visible(GTK_WIDGET(m_content), false);

    g_signal_connect(m_folderToggleBut, "clicked", G_CALLBACK(ToggleFolder),this);// expand and collapse folder
}

FPFolderButton::~FPFolderButton(){
    g_object_unref(m_fileData->file);
}


void FPFolderButton::AddChildFolder(FPFolderButton* Child){
    gtk_box_append(m_content, GTK_WIDGET(Child->m_baseBox));
}

void FPFolderButton::AddChildFile(FPFileButton* Child){
    gtk_box_append(m_content, GTK_WIDGET(Child->m_button));
}

void FPFolderButton::UnrefBuilder(){
    g_object_unref(G_OBJECT(builder));
}




/*
 * FPFileButton class
 */

static void FileButtonClick(GtkButton *self,FPFileButton &Parent){
    Parent.Open();
}

FPFileButton::FPFileButton(GFile *file, int level){
    m_button = GTK_BUTTON(gtk_button_new());
    m_fileData = filemanagement::LoadFileData(file);
    ButtonLoadFileNameAndIcon(m_button, m_fileData, level);

    gtk_widget_add_css_class(GTK_WIDGET(m_button), "FileButton");
    gtk_widget_set_hexpand(GTK_WIDGET(m_button), true);
    g_signal_connect(m_button, "clicked", G_CALLBACK(FileButtonClick), this);
}

FPFileButton::~FPFileButton(){
    g_object_unref(m_fileData->file);
}

void FPFileButton::Open(){
    editarea::OpenFile(m_fileData->file);
}
