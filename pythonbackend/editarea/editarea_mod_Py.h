#ifndef EDITAREA_MOD_PY_H_
#define EDITAREA_MOD_PY_H_

#include "datatypes/common.h"
#include <Python.h>

class EditArea;
typedef struct py_EditArea py_EditArea;

PyMODINIT_FUNC PyInit_editarea_module();

py_EditArea* find_editarea_py(const EditArea *ea);

void editarea_py_invoke_text_changed(EditArea *ea);
void editarea_py_invoke_lang_changed(EditArea *ea);
void editarea_py_invoke_cursor_moved(EditArea *ea, const ZPosition &pos);
void editarea_py_invoke_completion_requested(EditArea *ea);
void editarea_py_invoke_file_saved(EditArea *ea);
void editarea_py_invoke_filedata_changed(EditArea *ea);

#endif
