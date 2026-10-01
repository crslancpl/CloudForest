#ifndef CFCORE_H_
#define CFCORE_H_

typedef struct AppContext AppContext ;
class EditArea;

namespace cf{
void Init(AppContext& context);
void LoadTemplate(const char* lang);
void ReadFile(const char* filepath, const char* lang);
void FastHighlight(EditArea* ea);
}


#endif
