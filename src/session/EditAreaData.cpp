#include "EditAreaData.h"

#include "SessionEvent.h"
#include "datatypes/common.h" // for Language
#include "editarea/EditArea.h" // for EditArea
#include "layouts/tab/CfTabLayout.h"
#include "src/filemanagement/FileCallback.h"
#include "src/filemanagement/FileManagement_if.h"
#include "src/filemanagement/FileOperation.h"
#include "src/filemanagement/FileTree.h"
#include "src/session/FileData.h"
#include "src/session/TabData.h"
#include "toolset/event/Event.h"

#include <cstdio>
#include <unordered_map>
#include <unordered_set>

/*
 * Type def
 */


typedef void (*EditAreaCreatedCallback)(EditArea*);
typedef void (*EditAreaFocusedChangedCallback)(EditArea*);
typedef void (*EditAreaLangChangedCallback)(EditArea*, const char*);

/*
 * Return thes empty set if no editarea set found
 */
static std::unordered_set<EditArea*> empty_editarea_set;

static std::unordered_set<EditArea*> editarea_set;
static std::unordered_map<const Language*, std::unordered_set<EditArea*>> lang_to_editareas_map;

static std::unordered_map<const FileData*, EditArea*> file_data_to_editareas_map;

static EditArea* focused_editarea = nullptr;

/*
 * Callbacks
 */
void OnEditAreaClosed(EditArea* ea){
    auto itr = lang_to_editareas_map.find(ea->GetLanguage());
    if (itr != lang_to_editareas_map.end()) {
        itr->second.erase(ea);
    }

    editarea_set.erase(ea);
}


namespace session {

const std::unordered_set<EditArea*> &GetAllEditAreas(){
    return editarea_set;
}

const std::unordered_set<EditArea*> &GetEditAreasByLanguage(const Language* lang){
    auto itr = lang_to_editareas_map.find(lang);
    if (itr != lang_to_editareas_map.end()) {
        return itr->second;
    } else {
        return empty_editarea_set;
    }
}

EditArea *FindEditAreaByFileData(const FileData* file){
    Workspace* ws = FindWorkspaceByPath(file->absoPath);
    if (ws) {
        FileData* fd = ws->FindChildByPath(file->absoPath);
        return fd->editArea;
    }
    return nullptr;
};

EditArea* FindEditAreaByPath(const char* absopath){
    Workspace* ws = FindWorkspaceByPath(absopath);
    if (ws) {
        FileData* fd = ws->FindChildByPath(absopath);
        if (fd) {
            return fd->editArea;
        }
        return nullptr;
    }
    return nullptr;
}

//callbacks

void SetFocusedEditArea(EditArea* editarea){
    focused_editarea = editarea;
    const SimpleEvent &event = GetEvent(EDITAREA_FOCUSED_CHANGED);
    for(EventCallback callback : event.GetCallbackSet()){
        ((EditAreaFocusedChangedCallback)callback)(focused_editarea);
    }
}


/*
 * File editing
 */

void EditNewFile(){
    FileData* vfile = filemanager::CreateVirtualFile();
    FileBranch* branch = new FileBranch(vfile);
    Workspace* vws = FindWorkspaceByPath(vfile->absoPath);
    if (!vws){
        vws = NewWorkspace(filemanager::CreateVirtualFolder());
    }
    vws->AddChildFile(branch);
    EditArea* ea = new EditArea(vfile);//freed on EditArea closed(Editarea.Destroy())
    SetFocusedEditArea(ea);

    const SimpleEvent &event = GetEvent(EDITAREA_CREATED);
    for(EventCallback callback : event.GetCallbackSet()){
        ((EditAreaCreatedCallback)callback)(ea);
    }

    ea->Listen(EditArea::CLOSED, (EventCallback)OnEditAreaClosed);
    CfTabLayout* tablayout = session::GetFocusedTabLayout();
    if(tablayout){
        tablayout->Show(ea);
    }
}

void EditFile(FileData *file){
    EditArea* ea = FindEditAreaByFileData(file);
    if(ea){
        CfTabLayout* tablayout = session::GetFocusedTabLayout();
        if(tablayout){
            tablayout->Show(ea);
        }
        return;
    }

    ea = new EditArea(file);//freed on EditArea closed(Editarea.Destroy())
    SetFocusedEditArea(ea);
    file_data_to_editareas_map.emplace(file, ea);

    const SimpleEvent &event = GetEvent(EDITAREA_CREATED);
    for(EventCallback callback : event.GetCallbackSet()){
        ((EditAreaCreatedCallback)callback)(ea);
    }

    CfTabLayout* tablayout = session::GetFocusedTabLayout();
    if(tablayout){
        tablayout->Show(ea);
    }
}

void CloseFile(FileData *file){
    auto itr = file_data_to_editareas_map.find(file);
    if (itr != file_data_to_editareas_map.end()) {
        const Language* lang = itr->second->GetLanguage();
        lang_to_editareas_map.at(lang).erase(itr->second);
        editarea_set.erase(itr->second);
        file_data_to_editareas_map.erase(file);
    }
}

void InsertToEditAreaList(EditArea *ea){
    //editarea_set.emplace(ea->GetFilePath(), ea);
    printf("ins called\n");
}

void RemoveFromEditAreaList(EditArea* ea){
    //editarea_set.erase(ea->GetFilePath());
    printf("rem called\n");
}

}// namespace session
