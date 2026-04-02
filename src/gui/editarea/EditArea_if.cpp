#include "EditArea_if.h"

#include "EditArea.h"
#include "src/gui/layouts/tab/CfTab_if.h"

#include <unordered_map>
#include <unordered_set>


typedef struct{

    EditArea* editarea;
}EditAreaData;

static std::unordered_map<GFile*, EditAreaData*> open_editareas;
static std::unordered_map<const char*, EditAreaData*> file_path_to_editarea_map;

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

void editarea::NewEditArea(){
    auto newdata = new EditAreaData();
    auto neweditarea = new EditArea(nullptr);

    newdata->editarea = neweditarea;

    editarea::SetFocusedEditArea(neweditarea);
    file_path_to_editarea_map.insert({neweditarea->getFilePath(), newdata});

    for(auto cb : new_editarea_callbacks){
        cb(neweditarea);
    }

    tablayout::Show(neweditarea);
}

void editarea::OpenFile(GFile *file){
    auto result = open_editareas.find(file);
    if(result != open_editareas.end()){
        tablayout::Show((CfContent*)result->second->editarea);
        //result->second->container->Show(result->second->editor);
        return;
    }

    auto newdata = new EditAreaData();
    auto neweditarea = new EditArea(file);
    newdata->editarea = neweditarea;

    editarea::SetFocusedEditArea(neweditarea);
    open_editareas.emplace(file, newdata);
    file_path_to_editarea_map.insert({neweditarea->getFilePath(), newdata});

    for(auto cb : new_editarea_callbacks){
        cb(neweditarea);
    }

    tablayout::Show((CfContent*)neweditarea);
}

void editarea::CloseFile(GFile *file){
    open_editareas.erase(file);
}

EditArea* editarea::FindEditArea(const char* absopath){
    for(auto item : file_path_to_editarea_map){
        if(strcmp(item.first, absopath) == 0){
            return item.second->editarea;
        }
    }

    auto itr = file_path_to_editarea_map.find(absopath);
    if(itr != file_path_to_editarea_map.end()) return itr->second->editarea;
    return nullptr;
}

void editarea::AddNewEditAreaCallback(void (*callback)(EditArea*)){
    new_editarea_callbacks.insert(callback);
}

void editarea::RemoveNewEditAreaCallback(void (*callback)(EditArea*)){
    new_editarea_callbacks.erase(callback);
}

void editarea::AddFocusChangedCallback(void (*callback)(EditArea*)){
    focus_changed_callbacks.insert(callback);
}

void editarea::RemoveFocusChangedCallback(void (*callback)(EditArea*)){
    focus_changed_callbacks.erase(callback);
}

void editarea::AddLangChangedCallback(void (*callback)(EditArea*,const char*)){
    lang_changed_callbacks.insert(callback);
}

void editarea::RemoveLangChangedCallback(void (*callback)(EditArea*, const char*)){
    lang_changed_callbacks.erase(callback);
}
