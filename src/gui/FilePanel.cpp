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

#include "EditArea.h"
#include "FilePanel.h"
#include "MainWindow.h"
#include "guiCore.h"

/*
 * For FPFileButton and FPFolderButton. It will load the icon and the file name of
 * the GFile *file and place it into GtkButton *button.
 */
static void ButtonLoadFileNameAndIcon(GtkButton *button,GFile *file, int level){
    GtkBox *box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5));
    GIcon *icon = g_file_query_info(file,
        "standard::icon",
        G_FILE_QUERY_INFO_NONE,
        nullptr,
        nullptr) ?
        g_file_info_get_icon(g_file_query_info(file,
            "standard::icon",
            G_FILE_QUERY_INFO_NONE,
            nullptr,
            nullptr)) :
        g_content_type_get_icon("text/plain");
    GtkWidget *image = gtk_image_new_from_gicon(icon);
    gtk_image_set_pixel_size(GTK_IMAGE(image), 16);
    GtkWidget *label = gtk_label_new(g_file_get_basename(file));

    gtk_box_append(box, image);
    gtk_box_append(box, label);
    gtk_button_set_child(button, GTK_WIDGET(box));

    gtk_widget_set_margin_start(image, FilePanel::Offset * level);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
}


static void ChildEnumertated(GFile *file, GFileInfo *info){
    if (file == nullptr || info == nullptr)return;//ended
    char *name = g_file_get_basename(file);
    //g_print("%s\n", name);
    GFile *parent = g_file_get_parent(file);
    FPFolderButton *parentfolderbut = gui::AppFilePanel.FindFolderBut_GFile(parent);
    GFileType type = g_file_info_get_file_type(info);
    if (parentfolderbut != nullptr) {
        if(type == G_FILE_TYPE_DIRECTORY){
            gui::AppFilePanel.NewFolder(file, parent, parentfolderbut);
        }else if(type == G_FILE_TYPE_REGULAR){
            gui::AppFilePanel.NewFile(file, parentfolderbut);
        }
    }
}




/*
 * FilePanel class
 */
GFile *RootFolder;
unsigned short FilePanel::Offset = 10;

void FilePanel::LoadRoot(GFile *file, GFileInfo *fileinfo){
    /*
     * Load a new file or folder as root (Level = 0)
     */
    GFileType filetype = g_file_info_get_file_type(fileinfo);
    if( filetype == G_FILE_TYPE_DIRECTORY){
        FPFolderButton &newfolderbut = NewFolder(file, nullptr, nullptr);
    }else if(filetype == G_FILE_TYPE_REGULAR){
        FPFileButton &newfilebut = NewFile(file, nullptr);
    }
}

void FilePanel::init(){
    builder =  gtk_builder_new_from_file("UI/FilePanel.ui");

    BaseGrid = GTK_GRID(gtk_builder_get_object(builder, "BaseGrid"));
    FileTree = GTK_BOX(gtk_builder_get_object(builder, "FileTree"));
    gtk_widget_set_size_request(GTK_WIDGET(BaseGrid), 270, 20);// height is set to expand
    gtk_widget_set_hexpand(GTK_WIDGET(BaseGrid), false);
    gtk_box_set_spacing(FileTree, 5);
    gtk_widget_add_css_class(GTK_WIDGET(FileTree), "file-tree");
}

FPFolderButton* FilePanel::FindFolderBut_GFile(GFile *folder){
    char* foldername = g_file_get_basename(folder);
    for(std::shared_ptr<FPFolderButton> &folderbut: FolderButtons){

        if(folderbut->FolderName == foldername){
            return folderbut.get();
        }
    }
    return nullptr;
}

FPFileButton* FilePanel::FindFileBut_GFile(GFile *file){
    for(std::shared_ptr<FPFileButton> &filebut: FileButtons){
        if(filebut->File == file){
            return filebut.get();
        }
    }
    return nullptr;
}

void FilePanel::AddNewRoot(FPFolderButton& folderbutton){
    gtk_box_append(FileTree,GTK_WIDGET(folderbutton.BaseBox));
}

void FilePanel::AddNewRoot(FPFileButton& filebutton){
    gtk_box_append(FileTree,GTK_WIDGET(filebutton.Button));
}

