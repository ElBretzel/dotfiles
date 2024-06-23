-- bootstrap lazy.nvim, LazyVim and your plugins
require("config.lazy")
vim.opt.mouse = ""
vim.o.cc = "80"
vim.o.expandtab = true
vim.o.shiftwidth = 4
vim.o.softtabstop = 4
vim.o.backspace = "eol,start,indent"
vim.o.list = true
vim.o.listchars = "eol:⏎,tab:- ,space:."
vim.o.cindent = true
vim.o.textwidth = 120
vim.o.autoindent = true
vim.o.wrapmargin = 20
vim.o.linebreak = true
vim.o.undofile = true
vim.o.termguicolors = true

vim.o.updatetime = 250
vim.o.timeoutlen = 300
vim.o.completeopt = "menuone,noselect"

vim.cmd.colorscheme("tokyonight")
