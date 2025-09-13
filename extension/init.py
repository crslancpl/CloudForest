from CloudForest import EditArea
# language server cannot find CloudForest since
# the module is only available inside CloudForest

EditArea.test()

def textchanged(file):
    text = EditArea.getcontent(file)
    print(text)

    #if text == "aaaaaa\naaaaaa\naaaaaa" :
    #    EditArea.highlight(file, 1, 1, 2, "type")
    #    EditArea.highlight(file, 2, 2, 2, "type")
    #    EditArea.highlight(file, 3, 2, 3, "type")


EditArea.textchanged_addcallback("New File","textchanged")
