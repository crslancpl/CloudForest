#include "EditArea_if.h"

#include "EditArea.h"
#include "src/gui/layouts/tab/CfTab_if.h"

#include <unordered_map>
#include <unordered_set>


static std::unordered_map<GFile*, EditArea*> open_editareas;
static std::unordered_map<std::string, EditArea*> editarea_list;

//callbacks
static std::unordered_set<void(*)(EditArea*)> new_editarea_callbacks;
static std::unordered_set<void(*)(EditArea*)> focus_changed_callbacks;
static std::unordered_set<void(*)(EditArea*, const char*)> lang_changed_callbacks;


static EditArea* focused_editarea;

void editarea::SetFocusedEditArea(EditArea* editarea){
    focused_editarea = editarea;
    for(auto callback : focus_changed_callbacks){
        callback(focused_editarea);
    }
}

void editarea::CreateEmptyFile(){
    auto neweditarea = new EditArea(nullptr);

    editarea::SetFocusedEditArea(neweditarea);

    for(auto cb : new_editarea_callbacks){
        cb(neweditarea);
    }

    tablayout::Show(neweditarea);
}

void editarea::OpenFile(GFile *file){
    auto result = open_editareas.find(file);
    if(result != open_editareas.end()){
        tablayout::Show((CfContent*)result->second);
        return;
    }


    auto neweditarea = new EditArea(file);

    editarea::SetFocusedEditArea(neweditarea);
    open_editareas.emplace(file, neweditarea);

    for(auto cb : new_editarea_callbacks){
        cb(neweditarea);
    }

    tablayout::Show((CfContent*)neweditarea);
}

void editarea::CloseFile(GFile *file){
    open_editareas.erase(file);
}

void editarea::InsertToEditAreaList(EditArea *ea){
    editarea_list.emplace(ea->GetFilePath(), ea);
}

void editarea::RemoveFromEditAreaList(EditArea* ea){
    editarea_list.erase(ea->GetFilePath());
}

EditArea* editarea::FindEditArea(const char* absopath){
    auto result = editarea_list.find(absopath);
    return result == editarea_list.end() ? nullptr : result->second;
}

void editarea::ListenEvent(Event event, void (*callback)()){
    switch (event) {
    case EDITAREA_CREATED:
        new_editarea_callbacks.insert((void (*)(EditArea*))callback);
        break;
    case EDITAREA_FOCUSED_CHANGED:
        focus_changed_callbacks.insert((void (*)(EditArea*))callback);
        break;
    case EDITAREA_LANG_CHANGED:
        lang_changed_callbacks.insert((void (*)(EditArea*, const char*))callback);
        break;
    default:
        break;
    }
}

void editarea::StopListenEvent(Event event, void (*callback)()){
    switch (event) {
    case EDITAREA_CREATED:
        new_editarea_callbacks.erase((void (*)(EditArea*))callback);
        break;
    case EDITAREA_FOCUSED_CHANGED:
        focus_changed_callbacks.erase((void (*)(EditArea*))callback);
        break;
    case EDITAREA_LANG_CHANGED:
        lang_changed_callbacks.erase((void (*)(EditArea*, const char*))callback);
        break;
    default:
        break;
    }
}
