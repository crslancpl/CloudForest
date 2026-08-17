#include "FilePanelButtons.h"


#include "FilePanel.h"
#include "datatypes/file.h"
#include "Gui_if.h"
#include "src/filemanagement/FileReader.h"
#include "src/filemanagement/FileTree.h"
#include "src/session/EditAreaData.h"

#include <gio/gio.h>
#include <gtk/gtk.h>
#include <gtk/gtkshortcut.h>
#include <memory>

/*
 * For FPFileButton and FPFolderButton. It will load the icon and the file name of
 * the GFile *file and place it into GtkButton *button.
 */
static void button_load_file_name_and_icon(GtkButton *button,FileData *filedata, int level){
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

static void on_folder_button_clicked(GtkButton* self, FPFolderButton* parent){
    parent->ToggleFolder();
}

static void on_folder_button_right_clicked(GtkGestureClick* self, int n_press, double x, double y, FPFolderButton* parent){
    parent->ShowOptions();
}

FPFolderButton::FPFolderButton(FolderBranch &folderbranch, int level)
    : m_level(level),
    m_folderBranch(folderbranch){
    /* binding */
    builder = gtk_builder_new_from_file("data/ui/FPFolderButton.ui");
    m_baseBox = GTK_BOX(gtk_builder_get_object(builder, "folder-base-box"));
    m_toggleButton = GTK_BUTTON(gtk_builder_get_object(builder, "folder-toggle-button"));
    m_childArea = GTK_BOX(gtk_builder_get_object(builder, "child-area"));
    m_folderArea = GTK_BOX(gtk_builder_get_object(builder, "child-folder-area"));
    m_fileArea = GTK_BOX(gtk_builder_get_object(builder, "child-file-area"));
    m_rightClickGesture = GTK_GESTURE_CLICK(gtk_gesture_click_new());

    button_load_file_name_and_icon(m_toggleButton, m_folderBranch.GetFileData(), level);

    gtk_widget_add_css_class(GTK_WIDGET(m_toggleButton), std::string("folder-button").c_str());
    gtk_widget_set_visible(GTK_WIDGET(m_childArea), false);
    gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(m_rightClickGesture), 3);
    gtk_widget_add_controller(GTK_WIDGET(m_toggleButton), GTK_EVENT_CONTROLLER(m_rightClickGesture));

    g_signal_connect(m_toggleButton, "clicked", G_CALLBACK(on_folder_button_clicked), this);// expand and collapse folder
    g_signal_connect(m_rightClickGesture, "pressed", G_CALLBACK(on_folder_button_right_clicked), this);
}

FPFolderButton::~FPFolderButton(){
    //g_object_unref(m_folderData->file);
}


void FPFolderButton::AddChildFolder(std::unique_ptr<FPFolderButton> child){
    gtk_box_prepend(m_folderArea, child->GetBaseWidget());
    m_childFolders.emplace_back(std::move(child));
}

void FPFolderButton::AddChildFile(std::unique_ptr<FPFileButton> child){
    gtk_box_prepend(m_fileArea, child->GetBaseWidget());
    m_childFiles.emplace_back(std::move(child));
}

void FPFolderButton::ToggleFolder(){
    if(!m_folderBranch.GetIsChildLoaded()){
        folder_button_to_enumerate = this;
        filemanager::ExpandFolderBranch(m_folderBranch);

        for(const std::unique_ptr<FileBranch>& b : m_folderBranch.GetChildFiles()){
            std::unique_ptr<FPFileButton> childfilebtn = std::make_unique<FPFileButton>(*b, m_level + 1);
            this->AddChildFile(std::move(childfilebtn));
        }

        for (const std::unique_ptr<FolderBranch>& b : m_folderBranch.GetChildFolders()){
            std::unique_ptr<FPFolderButton> childfolderbtn = std::make_unique<FPFolderButton>(*b, m_level + 1);
            this->AddChildFolder(std::move(childfolderbtn));
        }
    }

    m_isOpen = !m_isOpen;
    gtk_widget_set_visible(GTK_WIDGET(m_childArea) , m_isOpen);
}

void FPFolderButton::ShowOptions(){
    //
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

static void on_file_button_click(GtkButton *self, FPFileButton *parent){
    parent->Clicked();
}

static void on_file_button_right_clicked(GtkGestureClick* self, int n_press, double x, double y, FPFileButton* parent){
    parent->ShowOptions();
}

FPFileButton::FPFileButton(FileBranch &filebranch, int level):
    m_fileBranch(filebranch){
    m_button = GTK_BUTTON(gtk_button_new());
    button_load_file_name_and_icon(m_button, m_fileBranch.GetFileData(), level);

    gtk_widget_add_css_class(GTK_WIDGET(m_button), "file-button");
    gtk_widget_set_hexpand(GTK_WIDGET(m_button), true);
    g_signal_connect(m_button, "clicked", G_CALLBACK(on_file_button_click), this);
}

FPFileButton::~FPFileButton(){
    //g_object_unref(m_fileData->file);
}

void FPFileButton::ShowOptions(){
    //
}

void FPFileButton::Clicked(){
    session::EditFile(m_fileBranch.GetFileData());
}

GtkWidget* FPFileButton::GetBaseWidget(){
    return GTK_WIDGET(m_button);
}
