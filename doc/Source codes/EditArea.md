Defined in `EditArea.h` and `EditArea.cpp`

![[Screenshot from 2025-08-13 12-50-54.png]]

## EditArea class

```cpp
class EditArea{
public:
    EditArea(GFile *file, FPFileButton* filebut);
    ~EditArea();
    string RandomId;
    FPFileButton* CorreFileButton = nullptr;//Correspoding File Button
    EditAreaHolderTabBut* ParentSwitcher = nullptr;
    EditAreaHolder* ParentHolder = nullptr;
    bool IsSaved = true;

    GFile* EditingFile = nullptr;
    string Lang;

    GtkBuilder *builder;

    int cacheTotalLine = 0;
    char *FileName;
    char *RelePath;
    char *AbsoPath;
    int CursorPos;

    bool IsCurMovedByKey;

    GtkGrid *BaseGrid;
    GtkTextIter *Cursoritr;
    GtkButton *LocationBut;// showing path
    GtkButton *SaveBut;
    GtkButton *ErrorBut;
    GtkLabel *ErrorButLabel;
    GtkButton *OutlineBut;// not working now
    GtkMenuButton *LangBut;// not working now
    GMenu *LangMenu;// not working now
    GtkButton *CursorPosBut;// showing cursor's current line and offset

    GtkTextView *TextView;
    GtkTextBuffer *TextViewBuffer;// contains file content
    GtkTextView *LineNoArea;
    GtkTextBuffer *LineNoAreaBuffer;

    GtkWindow *win;

    /* These iter are shared by multiple functions. ex: draw, delete line */
    GtkTextIter *StartItr;
    GtkTextIter *EndItr;

    void UnrefBuilder();
    void CountLine();
    void CountError();
    void LoadCursorPos();
    void ShowTip(char *Text);// not working
    void ShowSuggestion(const vector<shared_ptr<Suggestion>> &Suggestions);// auto complete
    void ChangeLanguage();//not working
    void ApplyTagByLength(int TextStartPos, int TextLength, char *TagName);
    void ApplyTagByPos(int TextStartPos, int TextEndPos, char *TagName);
    void Destroy();
    void Save();
    void LoadFile(GFile* newfile);
};
```
