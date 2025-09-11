#ifndef GLOBAL_H_
#define GLOBAL_H_

#include <gtk/gtk.h>
#include <string>
#include <vector>

enum class Parts{
    CORE,GUI,PYTHON,CLOUDYFOREST,FILEMANAGER
};

namespace request{
class Request{
public:
    const Parts Destination;
    virtual ~Request() = default;

protected:
    Request(Parts dest) : Destination(dest) {}
};

class EAGetText: public Request{
public:
    std::string Filepath;
    EAGetText() : Request(Parts::GUI) {}
};

class EAAddCallBack: public Request{
public:
    enum CallbackType{
        TEXTCHANGED
    };
    CallbackType Type;
    std::string Filepath;
    std::string Funcname;
    EAAddCallBack() : Request(Parts::GUI) {}
};

class EADrawByPos: public Request{
public:
    std::string Filepath;
    unsigned int Startpos, Endpos;
    std::string Tagname;
    EADrawByPos() : Request(Parts::GUI) {}
};

class EADrawByLine: public Request{
public:
    std::string Filepath;
    int Line, Offset, Length;
    std::string Tagname;
    EADrawByLine() : Request(Parts::GUI) {}
};

class PyRunCode: public Request{
public:
    std::string Code;
    PyRunCode() : Request(Parts::PYTHON) {}
};

class PyRunFile: public Request{
public:
    std::string Filepath;
    PyRunFile() : Request(Parts::PYTHON) {}
};

class FileOpenFile: public Request{
public:
    GFile *File;
    void (*Callback) (GFile *file,GFileInfo *fileinfo);
    FileOpenFile() : Request(Parts::FILEMANAGER) {}
};

class FileOpenFolder: public Request{
public:
    GFile *File;
    void (*Callback) (GFile *file,GFileInfo *fileinfo);
    FileOpenFolder() : Request(Parts::FILEMANAGER) {}
};

class FileRead: public Request{
public:
    GFile *File;
    void (*Callback) (GFile *file,GFileInfo *fileinfo);
    FileRead() : Request(Parts::FILEMANAGER) {}
};

class FileSave: public Request{
public:
    GFile *File;
    std::string Content;
    void (*Callback) (GFile *file,GFileInfo *fileinfo);
    FileSave() : Request(Parts::FILEMANAGER) {}
};

class FileEnumerate: public Request{
public:
    GFile *File;
    void (*Callback) (GFile *file,GFileInfo *fileinfo);
    FileEnumerate() : Request(Parts::FILEMANAGER) {}
};

class CFLoadTemplate: public Request{
public:
    std::string Language;
    void (*Callback)();
    CFLoadTemplate() : Request(Parts::CLOUDYFOREST) {}
};

class CFHighlight: public Request{
public:
    std::string Filepath;
    std::string Language;
    CFHighlight() : Request(Parts::CLOUDYFOREST) {}
};

class CFReadFile: public Request{
public:
    std::string Filepath;
    std::string Language;
    CFReadFile() : Request(Parts::CLOUDYFOREST) {}
};

}

namespace result{
class Result{
//nothing
};

class GetText: public Result{
public:
    const std::string* Text;
};

class GetAllEditAreaPath: public Result{
public:
    const std::vector<std::string>* Text;
};

}

namespace global{
extern GtkApplication *GtkApp;
}


#endif
