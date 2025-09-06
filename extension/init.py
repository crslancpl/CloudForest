from CloudForest import EditArea

EditArea.test()
def textchanged():
    print("text changed")

EditArea.textchanged_addcallback("New File","textchanged")
