import sys

from cloudforest import lsp

client: lsp.LspClient = lsp.add("clangd", "cpp")