FPFolderButton& FilePanel::NewFolder(GFile *folder,GFile *parentfolder,FPFolderButton *parentfolderbut){
    FolderButtons.emplace_back(std::make_shared<FPFolderButton>());
    std::shared_ptr<FPFolderButton>& folderbut = FolderButtons.back();
    if(parentfolderbut != nullptr){
        folderbut->init(folder, parentfolder,parentfolderbut->Level+1);
        parentfolderbut->AddChildFolder(*folderbut.get());
    }else{
        //root folder
        folderbut->init(folder, nullptr, 0);
        AddNewRoot(*folderbut.get());
    }
    return *folderbut.get();
}

FPFileButton& FilePanel::NewFile(GFile *file, FPFolderButton* parentfolderbut){
    FileButtons.emplace_back(std::make_shared<FPFileButton>());
    std::shared_ptr<FPFileButton>& filebut = FileButtons.back();
    if(parentfolderbut != nullptr){
        filebut->init(file,parentfolderbut->Level+1);
        parentfolderbut->AddChildFile(*filebut.get());
    }else{
        //show as root
        filebut->init(file, 0);
        AddNewRoot(*filebut.get());
    }
    return *filebut.get();
}

void FilePanel::UnrefBuilder(){
    g_object_unref(G_OBJECT(builder));
}





/*
 * FPFolderButton class
 */

static void ToggleFolder(GtkButton* self,FPFolderButton *filefolderbut){
    if(!filefolderbut->ChildLoaded){
        gui::EnumFolder(filefolderbut->Folder, ChildEnumertated);
        filefolderbut->ChildLoaded = true;
    }

    gtk_widget_set_visible(GTK_WIDGET(filefolderbut->Content) , !filefolderbut->IsOpened);
    filefolderbut->IsOpened = !filefolderbut->IsOpened;
 }


FPFolderButton::~FPFolderButton(){
    g_object_unref(Folder);// g_object_unref(child); moved here
}

void FPFolderButton::init(GFile *folder,GFile *parentfolder,int level){
    Folder = folder;
    Level=level;
    /* binding */
    builder = gtk_builder_new_from_file("UI/FPFolderButton.ui");
    BaseBox = GTK_BOX(gtk_builder_get_object(builder, "FolderBaseBox"));
    FolderToggleBut = GTK_BUTTON(gtk_builder_get_object(builder, "FolderToggleBut"));
    Content = GTK_BOX(gtk_builder_get_object(builder, "Content"));
    FolderName = g_file_get_basename(folder);// name of folder
    FolderPath = g_file_get_path(folder);
    ButtonLoadFileNameAndIcon(FolderToggleBut, Folder, level);
    /* set styles */
    if(parentfolder == nullptr){
        gtk_widget_add_css_class(GTK_WIDGET(BaseBox), std::string("rootfolder").c_str());
    }
    gtk_widget_add_css_class(GTK_WIDGET(FolderToggleBut), std::string("FolderButton").c_str());

    gtk_widget_set_visible(GTK_WIDGET(Content), false);

    g_signal_connect(FolderToggleBut, "clicked", G_CALLBACK(ToggleFolder),this);// expand and collapse folder

}

void FPFolderButton::AddChildFolder(FPFolderButton& Child){
    gtk_box_append(Content, GTK_WIDGET(Child.BaseBox));
}

void FPFolderButton::AddChildFile(FPFileButton& Child){
    gtk_box_append(Content, GTK_WIDGET(Child.Button));
}

void FPFolderButton::UnrefBuilder(){
    g_object_unref(G_OBJECT(builder));
}








/*
 * FPFileButton class
 */

static void FileButtonClick(GtkButton *self,FPFileButton &Parent){
    Parent.Open();// now gfile will not be dangling
}

FPFileButton::~FPFileButton(){
    g_object_unref(File);// g_object_unref(child); moved here
}

void FPFileButton::init(GFile *filegfile, int level) {
    Button = GTK_BUTTON(gtk_button_new());
    File = filegfile;

    ButtonLoadFileNameAndIcon(Button, File, level);

    FileAbsoPath = g_file_get_path(File);
    FileName = g_file_get_basename(File);

    gtk_widget_add_css_class(GTK_WIDGET(Button), "FileButton");
    g_signal_connect(Button, "clicked", G_CALLBACK(FileButtonClick), this);
}

void FPFileButton::Open(){
    auto ea = gui::GetEditArea(FileAbsoPath);
    if((ea) == nullptr){
        // no corresponding EditArea so create a new one
        ea = gui::NewEditArea(File);
    }
    // It can be shown on any EditAreaHolder, but now only AppWindow's EAHolder exist
    gui::FocusedEAHolder->Show(*ea);
}
