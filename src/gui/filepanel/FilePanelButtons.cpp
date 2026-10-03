#include "FilePanelButtons.h"


#include "FilePanel.h"
#include "FilePanelMenu.h"
#include "datatypes/file.h"
#include "src/filemanagement/FileReader.h"
#include "src/filemanagement/FileTree.h"

#include <gio/gio.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <gtk/gtkshortcut.h>
#include <memory>

/*
 * FilePanelButton base class
 */

static void on_file_panel_button_right_clicked(GtkGestureClick* self, int n_press, double x, double y, FilePanelButton* parent){
    parent->ShowMenu();
}

static void on_file_panel_button_renamed(GtkEditableLabel* label, GParamSpec *pspec G_GNUC_UNUSED, FilePanelButton* parent){
    if (!parent->GetIsEditing()) {
        parent->RenameEnded();
    }
}

FilePanelButton::FilePanelButton(Branch& branch, unsigned int level, AppContext& context) :
    m_branch(branch),
    m_level(level),
    m_context(context)
    {
    m_icon = GTK_IMAGE(gtk_image_new());
    gtk_image_set_pixel_size(GTK_IMAGE(m_icon), 16);
    gtk_widget_set_margin_start(GTK_WIDGET(m_icon), FilePanel::Offset * m_level);

    m_nameLabel = GTK_LABEL(gtk_label_new(""));
    gtk_widget_set_halign(GTK_WIDGET(m_nameLabel), GTK_ALIGN_START);

    m_nameEditLabel = GTK_EDITABLE_LABEL(gtk_editable_label_new(""));
    gtk_widget_set_halign(GTK_WIDGET(m_nameEditLabel), GTK_ALIGN_START);
    gtk_widget_set_visible(GTK_WIDGET(m_nameEditLabel), false);

    m_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5));
    gtk_box_append(m_box, GTK_WIDGET(m_icon));
    gtk_box_append(m_box, GTK_WIDGET(m_nameLabel));
    gtk_box_append(m_box, GTK_WIDGET(m_nameEditLabel));

    m_button = GTK_BUTTON(gtk_button_new());
    g_object_ref(m_button);
    gtk_button_set_child(m_button, GTK_WIDGET(m_box));

    // right click
    m_rightClickGesture = GTK_GESTURE_CLICK(gtk_gesture_click_new());
    gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(m_rightClickGesture), 3);
    g_signal_connect(m_rightClickGesture, "pressed", G_CALLBACK(on_file_panel_button_right_clicked), this);
    gtk_widget_add_controller(GTK_WIDGET(m_button), GTK_EVENT_CONTROLLER(m_rightClickGesture));

    // rename callback
    g_signal_connect(m_nameEditLabel, "notify::editing", G_CALLBACK(on_file_panel_button_renamed), this);

    this->LoadData();
}

FilePanelButton::~FilePanelButton(){
    m_menu.reset();
    g_object_unref(m_button);
}

void FilePanelButton::LoadData(){
    const FileData* data = m_branch.GetFileData();
    gtk_label_set_text(m_nameLabel, data->fileName.c_str());
    gtk_editable_set_text(GTK_EDITABLE(m_nameEditLabel), data->fileName.c_str());
    gtk_image_set_from_gicon(m_icon, data->icon);
}

void FilePanelButton::ShowMenu(){
    m_menu->Show();
}

bool FilePanelButton::GetIsEditing(){
    return gtk_editable_label_get_editing(m_nameEditLabel);
}

GtkButton* FilePanelButton::GetButton(){
    return m_button;
}

void FilePanelButton::Rename(){
    gtk_widget_set_visible(GTK_WIDGET(m_nameLabel), false);
    gtk_widget_set_visible(GTK_WIDGET(m_nameEditLabel), true);
    gtk_editable_label_start_editing(m_nameEditLabel);
}

void FilePanelButton::RenameEnded(){
    gtk_widget_set_visible(GTK_WIDGET(m_nameLabel), true);
    gtk_widget_set_visible(GTK_WIDGET(m_nameEditLabel), false);
    const char* name = gtk_editable_get_text(GTK_EDITABLE(m_nameEditLabel));
    m_branch.Rename(name);
    gtk_label_set_text(m_nameLabel, name);
}

void FilePanelButton::Delete(){
    FolderBranch* parent = m_branch.GetParent();
    FPFolderButton* parentfolderbtn = static_cast<FPFolderButton*>(parent->GetFilePanelButton());

    if (!parentfolderbtn) {
        return;
    }

    const FileData* fd = m_branch.GetFileData();
    if (fd->type == G_FILE_TYPE_REGULAR) {
        FileBranch* b = dynamic_cast<FileBranch*>(&m_branch);
        parent->RemoveChildFile(b);
        parentfolderbtn->RemoveChildFile(static_cast<FPFileButton*>(b->GetFilePanelButton()));
        m_branch.Delete();
    } else if (m_branch.GetFileData()->type == G_FILE_TYPE_DIRECTORY) {
        FolderBranch* b = dynamic_cast<FolderBranch*>(&m_branch);
        parent->RemoveChildFolder(b);
        parentfolderbtn->RemoveChildFolder(static_cast<FPFolderButton*>(b->GetFilePanelButton()));
        m_branch.Delete();
    }
}


