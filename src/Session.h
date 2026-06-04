#ifndef SESSION_H_
#define SESSION_H_

#include <gtk/gtk.h>

namespace session{

GtkApplication* GetApplication();
void SetApplication(GtkApplication* app);

}// namespace session


#endif
