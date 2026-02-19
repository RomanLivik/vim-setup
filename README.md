# Vim Fast Configurator
An interactive, high-performance terminal tool written in C++ to automate your Vim environment setup on MacOS and Linux. 

## Warning
The script includes very small part of plugins. If you need to create fully yours Vim setup with very much plugins you can't use the script for this. The number of plugins in the script will increase over time.

## The script had been tested on
* Gentoo linux
* Arch linux
* MacOS 26 Tahoe
* MacOS Sonoma

## Prerequisites
Before running the script, ensure you have the following installed:

* GCC/G++ (supporting C++17)
* curl (used to download the plugin manager)
* Vim (of course)

## Installation
```bash
git clone https://github.com/RomanLivik/vim-setup.git
cd vim-setup
g++ -std=c++17 vim-setup.cpp -o vim-setup
./vim-setup
```
## How to Use
* **Arrows (Up/Down):** Navigate through the menu items.
* **Space:** Toggle a feature or plugin (Select/Deselect).
* **Enter:** Confirm selection and move to the next screen.
* **Wait:** Once the installation starts, stay in the terminal until you see the "ALL DONE" message.

## Plugins Included
* **NERDTree:** Powerful file system explorer.
* **Vim-Airline:** Lean & mean status/tabline for vim that's light as air.
* **Auto-Pairs:** Insert or delete brackets, parens, quotes in pair.
* **Vim-Commentary:** Comment stuff out with ease.

## Themes Included
* Dracula
* Gruvbox
* Nord
* OneDark
