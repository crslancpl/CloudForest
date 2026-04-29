#ifndef LANGPANEL_H_
#define LANGPANEL_H_

// forward declaration
class EditArea;

/*
 * Language panel will be shared between different editareas
 */

namespace langpanel {
void Construct();
void ChooseLanguage(EditArea* editarea);
}

#endif
