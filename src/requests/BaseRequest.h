#ifndef BASEREQUEST_H_
#define BASEREQUEST_H_

enum class Parts{
    CORE,GUI,PYTHON,CLOUDYFOREST,FILEMANAGER
};

class Request{
public:
    const Parts Destination;
    virtual ~Request() = default;
protected:
    Request(Parts dest) : Destination(dest) {}
};

#endif
