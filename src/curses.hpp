#pragma once

#include <tuple>
#include <string>
#include <memory>
#include <stdexcept>

struct _win_st;
typedef struct _win_st WINDOW;

namespace curses
{

enum class attr
{
    normal     = 0,
    standout   = 0b0000'0001 << 16,
    underline  = 0b0000'0010 << 16,
    reverse    = 0b0000'0100 << 16,
    blink      = 0b0000'1000 << 16,
    dim        = 0b0001'0000 << 16,
    bold       = 0b0010'0000 << 16,
    altcharset = 0b0100'0000 << 16,
    invis      = 0b1000'0000 << 16,
    protect    = 0b0000'0001 << 24,
    italic     = 0b1000'0000 << 24,

    horizontal = 0b0000'0010 << 24,
    left       = 0b0000'0100 << 24,
    low        = 0b0000'1000 << 24,
    right      = 0b0001'0000 << 24,
    top        = 0b0010'0000 << 24,
    vertical   = 0b0100'0000 << 24,
};

class curses_error
    : std::runtime_error
{
public:
    explicit curses_error(const char *what_arg)
        : runtime_error(what_arg)
    {
    }
    explicit curses_error(const std::string &what_arg)
        : runtime_error(what_arg)
    {
    }
};

class window
{
public:
    // <line, column>
    using position_t = std::tuple<int, int>;
    using size_t = std::tuple<int, int>;

    window();
    window(int line, int col, int nLines, int nColumns);
    explicit window(WINDOW *impl, bool owned = true);

    size_t size();
    position_t position();
    position_t cursor_position();

    void move(int line, int col);

    int read_key();

    void write(char32_t data, attr fmt = attr::normal);
    void write(const std::string &data);

    void write(int line, int col, char32_t data, attr fmt = attr::normal);
    void write(int line, int col, const std::string &data);

    void refresh();

    void keypad(bool enable);

    WINDOW * unsafe_handle()
    {
        return mImpl.get();
    }

private:
    static void destroy(WINDOW *win) noexcept;

    std::shared_ptr<WINDOW> mImpl;
};

class curses_session 
{
public:
    curses_session();
    ~curses_session();

    window & session_window()
    {
        return mSessionWindow;
    }

private:
    window mSessionWindow;
};


}
