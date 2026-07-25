#ifndef CFLAYOUT_H_
#define CFLAYOUT_H_

#include <gtk/gtk.h>
#include <gtk/gtkshortcut.h>
#include <memory>
#include <vector>

#include "components/CfContent.h"

//forward declare
class CfLayout;



typedef struct{
    /*
     * This is for GetNeighbors functon
     */
    int prevPos, nextPos;
    CfContent *prevContent, *nextContent;
}LayoutNeighbor;

/*
 * This shouldn't be used outside CFLayout
 */
class CfSeparator : public CfContent {
public:
    CfSeparator(CfLayout& parent, GtkOrientation orientation);

    CfLayout &GetParentLayout();

    GtkWidget* GetBaseWidget() override;

private:
    CfLayout &m_parentLayout;
    GtkSeparator *m_separator;
};

/*
 * Usage
 *
 * constructing: layoutobject.Init(GTK_ORIENTATION_...);
 * add new child: layoutobject.InsertChild(GTK_WIDGET(widget));
 * remove child: layoutobject.ReomoveChild(GTK_WIDGET(widget));
 */
class CfLayout : public CfContent{
public:
    CfLayout(GtkOrientation orientation);
    ~CfLayout();

    void Insert(std::unique_ptr<CfContent> child);
    void Remove(CfContent *child);
    LayoutNeighbor GetNeighbors(CfContent *child);
    GtkOrientation GetOrientation();

private:
    std::vector<std::unique_ptr<CfContent>> m_childContents;// from left to right or from top to bottom
    std::vector<std::unique_ptr<CfSeparator>> m_separators;
    GtkOrientation m_orientation;
    GtkBox *m_baseBox;
};

#endif
