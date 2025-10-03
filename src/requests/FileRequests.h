#ifndef FILEREQUEST_H_
#define FILEREQUEST_H_

#include "BaseRequest.h"

#include <gio/gio.h>
#include <string>


class FileRequest;

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

struct FileRead: public Request{
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

#endif
