#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cstdlib>
#include <termios.h>
#include <unistd.h>
#include <filesystem>
#include <map>

namespace fs = std::filesystem;
using namespace std;

// Colors
const string RED     = "\033[31m";
const string GREEN   = "\033[32m";
const string YELLOW  = "\033[33m";
const string BLUE    = "\033[34m";
const string CYAN    = "\033[36m";
const string RESET   = "\033[0m";
const string BOLD    = "\033[1m";
const string INV     = "\033[7m";

// Localization
struct Strings {
    string lang_name, title_features, title_themes, title_plugins, title_install, hint_nav, backup_msg, plug_check, plug_installing, plug_wait, done_msg, run_vim_msg;
};

map<string, Strings> locales = {
    {"en", {
        "English", " Feature Configuration ", " Select Color Scheme ", " Select Plugins ", " Installation ",
        "Arrows: Navigate | Space: Toggle | Enter: Next", 
        "[!] Old .vimrc backed up to .backup", "[*] Checking plugin manager...",
        "[*] Installing plugins inside Vim...", "Please wait, this may take a minute...",
        "─── ALL DONE! ───", "Run 'vim' to start."
    }},
    {"ru", {
        "Русский", " Настройка функций ", " Выберите цветовую схему ", " Выберите плагины ", " Установка ",
        "Стрелки: Навигация | Пробел: Выбор | Enter: Далее",
        "[!] Старый .vimrc сохранен в .backup", "[*] Проверка менеджера плагинов...",
        "[*] Установка плагинов в Vim...", "Пожалуйста, подождите, это займет около минуты...",
        "─── ГОТОВО! ───", "Запустите 'vim' для начала работы."
    }}
};

// Structure
struct Option { 
    string label_en, label_ru, cmd; 
    bool selected; 
    string desc_en, desc_ru; 
};

struct Theme { 
    string name, repo, cmd; 
};

struct Plugin { 
    string name, repo, desc_en, desc_ru; 
    bool selected; 
};

