#ifndef TOGGLEBLOCK_H_
#define TOGGLEBLOCK_H_

#include "CfComponent.h"

#include <gtk/gtk.h>
#include <gtk/gtkshortcut.h>

class ToggleBlock : public CfComponent{
public:
    ToggleBlock(const char* name, const char* description, bool state);
    ~ToggleBlock();

    virtual void SetName(const char* name);
    virtual void SetDescription(const char* description);
    virtual void SetState(bool state);
    virtual void Toggle();

    GtkWidget* GetBaseWidget() override;

protected:
    bool m_toggleState;

    GtkGrid* m_baseGrid;
    GtkLabel* m_nameLabel;
    GtkLabel* m_descriptionLabel;
    GtkSwitch* m_switchBtn;
};

#endif
