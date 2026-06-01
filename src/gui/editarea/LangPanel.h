#ifndef LANGPANEL_H_
#define LANGPANEL_H_

#include "src/gui/components/Flyout.h"
#include "datatypes/common.h"

#include <vector>
#include <gtk/gtk.h>


// forward declaration
class EditArea;

/*
 * Language panel will be shared between different editareas
 */

class LangPanel : public Flyout{
public:
    LangPanel();

    void AddLanguage(Language* lang);
    void ChooseFor(EditArea* target);

    void LangChoosen(Language* lang);
private:
    EditArea *m_target;
    std::vector<GtkButton*> m_langBtns;
    GtkBox* m_langBtnBox;
};

void OpenLangPanelForEditArea(EditArea* target);


#endif
