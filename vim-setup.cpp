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

const string RED     = "\033[31m";
const string GREEN   = "\033[32m";
const string YELLOW  = "\033[33m";
const string BLUE    = "\033[34m";
const string CYAN    = "\033[36m";
const string RESET   = "\033[0m";
const string BOLD    = "\033[1m";
const string INV     = "\033[7m";

struct Strings {
    string lang_name;
    string title_features;
    string title_themes;
    string title_plugins;
    string title_install;
    string hint_nav;
    string backup_msg;
    string plug_check;
    string plug_installing;
    string plug_wait;
    string done_msg;
    string run_vim_msg;
};

map<string, Strings> locales = {
    {"en", {
        "English", " Feature Configuration ", " Select Color Scheme ", " Select Plugins ", " Installation ",
        "Arrows: Navigate | Space: Toggle | Enter: Next", 
        "[!] Old .vimrc backed up to .vimrc.backup", "[*] Checking plugin manager...",
        "[*] Installing plugins inside Vim...", "Please wait, this may take a minute...",
        "─── ALL DONE! ───", "Run 'vim' to start."
    }},
    {"ru", {
        "Русский", " Настройка функций ", " Выберите цветовую схему ", " Выберите плагины ", " Установка ",
        "Стрелки: Навигация | Пробел: Выбор | Enter: Далее",
        "[!] Старый .vimrc сохранен в .vimrc.backup", "[*] Проверка менеджера плагинов...",
        "[*] Установка плагинов в Vim...", "Пожалуйста, подождите, это займет около минуты...",
        "─── ГОТОВО! ───", "Запустите 'vim' для начала работы."
    }}
};