// System func
int getch() {
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

void clear() { cout << "\033[H\033[2J"; }

class Configurator {
    Strings s;
    string lang = "en";
    vector<Option> opts;
    vector<Theme> themes;
    vector<Plugin> plugins;
    int current_theme = 0;
    string home_dir;

public:
    Configurator() {
        const char* h = getenv("HOME");
        home_dir = h ? string(h) : ".";
        
        opts = {
            {"Native Copy/Paste", "Привычное Копирование", "clipboard_logic", true, "Ctrl+C/V for Laptop, Cmd+C/V for Mac", "Ctrl+C/V для ноутбуков, Cmd+C/V для Mac"},
            {"Line Numbers", "Номера строк", "set number", true, "Show line numbers on the left", "Показывать номера строк слева"},
            {"Mouse Support", "Поддержка мыши", "set mouse=a", true, "Allow scrolling and clicking", "Разрешить клики и скролл"},
            {"Smart Indent", "Умные отступы", "set smartindent", true, "Auto-indent for code blocks", "Авто-отступы для кода"},
            {"Auto-Tab (4 spaces)", "Авто-таб (4 пробела)", "set expandtab\nset tabstop=4\nset shiftwidth=4", true, "Use spaces instead of tabs", "Использовать пробелы вместо табуляции"}
        };

        themes = {
            {"Dracula (Dark)", "dracula/vim", "colorscheme dracula"},
            {"Gruvbox (Retro)", "morhetz/gruvbox", "colorscheme gruvbox"},
            {"Nord (Arctic)", "arcticicestudio/nord-vim", "colorscheme nord"},
            {"None", "", ""}
        };

        plugins = {
            {"NERDTree", "preservim/nerdtree", "File explorer (Ctrl+n)", "Проводник файлов (Ctrl+n)", true},
            {"Vim-Airline", "vim-airline/vim-airline", "Beautiful status bar", "Красивая статус-строка", true},
            {"Auto-Pairs", "jiangmiao/auto-pairs", "Auto-close brackets", "Авто-закрытие скобок", true},
            {"Vim-Commentary", "tpope/vim-commentary", "Quick comments (gcc)", "Быстрое комментирование", true}
        };
    }

    void draw_header(string title) {
        cout << RED << "╭──────────────────────────────────────────────────╮" << endl;
        cout << "│ " << BOLD << CYAN << title << RESET << RED << endl;
        cout << "╰──────────────────────────────────────────────────╯" << RESET << endl;
    }

    void select_language() {
        int cursor = 0;
        vector<string> l_keys = {"en", "ru"};
        while(true) {
            clear();
            cout << RED << "╭──────────────────────────╮" << endl;
            cout << "│     Vim Config Wizard    │" << endl;
            cout << "╰──────────────────────────╯" << RESET << endl;
            for(int i=0; i<2; ++i) {
                if(i == cursor) cout << YELLOW << "  ▸ " << INV << " " << locales[l_keys[i]].lang_name << " " << RESET << endl;
                else cout << "    " << locales[l_keys[i]].lang_name << endl;
            }
            int c = getch();
            if(c == 27) { getch(); switch(getch()) { case 'A': cursor = 0; break; case 'B': cursor = 1; break; } }
            else if(c == 10) { lang = l_keys[cursor]; s = locales[lang]; break; }
        }
    }

    void run() {
        select_language();
        
        int cursor = 0;
        while(true) {
            clear(); draw_header(s.title_features);
            for(int i=0; i<(int)opts.size(); ++i) {
                if(i == cursor) cout << YELLOW << "  ▸ " << INV; else cout << "    ";
                cout << "[" << (opts[i].selected ? "X" : " ") << "] " << (lang=="ru"?opts[i].label_ru:opts[i].label_en) << RESET;
                if(i == cursor) cout << BLUE << " ← " << (lang=="ru"?opts[i].desc_ru:opts[i].desc_en); // ВОТ ОПИСАНИЕ
                cout << endl;
            }
            cout << "\n" << YELLOW << s.hint_nav << RESET;
            int c = getch();
            if(c == 27) { getch(); switch(getch()) { case 'A': if(cursor>0) cursor--; break; case 'B': if(cursor<(int)opts.size()-1) cursor++; break; } }
            else if(c == ' ') opts[cursor].selected = !opts[cursor].selected;
            else if(c == 10) break;
        }

        // Themes
        cursor = 0;
        while(true) {
            clear(); draw_header(s.title_themes);
            for(int i=0; i<(int)themes.size(); ++i) {
                if(i == cursor) cout << YELLOW << "  ▸ " << INV << " " << themes[i].name << " " << RESET << endl;
                else cout << "    " << themes[i].name << endl;
            }
            int c = getch();
            if(c == 27) { getch(); switch(getch()) { case 'A': if(cursor>0) cursor--; break; case 'B': if(cursor<(int)themes.size()-1) cursor++; break; } }
            else if(c == 10) { current_theme = cursor; break; }
        }

        // Plugins
        cursor = 0;
        while(true) {
            clear(); draw_header(s.title_plugins);
            for(int i=0; i<(int)plugins.size(); ++i) {
                if(i == cursor) cout << YELLOW << "  ▸ " << INV; else cout << "    ";
                cout << "[" << (plugins[i].selected ? "X" : " ") << "] " << plugins[i].name << RESET;
                if(i == cursor) cout << BLUE << " ← " << (lang=="ru"?plugins[i].desc_ru:plugins[i].desc_en); // ВОТ ОПИСАНИЕ
                cout << endl;
            }
            int c = getch();
            if(c == 27) { getch(); switch(getch()) { case 'A': if(cursor>0) cursor--; break; case 'B': if(cursor<(int)plugins.size()-1) cursor++; break; } }
            else if(c == ' ') plugins[cursor].selected = !plugins[cursor].selected;
            else if(c == 10) break;
        }

        install();
    }

    void install() {
        clear(); draw_header(s.title_install);
        string v_path = home_dir + "/.vimrc";

        if (fs::exists(v_path)) {
            fs::copy_file(v_path, v_path + ".backup", fs::copy_options::overwrite_existing);
            cout << YELLOW << s.backup_msg << RESET << endl;
        }

        ofstream v(v_path);
        v << "set nocompatible\nsyntax on" << endl;

        bool native_clipboard = false;
        for(auto& o : opts) {
            if(o.cmd == "clipboard_logic" && o.selected) native_clipboard = true;
            else if(o.selected) v << o.cmd << endl;
        }

        if(native_clipboard) {
            v << "\n\" --- Copy/Paste Logic ---" << endl;
            #ifdef __APPLE__
                v << "set clipboard=unnamed" << endl;
                v << "vnoremap <C-c> \"+y" << endl;
            #else
                v << "set clipboard=unnamedplus" << endl;
                v << "vnoremap <C-c> \"+y" << endl;
                v << "nnoremap <C-v> \"+p" << endl;
                v << "inoremap <C-v> <C-r>+" << endl;
                v << "vnoremap <C-v> \"+p" << endl;
            #endif
        }

        v << "\ncall plug#begin('~/.vim/plugged')" << endl;
        if(!themes[current_theme].repo.empty()) v << "Plug '" << themes[current_theme].repo << "'" << endl;
        for(auto& p : plugins) if(p.selected) v << "Plug '" << p.repo << "'" << endl;
        v << "call plug#end()" << endl;

        if(!themes[current_theme].cmd.empty()) v << "silent! " << themes[current_theme].cmd << endl;
        if(plugins[0].selected) v << "map <C-n> :NERDTreeToggle<CR>" << endl;

        v.close();

        cout << GREEN << s.plug_check << RESET << endl;
        system("curl -fLo ~/.vim/autoload/plug.vim --create-dirs https://raw.githubusercontent.com/junegunn/vim-plug/master/plug.vim > /dev/null 2>&1");
        
        cout << CYAN << s.plug_installing << RESET << endl;
        string quiet_cmd = "vim -i NONE -u " + v_path + " +PlugInstall +qall > /dev/null 2>&1";
        system(quiet_cmd.c_str());

        cout << endl << BOLD << GREEN << s.done_msg << RESET << endl;
        cout << s.run_vim_msg << endl;
    }
};

int main() {
    Configurator c;
    c.run();
    return 0;
}
