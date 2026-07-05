#ifndef HEADERBAR_H_
#define HEADERBAR_H_

#include <gtk/gtk.h>

#include "components/CfComponent.h"

typedef struct AppUI AppUI;

class HeaderBar : public CfComponent{
public:
    HeaderBar(AppUI& appui);

    void IdeButtonClicked();

    GtkWidget * GetBaseWidget() override;
private:
    AppUI& m_appUI;
    GtkHeaderBar *m_headerBarWidget;
    GtkMenuButton *m_fileBut;
    GtkButton *m_compileBut;
    GtkButton *m_ideBut;
    GtkSearchEntry *m_searchBar;
};

#endif
