#include "EditArea_if.h"

#include "EditArea.h"
#include "datatypes/common.h"
#include "datatypes/file.h"
#include "src/filemanagement/FileOperation.h"
#include "src/gui/layouts/tab/CfTab_if.h"

#include <unordered_map>
#include <unordered_set>


typedef void (*EditAreaCreatedCallback)(EditArea*);
typedef void (*EditAreaFocusedChangedCallback)(EditArea*);
typedef void (*EditAreaLangChangedCallback)(EditArea*, const char*);

static std::unordered_map<FileData*, EditArea*> open_editareas;
static std::unordered_map<std::string, EditArea*> editarea_list;

//callbacks
static std::unordered_set<EditAreaCreatedCallback> editarea_created_callbacks;
static std::unordered_set<EditAreaFocusedChangedCallback> focused_changed_callbacks;
static std::unordered_set<EditAreaLangChangedCallback> lang_changed_callbacks;


static EditArea* focused_editarea;

namespace editarea {

void SetFocusedEditArea(EditArea* editarea){
    focused_editarea = editarea;
    for(auto callback : focused_changed_callbacks){
        callback(focused_editarea);
    }
}

void EditNewFile(){
    auto neweditarea = new EditArea(filemanagement::CreateVirtualFile());//freed on EditArea closed(Editarea.Destroy())
    SetFocusedEditArea(neweditarea);

    for(auto cb : editarea_created_callbacks){
        cb(neweditarea);
    }

    tablayout::Show(neweditarea);
}

void EditFile(FileData *file){
    auto result = open_editareas.find(file);
    if(result != open_editareas.end()){
        tablayout::Show((CfContent*)result->second);
        return;
    }


    auto neweditarea = new EditArea(file);//freed on EditArea closed(Editarea.Destroy())

    SetFocusedEditArea(neweditarea);
    open_editareas.emplace(file, neweditarea);

    for(auto cb : editarea_created_callbacks){
        cb(neweditarea);
    }

    tablayout::Show(neweditarea);
}

void CloseFile(FileData *file){
    open_editareas.erase(file);
}

void InsertToEditAreaList(EditArea *ea){
    editarea_list.emplace(std::string(ea->GetFilePath()), ea);
}

void RemoveFromEditAreaList(EditArea* ea){
    editarea_list.erase(ea->GetFilePath());
}

EditArea* FindEditArea(const char* absopath){
    auto result = editarea_list.find(std::string(absopath));
    return result == editarea_list.end() ? nullptr : result->second;
}

void ListenEvent(Event event, EventCallback callback){
    switch (event) {
    case EDITAREA_CREATED:
        editarea_created_callbacks.insert((EditAreaCreatedCallback)callback);
        break;
    case EDITAREA_FOCUSED_CHANGED:
        focused_changed_callbacks.insert((EditAreaFocusedChangedCallback)callback);
        break;
    case EDITAREA_LANG_CHANGED:
        lang_changed_callbacks.insert((EditAreaLangChangedCallback)callback);
        break;
    default:
        break;
    }
}

void StopListenEvent(Event event, EventCallback callback){
    switch (event) {
    case EDITAREA_CREATED:
        editarea_created_callbacks.erase((EditAreaCreatedCallback)callback);
        break;
    case EDITAREA_FOCUSED_CHANGED:
        focused_changed_callbacks.erase((EditAreaFocusedChangedCallback)callback);
        break;
    case EDITAREA_LANG_CHANGED:
        lang_changed_callbacks.erase((EditAreaLangChangedCallback)callback);
        break;
    default:
        break;
    }
}

}//namespace editarea
