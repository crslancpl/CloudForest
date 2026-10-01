#ifndef HEADERBAR_H_
#define HEADERBAR_H_

#include <gtk/gtk.h>

#include "components/CfComponent.h"

typedef struct AppContext AppContext;

class HeaderBar : public CfComponent{
public:
    HeaderBar(AppContext& appui);
    ~HeaderBar();

    GtkWidget * GetBaseWidget() override;

    void OnLoadFileClicked();
    void OnLoadFolderClicked();
    void OnNewFileClicked();
    void OnIdeButtonClicked();

private:
    AppContext& m_context;
    GtkHeaderBar *m_headerBarWidget;
    GtkMenuButton *m_fileBut;
    GtkButton *m_compileBut;
    GtkButton *m_ideBut;
    GtkSearchEntry *m_searchBar;
};

#endif
