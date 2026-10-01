#include "EditAreaData.h"

#include "SessionEvent.h"
#include "editarea/CompletionPopover.h"
#include "editarea/CompletionTool.h"
#include "editarea/EditArea.h" // for EditArea
#include "layouts/tab/CfTabLayout.h"
#include "layouts/tab/CfTabSwitcher.h"
#include "src/AppContext.h"
#include "src/filemanagement/FileManager.h"
#include "src/languages/LanguageManager.h"
#include "src/session/TabData.h"
#include "toolset/event/Event.h"

#include <cstdio>
#include <memory>
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
    std::unordered_map<const Language*, std::unique_ptr<Language>> languageGroupsMap;
    EditArea* focusedEditArea;
} SessionEditAreaData;

static AppContext* current_context;

SessionEditAreaData session_editarea_data;

/*
 * Callbacks
 */
static void on_editarea_lang_changed(EditArea* ea, const Language* oldlang, const Language* newlang);
static void on_editarea_closed(EditArea* ea);

static void on_editarea_created(EditArea* ea){
    ea->Listen(EditArea::LANG_CHANGED,(EventCallback)on_editarea_lang_changed);
    ea->Listen(EditArea::CLOSED, (EventCallback)on_editarea_closed);

    Language* lang = ea->GetLanguage();
    auto itr = session_editarea_data.languageGroupsMap.find(lang);
    /*/
    if (itr != session_editarea_data.languageGroupsMap.end()) {
        itr->second->Add(ea);
    } else {
        current_context->langManager->CreateLanguage(std::unique_ptr<LanguageInfo> info)
        std::unique_ptr<Language> newlanggroup = std::make_unique<Language>(lang);
        newlanggroup->Add(ea);
        session_editarea_data.languageGroupsMap.emplace(lang, std::move(newlanggroup));
        SimpleEvent& event = session::GetEvent(session::LANGUAGE_USED);
        for (EventCallback callback : event.GetCallbackSet()) {
            ((LanguageUsedCallback)callback)(lang);
        }
    }
    */
}

static void on_editarea_closed(EditArea* ea){
    auto itr = session_editarea_data.languageGroupsMap.find(ea->GetLanguage());
    if (itr != session_editarea_data.languageGroupsMap.end()) {
        itr->second->Remove(ea);
    }
    session_editarea_data.allEditAreaSet.erase(ea);
}

static void on_editarea_lang_changed(EditArea* ea, Language* oldlang, Language* newlang){
    auto itr = session_editarea_data.languageGroupsMap.find(oldlang);
    if (itr != session_editarea_data.languageGroupsMap.end()) {
        itr->second->Remove(ea);
    }
    itr = session_editarea_data.languageGroupsMap.find(newlang);

    if (itr != session_editarea_data.languageGroupsMap.end()) {
        itr->second->Add(ea);
    } else {
        //std::unique_ptr<Language> newlanggroup = std::make_unique<Language>(newlang);// free on app closed
        //newlanggroup->Add(ea);
        //session_editarea_data.languageGroupsMap.emplace(newlang, std::move(newlanggroup));
        SimpleEvent& event = session::GetEvent(session::LANGUAGE_USED);
        for (EventCallback callback : event.GetCallbackSet()) {
            ((LanguageUsedCallback)callback)(newlang);
        }
    }
}

namespace session {

void InitEditAreaData(AppContext& context){
    current_context = &context;
    session::Listen(EDITAREA_CREATED, (EventCallback)on_editarea_created);
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

EditArea* FindEditAreaByPath(const char* absopath){
    FileBranch* result = current_context->fileManager->FindOpenedFileByPath(absopath);
    if (result) {
        return result->GetEditArea();
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
    FileBranch& filebranch = current_context->fileManager->CreateNewFile();

    std::unique_ptr<EditArea> neweditarea = std::make_unique<EditArea>(filebranch, *current_context);
    //session::AddSingleFile(std::move(filebranch));

    EditArea* ea = neweditarea.get();

    CfTabLayout* tablayout = session::GetFocusedTabLayout();
    if(tablayout){
        CfTabSwitcher& switcher = tablayout->Add(std::move(neweditarea));
        tablayout->SwitchTo(switcher);
        session::SetFocusedEditArea(ea);
    }
}

void EditFile(FileBranch& file){
    EditArea* ea = file.GetEditArea();

    if(ea){
        CfTabLayout* tablayout = session::GetFocusedTabLayout();
        if(tablayout){
            tablayout->Show(*ea);
        }
        return;
    }

    std::unique_ptr<EditArea> neweditarea = std::make_unique<EditArea>(file, *current_context);
    ea = neweditarea.get();

    CfTabLayout* tablayout = session::GetFocusedTabLayout();

    if(tablayout){
        CfTabSwitcher& switcher = tablayout->Add(std::move(neweditarea));
        tablayout->SwitchTo(switcher);
        session::SetFocusedEditArea(ea);
    }
}

void CloseFile(FileData *file){
    // not yet implemented
}

}// namespace session
