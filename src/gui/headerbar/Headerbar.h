#ifndef HEADERBAR_H_
#define HEADERBAR_H_

#include <gtk/gtk.h>

#include "../components/CfComponent.h"

class HeaderBar : public CfComponent{
public:
    HeaderBar();
    GtkWidget * GetBaseWidget() override;
private:
    GtkHeaderBar *m_headerBarWidget;
    GtkMenuButton *m_fileBut;
    GtkButton *m_compileBut;
    GtkButton *m_ideBut;
    GtkSearchEntry *m_searchBar;
};

#endif
