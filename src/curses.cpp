#include "curses.hpp"
#include "curses.h"

namespace curses
{

static_assert((chtype)attr::normal == A_NORMAL);
static_assert((chtype)attr::standout == A_STANDOUT);
static_assert((chtype)attr::underline == A_UNDERLINE);
static_assert((chtype)attr::reverse == A_REVERSE);
static_assert((chtype)attr::blink == A_BLINK);
static_assert((chtype)attr::dim == A_DIM);
static_assert((chtype)attr::bold == A_BOLD);
static_assert((chtype)attr::protect == A_PROTECT);
static_assert((chtype)attr::invis == A_INVIS);
static_assert((chtype)attr::altcharset == A_ALTCHARSET);
static_assert((chtype)attr::italic == A_ITALIC);
static_assert((chtype)attr::horizontal == A_HORIZONTAL);
static_assert((chtype)attr::left == A_LEFT);
static_assert((chtype)attr::low == A_LOW);
static_assert((chtype)attr::right == A_RIGHT);
static_assert((chtype)attr::top == A_TOP);
static_assert((chtype)attr::vertical == A_VERTICAL);


namespace
{

template<typename T, typename... TArgs>
inline int curses_call(T callee, const char *errmsg, TArgs... args)
{
    if (int rv = callee(args...); rv != ERR)
    {
        return rv;
    }
    throw curses_error(errmsg);
}

}

window::window()
    : window(0, 0, 0, 0)
{   
}

window::window(int y, int x, int nLines, int nColumns)
    : window(newwin(nLines, nColumns, y, x))
{
}

window::window(WINDOW *impl, bool owned)
    : mImpl(impl, owned ? delwin : [](WINDOW *) {return 0;})
{
}

window::size_t window::size()
{
    int line, col;
    getmaxyx(unsafe_handle(), line, col);
    return { line, col };
}

window::position_t window::position()
{
    int line, col;
    getbegyx(unsafe_handle(), line, col);
    return { line, col };
}

window::position_t window::cursor_position()
{
    int line, col;
    getyx(unsafe_handle(), line, col);
    return { line, col };
}

void window::move(int line, int col)
{
    curses_call(wmove, "failed to move window cursor", 
        unsafe_handle(), line, col);
}

int window::read_key()
{
    auto h = unsafe_handle();
    curses_call(nodelay, "failed to enable input delay",
        h, FALSE);
    
    return curses_call(wgetch, "failed to wait for an input character",
        h);
}

std::tuple<bool, int> window::try_read_key()
{
    auto h = unsafe_handle();
    curses_call(nodelay, "failed to enable input delay",
        h, TRUE);

    auto val = wgetch(h);
    auto av = val != ERR;
    unsigned int mask = -av;
    return { av, val & mask };
}

void window::write(const std::string &data)
{
    curses_call(waddstr, "failed to write a string to the window",
        unsafe_handle(), data.c_str());
}

void window::write(char32_t data, attr fmt)
{
    const chtype character
        = static_cast<chtype>(data) | static_cast<chtype>(fmt);
    
    
}

void window::write(int line, int col, const std::string &data)
{
    move(line, col);
    write(data);
}

void window::refresh()
{
    if (wrefresh(unsafe_handle()) == ERR)
    {
        throw curses_error("Failed to refresh a ncurses window");
    }
}

void window::keypad(bool enable)
{
    ::keypad(unsafe_handle(), enable ? TRUE : FALSE);
}

void window::destroy(WINDOW *win) noexcept
{

}

curses_session::curses_session()
    : mSessionWindow(initscr(), false)
{
    raw();
    noecho();
    curs_set(0);
    mSessionWindow.keypad(true);
}

curses_session::~curses_session()
{
    endwin();
}

}