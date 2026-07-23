#include "EditAreaData.h"

#include "SessionEvent.h"
#include "datatypes/common.h" // for Language
#include "editarea/CompletionPopover.h"
#include "editarea/CompletionTool.h"
#include "editarea/EditArea.h" // for EditArea
#include "layouts/tab/CfTabLayout.h"
#include "src/filemanagement/FileCallback.h"
#include "src/filemanagement/FileOperation.h"
#include "src/languages/LanguageGroup.h"
#include "src/session/FileData.h"
#include "src/session/TabData.h"
#include "toolset/event/Event.h"

#include <memory>
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
    std::unordered_map<const Language*, std::unique_ptr<LanguageGroup>> languageGroupsMap;
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
        std::unique_ptr<LanguageGroup> newlanggroup = std::make_unique<LanguageGroup>(lang);
        newlanggroup->Add(ea);
        session_editarea_data.languageGroupsMap.emplace(lang, std::move(newlanggroup));
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
        std::unique_ptr<LanguageGroup> newlanggroup = std::make_unique<LanguageGroup>(newlang);// free on app closed
        newlanggroup->Add(ea);
        session_editarea_data.languageGroupsMap.emplace(newlang, std::move(newlanggroup));
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
        return itr->second.get();
    }
    return nullptr;
}


/*
 * File editing
 */

void EditNewFile(){
    std::unique_ptr<FileData> filedata = filemanager::CreateNewFile();
    FileData* filedataraw = filedata.get();
    session::AddSingleFile(std::move(filedata));

    std::unique_ptr<EditArea> neweditarea = std::make_unique<EditArea>(filedataraw);
    EditArea* ea = neweditarea.get();

    CfTabLayout* tablayout = session::GetFocusedTabLayout();
    if(tablayout){
        tablayout->Add(std::move(neweditarea));
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

    std::unique_ptr<EditArea> neweditarea = std::make_unique<EditArea>(file);
    ea = neweditarea.get();

    CfTabLayout* tablayout = session::GetFocusedTabLayout();
    if(tablayout){
        tablayout->Add(std::move(neweditarea));
        tablayout->Show(*ea);
        session::SetFocusedEditArea(ea);
    }
}

void CloseFile(FileData *file){
    // not yet implemented
}

}// namespace session
