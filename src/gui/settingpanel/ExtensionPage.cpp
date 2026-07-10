#include "ExtensionPage.h"

#include "components/ToggleBlock.h"
#include "datatypes/extension.h"
#include "src/Setting.h"

#include <gtk/gtk.h>
#include <gtk/gtkshortcut.h>
#include <memory>

ExtensionBlock::ExtensionBlock(Extension* extension): ToggleBlock(extension->name, extension->description, extension->enabled)
    , m_extension(extension)
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

    if (state) {
        setting::EnableExtension(m_extension);
    }else{
        setting::DisableExtension(m_extension);
    }
}





ExtensionPage::ExtensionPage(){
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
    const std::vector<std::unique_ptr<Extension>>& allExtensions = setting::GetAllExtensions();
    for( const std::unique_ptr<Extension>& extension : allExtensions){
        std::unique_ptr<ExtensionBlock> block = std::make_unique<ExtensionBlock>(extension.get());
        if(extension->enabled){
            gtk_box_append(m_enabledExtensonBox, block->GetBaseWidget());
        }else{
            gtk_box_append(m_disabledExtensonBox, block->GetBaseWidget());
        }
        m_extensionBlocks.emplace_back(std::move(block));
    }
}
