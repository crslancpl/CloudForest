#ifndef EXTENSIONPAGE_H_
#define EXTENSIONPAGE_H_

#include "SettingPage.h"
#include "datatypes/extension.h"
#include "components/ToggleBlock.h"

#include <memory>
#include <vector>


typedef struct AppContext AppContext ;


class ExtensionBlock : public ToggleBlock {
public:
    ExtensionBlock(Extension* extension, AppContext& context);
    ~ExtensionBlock();

    void SetState(bool state) override;

private:
    AppContext& m_context;
    Extension* m_extension;
};

class ExtensionPage : public SettingPage {
public:
    ExtensionPage(AppContext& context);
    ~ExtensionPage();

    void ShowExtensions();

private:
    AppContext& m_context;

    GtkBox* m_enabledExtensonBox;
    GtkBox* m_disabledExtensonBox;

    std::vector<std::unique_ptr<ExtensionBlock>> m_extensionBlocks;
};

#endif