/*
 * FPFolderButton class
 */

static void on_folder_button_clicked(GtkButton* self, FPFolderButton* parent){
    parent->ToggleFolder();
}

FPFolderButton::FPFolderButton(FolderBranch &folderbranch, int level, AppContext& context) : FilePanelButton(folderbranch, level, context)
    , m_folderBranch(folderbranch){

    m_baseBox = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    gtk_box_append(m_baseBox, GTK_WIDGET(m_button));

    m_folderArea = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    gtk_widget_set_visible(GTK_WIDGET(m_folderArea), false);
    gtk_box_append(m_baseBox, GTK_WIDGET(m_folderArea));

    m_fileArea = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    gtk_widget_set_visible(GTK_WIDGET(m_fileArea), false);
    gtk_box_append(m_baseBox, GTK_WIDGET(m_fileArea));

    m_menu = std::make_unique<FPFolderMenu>(*this, context);

    gtk_widget_add_css_class(GTK_WIDGET(m_button), std::string("folder-button").c_str());

    g_signal_connect(m_button, "clicked", G_CALLBACK(on_folder_button_clicked), this);// expand and collapse folder
}

FPFolderButton::~FPFolderButton(){}


void FPFolderButton::AddChildFolder(std::unique_ptr<FPFolderButton> child){
    gtk_box_prepend(m_folderArea, child->GetBaseWidget());
    m_childFolders.emplace_back(std::move(child));
}

void FPFolderButton::RemoveChildFolder(FPFolderButton* child){
    int itr = 0;
    for (std::unique_ptr<FPFolderButton>& item : m_childFolders) {
        if (item.get() == child) {
            gtk_box_remove(m_folderArea, child->GetBaseWidget());
            m_childFolders.erase(m_childFolders.begin() + itr);
            break;
        }
        itr ++;
    }
}

void FPFolderButton::AddChildFile(std::unique_ptr<FPFileButton> child){
    gtk_box_prepend(m_fileArea, child->GetBaseWidget());
    m_childFiles.emplace_back(std::move(child));
}

void FPFolderButton::RemoveChildFile(FPFileButton* child){
    int itr = 0;
    for (std::unique_ptr<FPFileButton>& item : m_childFiles) {
        if (item.get() == child) {
            gtk_box_remove(m_fileArea, child->GetBaseWidget());
            m_childFiles.erase(m_childFiles.begin() + itr);
            break;
        }
        itr ++;
    }
}

void FPFolderButton::ToggleFolder(){
    if(!m_folderBranch.GetIsChildLoaded()){
        m_folderBranch.Load();

        for (const std::unique_ptr<FileBranch>& b : m_folderBranch.GetChildFiles()){
            std::unique_ptr<FPFileButton> childfilebtn = std::make_unique<FPFileButton>(*b, m_level + 1, m_context);
            this->AddChildFile(std::move(childfilebtn));
        }

        for (const std::unique_ptr<FolderBranch>& b : m_folderBranch.GetChildFolders()){
            std::unique_ptr<FPFolderButton> childfolderbtn = std::make_unique<FPFolderButton>(*b, m_level + 1, m_context);
            this->AddChildFolder(std::move(childfolderbtn));
        }
    }

    m_isOpen = !m_isOpen;
    gtk_widget_set_visible(GTK_WIDGET(m_folderArea), m_isOpen);
    gtk_widget_set_visible(GTK_WIDGET(m_fileArea), m_isOpen);
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

FPFileButton::FPFileButton(FileBranch &filebranch, int level, AppContext& context) : FilePanelButton(filebranch, level, context),
    m_fileBranch(filebranch)
{
    m_menu = std::make_unique<FPFileMenu>(*this, context);

    gtk_widget_add_css_class(GTK_WIDGET(m_button), "file-button");
    gtk_widget_set_hexpand(GTK_WIDGET(m_button), true);

    //m_fileBranch.ConnectOpened(EventReceiver *receiver, void (EventReceiver::*callback)(FileBranch *))

    g_signal_connect(m_button, "clicked", G_CALLBACK(on_file_button_click), this);
}

FPFileButton::~FPFileButton(){}

void FPFileButton::Clicked(){
    m_context.sessionManager->GetEditorsManager().EditFile(m_fileBranch);
}

GtkWidget* FPFileButton::GetBaseWidget(){
    return GTK_WIDGET(m_button);
}
