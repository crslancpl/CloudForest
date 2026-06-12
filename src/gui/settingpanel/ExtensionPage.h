#ifndef EXTENSIONPAGE_H_
#define EXTENSIONPAGE_H_

#include "SettingPage.h"
#include "datatypes/extension.h"
#include "components/ToggleBlock.h"

class ExtensionBlock : public ToggleBlock {
public:
    ExtensionBlock(Extension* extension);
    ~ExtensionBlock();

    void SetState(bool state) override;

private:
    Extension* m_extension;
};

class ExtensionPage : public SettingPage {
public:
    ExtensionPage();

    void ShowExtensions();

private:
    GtkBox* m_enabledExtensonBox;
    GtkBox* m_disabledExtensonBox;
};

#endif