struct Option { string label_en; string label_ru; string cmd; bool selected; string desc_en; string desc_ru; };
struct Theme  { string name; string repo; string cmd; };
struct Plugin { string name; string repo; string desc_en; string desc_ru; bool selected; };

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
            {"Line Numbers", "Номера строк", "set number", true, "Show line numbers", "Показывать номера строк"},
            {"Relative Numbers", "Относительные номера", "set relativenumber", false, "Helpful for jumps", "Удобно для прыжков по коду"},
            {"Mouse Support", "Поддержка мыши", "set mouse=a", true, "Enable scrolling/clicks", "Разрешить клики и скролл"},
            {"System Clipboard", "Буфер обмена", "set clipboard=unnamedplus", true, "Sync with OS copy/paste", "Общий буфер с системой"},
            {"Smart Indent", "Умные отступы", "set smartindent", true, "Auto indentation", "Автоматические отступы"},
            {"Expand Tabs", "Пробелы вместо табов", "set expandtab\nset tabstop=4\nset shiftwidth=4", true, "4 spaces instead of Tab", "4 пробела вместо Tab"}
        };

        themes = {
            {"Dracula (Dark)", "dracula/vim", "colorscheme dracula"},
            {"Gruvbox (Retro)", "morhetz/gruvbox", "colorscheme gruvbox"},
            {"Nord (Arctic)", "arcticicestudio/nord-vim", "colorscheme nord"},
            {"OneDark (Atom)", "joshdick/onedark.vim", "colorscheme onedark"},
            {"None (Default)", "", ""}
        };

        plugins = {
            {"NERDTree", "preservim/nerdtree", "File explorer", "Проводник файлов (Ctrl+n)", true},
            {"Vim-Airline", "vim-airline/vim-airline", "Status bar", "Красивая статус-строка", true},
            {"Auto-Pairs", "jiangmiao/auto-pairs", "Auto close brackets", "Автозакрытие скобок", true},
            {"Vim-Commentary", "tpope/vim-commentary", "Quick comments (gcc)", "Быстрое комментирование", false}
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
            cout << "│     Select Language      │" << endl;
            cout << "╰──────────────────────────╯" << RESET << endl;
            for(int i=0; i<2; ++i) {
                if(i == cursor) cout << YELLOW << "  ▸ " << INV << " " << locales[l_keys[i]].lang_name << " " << RESET << endl;
                else cout << "    " << locales[l_keys[i]].lang_name << endl;
            }
            int c = getch();
            if(c == 27) { getch(); switch(getch()) {
                case 'A': cursor = 0; break; case 'B': cursor = 1; break;
            }} else if(c == 10) { lang = l_keys[cursor]; s = locales[lang]; break; }
        }
    }

    void run() {
        select_language();
        
        int cursor = 0;
        while(true) {
            clear(); draw_header(s.title_features);
            for(int i=0; i<opts.size(); ++i) {
                if(i == cursor) cout << YELLOW << "  ▸ " << INV; else cout << "    ";
                cout << "[" << (opts[i].selected ? "X" : " ") << "] " << (lang=="ru"?opts[i].label_ru:opts[i].label_en) << RESET;
                if(i == cursor) cout << BLUE << " ← " << (lang=="ru"?opts[i].desc_ru:opts[i].desc_en);
                cout << endl;
            }
            cout << "\n" << YELLOW << s.hint_nav << RESET;
            int c = getch();
            if(c == 27) { getch(); switch(getch()) {
                case 'A': if(cursor>0) cursor--; break; case 'B': if(cursor<opts.size()-1) cursor++; break;
            }} else if(c == ' ') opts[cursor].selected = !opts[cursor].selected;
            else if(c == 10) break;
        }

        cursor = 0;
        while(true) {
            clear(); draw_header(s.title_themes);
            for(int i=0; i<themes.size(); ++i) {
                if(i == cursor) cout << YELLOW << "  ▸ " << INV << " " << themes[i].name << " " << RESET << endl;
                else cout << "    " << themes[i].name << endl;
            }
            int c = getch();
            if(c == 27) { getch(); switch(getch()) {
                case 'A': if(cursor>0) cursor--; break; case 'B': if(cursor<themes.size()-1) cursor++; break;
            }} else if(c == 10) { current_theme = cursor; break; }
        }

        cursor = 0;
        while(true) {
            clear(); draw_header(s.title_plugins);
            for(int i=0; i<plugins.size(); ++i) {
                if(i == cursor) cout << YELLOW << "  ▸ " << INV; else cout << "    ";
                cout << "[" << (plugins[i].selected ? "X" : " ") << "] " << plugins[i].name << RESET;
                if(i == cursor) cout << BLUE << " ← " << (lang=="ru"?plugins[i].desc_ru:plugins[i].desc_en);
                cout << endl;
            }
            int c = getch();
            if(c == 27) { getch(); switch(getch()) {
                case 'A': if(cursor>0) cursor--; break; case 'B': if(cursor<plugins.size()-1) cursor++; break;
            }} else if(c == ' ') plugins[cursor].selected = !plugins[cursor].selected;
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
        v << "\" Генерируемый конфиг\nset nocompatible\nsyntax on" << endl;
        for(auto& o : opts) if(o.selected) v << o.cmd << endl;

        v << "\ncall plug#begin('~/.vim/plugged')" << endl;
        if(!themes[current_theme].repo.empty()) v << "Plug '" << themes[current_theme].repo << "'" << endl;
        for(auto& p : plugins) if(p.selected) v << "Plug '" << p.repo << "'" << endl;
        v << "call plug#end()" << endl;

        if(!themes[current_theme].cmd.empty()) {
            v << "\n\" Тема применяется без ошибок при первом запуске" << endl;
            v << "silent! " << themes[current_theme].cmd << endl;
        }
        
        if(plugins[0].selected) v << "map <C-n> :NERDTreeToggle<CR>" << endl;
        v.close();

        cout << GREEN << s.plug_check << RESET << endl;
        system("curl -fLo ~/.vim/autoload/plug.vim --create-dirs https://raw.githubusercontent.com/junegunn/vim-plug/master/plug.vim > /dev/null 2>&1");
        
        cout << CYAN << s.plug_installing << RESET << endl;
        cout << YELLOW << s.plug_wait << RESET << endl;
        
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