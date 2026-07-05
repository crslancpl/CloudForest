#ifndef LANGPANEL_H_
#define LANGPANEL_H_

#include "components/Flyout.h"
#include "datatypes/common.h"
#include "headerbar/Headerbar.h"

#include <vector>
#include <gtk/gtk.h>


// forward declaration
class EditArea;

/*
 * Language panel will be shared between different editareas
 */

class LangPanel : public Flyout{
public:
    LangPanel(AppUI& appui);

    void AddLanguage(Language* lang);
    void ChooseFor(EditArea* target);

    void LangChoosen(Language* lang);
private:
    AppUI& m_appUI;
    EditArea *m_target;
    std::vector<GtkButton*> m_langBtns;
    GtkBox* m_langBtnBox;
};

void OpenLangPanelForEditArea(EditArea* target);


#endif
