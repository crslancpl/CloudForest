#ifndef LANGPANEL_H_
#define LANGPANEL_H_

/*
 * Forward definition
 */
class EditArea;

/*
 * Language panel will be shared between different editareas
 */

namespace langpanel {
void Construct();
void ChooseLanguage(EditArea* editarea);
}

#endif
