from pythonscripts import editarea, extension_loader, language, setting

if __name__ == "__main__":
    language.load_language_settings()
    setting.initialize_setting_panel()
    extension_loader.load_extensions()
