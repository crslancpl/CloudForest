from CloudForest import EditArea
# language server cannot find CloudForest since
# the module is only available inside CloudForest

EditArea.test()

def NewEACreated(file):
    print(file+" created")
    EditArea.textchanged_addcallback(file,"textchanged")

def textchanged(file):
    text = EditArea.getcontent(file)
    print(text)


EditArea.opennew_addcallback("NewEACreated")
