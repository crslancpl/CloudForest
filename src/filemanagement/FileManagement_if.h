#ifndef FILEMANAGEMENT_IF_H_
#define FILEMANAGEMENT_IF_H_

#include <gio/gio.h>
#include <gtk/gtk.h>

namespace datatypes{
typedef struct FileData FileData;
}

namespace filemanagement{
    //defined in filemanagement_if
    void Init();
    void OpenFile(GFile* file);
    void CloseFile(GFile *file);

    //defined in
    void ChooseFile();
    void ChooseFolder();

    //defined in FileOperation
    GFile* CreateFile(char* name, GFile* parentfolder);//create file in file panel
    void SaveFile(GFile* file, char* content, void (*savedcallback)(GFile*));

    //defined in FileReader
    void EnumerateFolderChild(GFile* folder, void (*callback)(GFile*, GFileInfo*));
    datatypes::FileData* LoadFileData(GFile* file);
    void ReadFileText(GFile* file, char** output);



    //callbacks
    enum Event : char{
        FILE_EVENT_FILE_CHOOSEN,
        FILE_EVENT_FOLDER_CHOOSEN,
        FILE_EVENT_FILE_SAVED
    };

    void AddCallback(filemanagement::Event event, void (*callback)(GFile*, GFileInfo*));
}


#endif
