#include "EditAreaData.h"

#include "SessionEvent.h"
#include "datatypes/common.h" // for Language
#include "editarea/EditArea.h" // for EditArea
#include "layouts/tab/CfTabLayout.h"
#include "src/filemanagement/FileCallback.h"
#include "src/filemanagement/FileOperation.h"
#include "src/languages/LanguageGroup.h"
#include "src/session/FileData.h"
#include "src/session/TabData.h"
#include "toolset/event/Event.h"

#include <unordered_map>
#include <unordered_set>

/*
 * Type def
 */


typedef void (*EditAreaFocusedChangedCallback)(EditArea*);
typedef void (*LanguageUsedCallback)(const Language*);



typedef struct {
    /*
     * Return thes empty set if no editarea set found
     */
    const std::unordered_set<EditArea*> EMPTY_EDITAREA_SET;

    std::unordered_set<EditArea*> allEditAreaSet;
    std::unordered_map<const Language*, LanguageGroup*> languageGroupsMap;
    EditArea* focusedEditArea;
} SessionEditAreaData;

SessionEditAreaData session_editarea_data;

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
    auto itr = session_editarea_data.languageGroupsMap.find(lang);
    if (itr != session_editarea_data.languageGroupsMap.end()) {
        itr->second->Add(ea);
    } else {
        LanguageGroup* langgroup = new LanguageGroup(lang);
        session_editarea_data.languageGroupsMap.emplace(lang, langgroup);
        langgroup->Add(ea);
        SimpleEvent& event = session::GetEvent(session::LANGUAGE_USED);
        for (EventCallback callback : event.GetCallbackSet()) {
            ((LanguageUsedCallback)callback)(lang);
        }
    }
}

static void OnEditAreaClosed(EditArea* ea){
    auto itr = session_editarea_data.languageGroupsMap.find(ea->GetLanguage());
    if (itr != session_editarea_data.languageGroupsMap.end()) {
        itr->second->Remove(ea);
    }
    session_editarea_data.allEditAreaSet.erase(ea);
}

static void OnEditAreaLangChanged(EditArea* ea, const Language* oldlang, const Language* newlang){
    auto itr = session_editarea_data.languageGroupsMap.find(oldlang);
    if (itr != session_editarea_data.languageGroupsMap.end()) {
        itr->second->Remove(ea);
    }
    itr = session_editarea_data.languageGroupsMap.find(newlang);
    if (itr != session_editarea_data.languageGroupsMap.end()) {
        itr->second->Add(ea);
    } else {
        LanguageGroup* langgroup = new LanguageGroup(newlang);// free on app closed
        session_editarea_data.languageGroupsMap.emplace(newlang, langgroup);
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
    return session_editarea_data.allEditAreaSet;
}

const std::unordered_set<EditArea*> &GetEditAreasByLanguage(const Language* lang){
    auto itr = session_editarea_data.languageGroupsMap.find(lang);
    if (itr != session_editarea_data.languageGroupsMap.end()) {
        return itr->second->GetEditAreaSet();
    } else {
        return session_editarea_data.EMPTY_EDITAREA_SET;
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
    session_editarea_data.focusedEditArea = editarea;
    const SimpleEvent &event = GetEvent(EDITAREA_FOCUSED_CHANGED);
    for(EventCallback callback : event.GetCallbackSet()){
        ((EditAreaFocusedChangedCallback)callback)(session_editarea_data.focusedEditArea);
    }
}

LanguageGroup* FindLanguageGroup(const Language* lang){
    auto itr = session_editarea_data.languageGroupsMap.find(lang);
    if (itr != session_editarea_data.languageGroupsMap.end()) {
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
        tablayout->Show(*ea);
        session::SetFocusedEditArea(ea);
    }
}

void EditFile(FileData *file){
    EditArea* ea = FindEditAreaByFileData(file);
    if(ea){
        CfTabLayout* tablayout = session::GetFocusedTabLayout();
        if(tablayout){
            tablayout->Show(*ea);
        }
        return;
    }

    ea = new EditArea(file);//freed on EditArea closed(Editarea.Destroy())

    CfTabLayout* tablayout = session::GetFocusedTabLayout();
    if(tablayout){
        tablayout->Show(*ea);
        session::SetFocusedEditArea(ea);
    }
}

void CloseFile(FileData *file){
    // not yet implemented
}

}// namespace session
