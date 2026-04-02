#ifndef LSPPOPOVERS_IF_H_
#define LSPPOPOVERS_IF_H_

#include <gtk/gtk.h>
namespace datatypes{
struct Suggestion;
}

class EditArea;

namespace lsppopovers{
    void Construct();

namespace suggestion{
    void SetTargetEditArea(EditArea* target);
    void Show();
    void Hide();
    void Add(datatypes::Suggestion *item);
    void Clear();
}

namespace tip{
    void SetTargetEditArea(EditArea* target);
    void Show(const char* content);
    void Hide();
}

}

#endif
