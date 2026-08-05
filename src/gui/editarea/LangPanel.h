#ifndef LANGPANEL_H_
#define LANGPANEL_H_

#include "components/CfComponent.h"
#include "components/Flyout.h"
#include "datatypes/common.h"
#include "headerbar/Headerbar.h"

#include <functional>
#include <gtk/gtkshortcut.h>
#include <memory>
#include <vector>
#include <gtk/gtk.h>


// forward declaration
class EditArea;

class LangPanelItem : CfComponent{
public:
    LangPanelItem(const Language& lang);
    ~LangPanelItem();

    // called by g_signal
    void Select();

    void SetState(bool selected);// only affect the style. does not set the language of the target
    const Language& GetLanguage() const;
    void OnSelected(std::function<void(LangPanelItem&)> callback);

    GtkWidget* GetBaseWidget() override;

private:
    const Language& m_lang;
    std::function<void(LangPanelItem&)> m_selectedCallback;
    GtkButton* m_button;
    //
    GtkLabel* m_nameLabel;
    GtkLabel* m_idLabel;
    GtkLabel* m_fileExtLabel;
};

/*
 * Language panel will be shared between different editareas
 */

class LangPanel : public Flyout{
public:
    LangPanel(AppUI& appui);

    void AddLanguage(const Language* lang);
    void ChooseFor(EditArea* target);

private:
    int m_nextItemCol = 0;
    int m_nextItemRow = 0;
    AppUI& m_appUI;
    EditArea *m_target;
    GtkScrolledWindow *m_scrolledWindow;
    GtkGrid *m_grid;
    LangPanelItem *m_selectedItem = nullptr;
    std::vector<std::unique_ptr<LangPanelItem>> m_items;
};

void OpenLangPanelForEditArea(EditArea* target);


#endif
