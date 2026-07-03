#include "EditAreaData.h"

#include "SessionEvent.h"
#include "datatypes/common.h" // for Language
#include "editarea/EditArea.h" // for EditArea
#include "layouts/tab/CfTabLayout.h"
#include "src/filemanagement/FileCallback.h"
#include "src/filemanagement/FileManagement_if.h"
#include "src/filemanagement/FileOperation.h"
#include "src/languages/LanguageGroup.h"
#include "src/session/FileData.h"
#include "src/session/TabData.h"
#include "toolset/event/Event.h"

#include <cstdio>
#include <unordered_map>
#include <unordered_set>

/*
 * Type def
 */


typedef void (*EditAreaFocusedChangedCallback)(EditArea*);
typedef void (*LanguageUsedCallback)(const Language*);

/*
 * Return thes empty set if no editarea set found
 */
static const std::unordered_set<EditArea*> empty_editarea_set;
static std::unordered_set<EditArea*> all_editarea_set;
static std::unordered_map<const Language*, LanguageGroup*> language_groups_map;

static EditArea* focused_editarea = nullptr;

/*
 * Callbacks
 */
static void OnEditAreaLangChanged(EditArea* ea, const Language* oldlang, const Language* newlang);
static void OnEditAreaClosed(EditArea* ea);

static void OnEditAreaCreated(EditArea* ea){
    //
    ea->Listen(EditArea::LANG_CHANGED,(EventCallback)OnEditAreaLangChanged);
    ea->Listen(EditArea::CLOSED, (EventCallback)OnEditAreaClosed);

    const Language* lang = ea->GetLanguage();
    auto itr = language_groups_map.find(lang);
    if (itr != language_groups_map.end()) {
        itr->second->Add(ea);
    } else {
        LanguageGroup* langgroup = new LanguageGroup(lang);
        language_groups_map.emplace(lang, langgroup);
        langgroup->Add(ea);
        SimpleEvent& event = session::GetEvent(session::LANGUAGE_USED);
        for (EventCallback callback : event.GetCallbackSet()) {
            ((LanguageUsedCallback)callback)(lang);
        }
    }
}

static void OnEditAreaClosed(EditArea* ea){
    auto itr = language_groups_map.find(ea->GetLanguage());
    if (itr != language_groups_map.end()) {
        itr->second->Remove(ea);
    }
    all_editarea_set.erase(ea);
}

static void OnEditAreaLangChanged(EditArea* ea, const Language* oldlang, const Language* newlang){
    auto itr = language_groups_map.find(oldlang);
    if (itr != language_groups_map.end()) {
        itr->second->Remove(ea);
    }
    itr = language_groups_map.find(newlang);
    if (itr != language_groups_map.end()) {
        itr->second->Add(ea);
    } else {
        LanguageGroup* langgroup = new LanguageGroup(newlang);// free on app closed
        language_groups_map.emplace(newlang, langgroup);
        langgroup->Add(ea);
        SimpleEvent& event = session::GetEvent(session::LANGUAGE_USED);
        for (EventCallback callback : event.GetCallbackSet()) {
            ((LanguageUsedCallback)callback)(newlang);
        }
    }
}


namespace session {

void InitEditAreaData(){
    session::Listen(EDITAREA_CREATED, (EventCallback)OnEditAreaCreated);
}

const std::unordered_set<EditArea*> &GetAllEditAreas(){
    return all_editarea_set;
}

const std::unordered_set<EditArea*> &GetEditAreasByLanguage(const Language* lang){
    auto itr = language_groups_map.find(lang);
    if (itr != language_groups_map.end()) {
        return itr->second->GetEditAreaSet();
    } else {
        return empty_editarea_set;
    }
}

EditArea *FindEditAreaByFileData(const FileData* file){
    Workspace* ws = FindWorkspaceByPath(file->absoPath);
    FileData* fd;
    if (ws) {
        fd = ws->FindChildByPath(file->absoPath);
        return fd->editArea;
    } else {
        fd = FindSingleFileByPath(file->absoPath);
        return fd->editArea;
    }
    return nullptr;
};

EditArea* FindEditAreaByPath(const char* absopath){
    Workspace* ws = FindWorkspaceByPath(absopath);
    FileData* fd;
    if (ws) {
        fd = ws->FindChildByPath(absopath);
        if (fd) {
            return fd->editArea;
        }
    } else {
        fd = FindSingleFileByPath(absopath);
        if (fd) {
            return fd->editArea;
        }
    }
    return nullptr;
}

void SetFocusedEditArea(EditArea* editarea){
    focused_editarea = editarea;
    const SimpleEvent &event = GetEvent(EDITAREA_FOCUSED_CHANGED);
    for(EventCallback callback : event.GetCallbackSet()){
        ((EditAreaFocusedChangedCallback)callback)(focused_editarea);
    }
}

LanguageGroup* FindLanguageGroup(const Language* lang){
    auto itr = language_groups_map.find(lang);
    if (itr != language_groups_map.end()) {
        return itr->second;
    }
    return nullptr;
}


/*
 * File editing
 */

void EditNewFile(){
    FileData* filedata = filemanager::CreateNewFile();
    session::AddSingleFile(filedata);

    EditArea* ea = new EditArea(filedata);//freed on EditArea closed(Editarea.Destroy())

    CfTabLayout* tablayout = session::GetFocusedTabLayout();
    if(tablayout){
        tablayout->Show(ea);
        session::SetFocusedEditArea(ea);
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

    CfTabLayout* tablayout = session::GetFocusedTabLayout();
    if(tablayout){
        tablayout->Show(ea);
        session::SetFocusedEditArea(ea);
    }
}

void CloseFile(FileData *file){
    // not yet implemented
}



}// namespace session
