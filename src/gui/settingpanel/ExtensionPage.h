#ifndef EXTENSIONPAGE_H_
#define EXTENSIONPAGE_H_

#include "SettingPage.h"
#include "datatypes/extension.h"
#include "components/ToggleBlock.h"
#include <memory>
#include <vector>

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
    ~ExtensionPage();

    void ShowExtensions();

private:
    GtkBox* m_enabledExtensonBox;
    GtkBox* m_disabledExtensonBox;

    std::vector<std::unique_ptr<ExtensionBlock>> m_extensionBlocks;
};

#endif
