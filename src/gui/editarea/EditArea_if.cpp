#include "EditArea_if.h"

#include "EditArea.h"
#include "datatypes/file.h"
#include "src/filemanagement/FileOperation.h"
#include "layouts/tab/CfTab_if.h"
#include "toolset/event/Event.h"

#include <unordered_map>


typedef void (*EditAreaCreatedCallback)(EditArea*);
typedef void (*EditAreaFocusedChangedCallback)(EditArea*);
typedef void (*EditAreaLangChangedCallback)(EditArea*, const char*);

static std::unordered_map<FileData*, EditArea*> open_editareas;
static std::unordered_map<std::string, EditArea*> editarea_list;


static EditArea* focused_editarea;

namespace editarea {

//callbacks
static std::unordered_map<Signal, SimpleEvent> event_map = {
    {EDITAREA_CREATED, SimpleEvent()},
    {EDITAREA_FOCUSED_CHANGED, SimpleEvent()},
    {EDITAREA_LANG_CHANGED, SimpleEvent()}
};

void SetFocusedEditArea(EditArea* editarea){
    focused_editarea = editarea;
    const SimpleEvent &event = event_map.at(EDITAREA_FOCUSED_CHANGED);
    for(EventCallback callback : event.GetCallbackSet()){
        ((EditAreaFocusedChangedCallback)callback)(focused_editarea);
    }
}

void EditNewFile(){
    auto neweditarea = new EditArea(filemanagement::CreateVirtualFile());//freed on EditArea closed(Editarea.Destroy())
    SetFocusedEditArea(neweditarea);

    const SimpleEvent &event = event_map.at(EDITAREA_CREATED);
    for(EventCallback callback : event.GetCallbackSet()){
        ((EditAreaCreatedCallback)callback)(neweditarea);
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

    const SimpleEvent &event = event_map.at(EDITAREA_CREATED);
    for(EventCallback callback : event.GetCallbackSet()){
        ((EditAreaCreatedCallback)callback)(neweditarea);
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

void Listen(Signal signal, EventCallback callback){
    auto itr = event_map.find(signal);
    if(itr != event_map.end()){
        itr->second.Connect(callback);
    }
}

void StopListen(Signal signal, EventCallback callback){
    auto itr = event_map.find(signal);
    if(itr != event_map.end()){
        itr->second.Disconnect(callback);
    }
}

}//namespace editarea
