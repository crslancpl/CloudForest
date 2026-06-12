#include "FilePanelButtons.h"


#include "FilePanel.h"
#include "editarea/EditArea_if.h"
#include "datatypes/file.h"
#include "Gui_if.h"
#include "src/filemanagement/FileManagement_if.h"
#include "src/filemanagement/FileReader.h"

#include <gio/gio.h>
#include <gtk/gtk.h>
#include <gtk/gtkshortcut.h>

/*
 * For FPFileButton and FPFolderButton. It will load the icon and the file name of
 * the GFile *file and place it into GtkButton *button.
 */
static void ButtonLoadFileNameAndIcon(GtkButton *button,FileData *filedata, int level){
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

static void OnFolderClicked(GtkButton* self,FPFolderButton *filefolderbut){
    filefolderbut->ToggleFolder();
}


FPFolderButton::FPFolderButton(FileBranch *folderbranch, int level):m_level(level){
    /* binding */
    builder = gtk_builder_new_from_file("data/ui/FPFolderButton.ui");
    m_baseBox = GTK_BOX(gtk_builder_get_object(builder, "folder-base-box"));
    m_folderToggleBut = GTK_BUTTON(gtk_builder_get_object(builder, "folder-toggle-button"));
    m_childArea = GTK_BOX(gtk_builder_get_object(builder, "child-area"));
    m_folderArea = GTK_BOX(gtk_builder_get_object(builder, "child-folder-area"));
    m_fileArea = GTK_BOX(gtk_builder_get_object(builder, "child-file-area"));
    m_folderBranch = folderbranch;

    ButtonLoadFileNameAndIcon(m_folderToggleBut, m_folderBranch->fileData, level);

    gtk_widget_add_css_class(GTK_WIDGET(m_folderToggleBut), std::string("folder-button").c_str());
    gtk_widget_set_visible(GTK_WIDGET(m_childArea), false);

    g_signal_connect(m_folderToggleBut, "clicked", G_CALLBACK(OnFolderClicked),this);// expand and collapse folder
}

FPFolderButton::~FPFolderButton(){
    //g_object_unref(m_folderData->file);
}


void FPFolderButton::AddChildFolder(FPFolderButton* child){
    gtk_box_append(m_folderArea, child->GetBaseWidget());
}

void FPFolderButton::AddChildFile(FPFileButton* child){
    gtk_box_append(m_fileArea, child->GetBaseWidget());
}

void FPFolderButton::ToggleFolder(){
    if(!m_childLoaded){
        folder_button_to_enumerate = this;
        filemanagement::ExpandFileBranch(m_folderBranch);
        m_childLoaded = true;
    }

    for(FileBranch* b : m_folderBranch->childBranch){
        if(b->fileData->type == G_FILE_TYPE_DIRECTORY){
            FPFolderButton* childfolderbtn = new FPFolderButton(b, m_level + 1);
            this->AddChildFolder(childfolderbtn);
        }else if(b->fileData->type == G_FILE_TYPE_REGULAR){
            FPFileButton* childfilebtn = new FPFileButton(b, m_level + 1);
            this->AddChildFile(childfilebtn);
        }
    }
    m_isOpen = !m_isOpen;
    gtk_widget_set_visible(GTK_WIDGET(m_childArea) , m_isOpen);
}



void FPFolderButton::UnrefBuilder(){
    g_object_unref(G_OBJECT(builder));
}

unsigned int FPFolderButton::GetLevel(){
    return m_level;
}

GtkWidget* FPFolderButton::GetBaseWidget(){
    return GTK_WIDGET(m_baseBox);
}


/*
 * FPFileButton class
 */

static void FileButtonClick(GtkButton *self,FPFileButton &Parent){
    Parent.Open();
}

FPFileButton::FPFileButton(FileBranch *filebranch, int level){
    m_button = GTK_BUTTON(gtk_button_new());
    m_fileBranch = filebranch;
    ButtonLoadFileNameAndIcon(m_button, m_fileBranch->fileData, level);

    gtk_widget_add_css_class(GTK_WIDGET(m_button), "file-button");
    gtk_widget_set_hexpand(GTK_WIDGET(m_button), true);
    g_signal_connect(m_button, "clicked", G_CALLBACK(FileButtonClick), this);
}

FPFileButton::~FPFileButton(){
    //g_object_unref(m_fileData->file);
}

void FPFileButton::Open(){
    filemanagement::OpenFile(m_fileBranch->fileData);
}

GtkWidget* FPFileButton::GetBaseWidget(){
    return GTK_WIDGET(m_button);
}
