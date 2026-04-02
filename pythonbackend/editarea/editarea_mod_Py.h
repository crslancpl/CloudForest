#ifndef EDITAREA_MOD_PY_H_
#define EDITAREA_MOD_PY_H_

#include <Python.h>

class EditArea;

PyMODINIT_FUNC PyInit_editarea_module();

void editarea_py_invoke_text_changed(EditArea *editarea);
void editarea_py_invoke_cursor_moved(EditArea *editarea, int line, int column);
void editarea_py_invoke_completion_requested(EditArea *editarea);
void editarea_py_invoke_file_saved(EditArea *editarea);
void editarea_py_invoke_filedata_changed(EditArea *editarea);

#endif
