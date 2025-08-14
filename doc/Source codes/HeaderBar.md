In `HeaderBar.h` and `HeaderBar.cpp`, `HeaderBar` class is defined.

## HeaderBar class

```cpp
class HeaderBar{
public:
    GtkHeaderBar *HeaderBarWidget;
    GtkMenuButton *FileBut;
    GtkButton *IdeBut;
    // Other buttons is not funtioning yet
    GtkButton *CompileBut;
    GtkSearchEntry *SearchBar;
};
```

HeaderBar class is not a gtk widget. The headerbar you see is `GtkHeaderBar *HeaderBarWidget` under the class.

Only one HeaderBar is needed so call `HeaderBar* LoadHeaderBar(GtkBuilder *builder, GtkApplication *app)` to create a HeaderBar

Currently, only `FileBut` and `IdeBut` are working.

Click the IDE button will call the `IdeButtonClicked()` which shows the [[SettingPanel]]

If you read the UI file, you will see that buttons and search entry are placed inside a GtkBox.

### CSS:
```css
headerbar

#headerbarbuttonbox button

#SearchBox
```

## Function

```c++
HeaderBar* LoadHeaderBar(GtkBuilder *builder);

void IdeButtonClicked(GtkButton *self, void* userdata);

void LoadFileClicked(GSimpleAction *action, GVariant *parameter, gpointer app);

void LoadFolderClicked(GSimpleAction *action, GVariant *parameter, gpointer app);
```

`LoadHeaderBar()` uses the same builder with `NewWindow()` to reduce memory usage. It will load the widget from the UI file and assign it to a new `HeaderBar` object. And then return the pointer.

`IdeButtonClicked` is the callback for IDE button. It opens [[SettingPanel]].

`LoadFileClicked()` is a g_callback function triggered when the "Open File" button under "File" menu is clicked,  `LoadFolderClicked()` is also a callback function.

## GActionEntry

[GActionEntry](https://docs.gtk.org/gio/struct.ActionEntry.html) is used by menu button

```c++
static GActionEntry app_entries[] =
{
  { "file.open", LoadFileClicked, nullptr, nullptr, nullptr },
  { "folder.open", LoadFolderClicked, nullptr, nullptr, nullptr }
};
```