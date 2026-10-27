#include "ExtensionPage.h"

#include "components/ToggleBlock.h"
#include "datatypes/extension.h"
#include "src/AppContext.h"
#include "src/session/ExtensionManager.h"

#include <gtk/gtk.h>
#include <gtk/gtkshortcut.h>
#include <memory>

ExtensionBlock::ExtensionBlock(Extension* extension, AppContext& context): ToggleBlock(extension->name, extension->description, extension->enabled)
    , m_extension(extension)
    , m_context(context)
{
    //
}

ExtensionBlock::~ExtensionBlock(){
    //
}

void ExtensionBlock::SetState(bool state){
    gtk_switch_set_active(m_switchBtn, state);
    gtk_switch_set_state(m_switchBtn, state);

    m_toggleState = state;
    ExtensionManager& manager = m_context.sessionManager->GetExtensionManager();
    if (state) {
        manager.EnableExtension(m_extension);
    }else{
        manager.DisableExtension(m_extension);
    }
}





ExtensionPage::ExtensionPage(AppContext& context) :
    m_context(context)
    {
    this->AddHeader1("Extension");
    this->AddParagraph("Restart the app to take effect");
    this->AddHeader2("Enabled");
    m_enabledExtensonBox = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 10));
    this->AddWidget(GTK_WIDGET(m_enabledExtensonBox));
    this->AddHeader2("Disabled");
    m_disabledExtensonBox = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 10));
    this->AddWidget(GTK_WIDGET(m_disabledExtensonBox));
    this->ShowExtensions();
}

ExtensionPage::~ExtensionPage(){
    //
}

void ExtensionPage::ShowExtensions(){
    ExtensionManager& manager = m_context.sessionManager->GetExtensionManager();

    const std::vector<std::unique_ptr<Extension>>& allExtensions = manager.GetAllExtensions();

    for( const std::unique_ptr<Extension>& extension : allExtensions){
        std::unique_ptr<ExtensionBlock> block = std::make_unique<ExtensionBlock>(extension.get(), m_context);
        if(extension->enabled){
            gtk_box_append(m_enabledExtensonBox, block->GetBaseWidget());
        }else{
            gtk_box_append(m_disabledExtensonBox, block->GetBaseWidget());
        }
        m_extensionBlocks.emplace_back(std::move(block));
    }
}
