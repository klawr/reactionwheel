#pragma once

#include <chrono>
#include <string>

#include <cursespp/cursespp.hpp>
#include <ucmd-parser/command_tree.hpp>
#include "vector3.h"
#include "drv10975.hpp"
#include "thread_bus.h"

class box
{
public:
    box(curspp::window &target, curspp::vector2i pos, curspp::vector2i size)
        : mTarget(target)
        , mPosition(pos)
        , mSize(size)
    {
    }

    void render()
    {
        constexpr char32_t symbols[] = {
            U'╔', U'╗', U'╚', U'╝', U'═', U'║'
        };

        curspp::cposition_scope rpos_scope { mTarget, mPosition };

        mTarget.write(symbols[0]);
        for (int i = 1; i < mSize.x - 1; ++i)
        {
            mTarget.write(symbols[4]);
        }
        mTarget.write(symbols[1]);

        for (int i = 1; i < mSize.y - 1; ++i)
        {
            auto p = mPosition + curspp::vector2i{0, i};
            mTarget.write(p, symbols[5]);
            p += curspp::vector2i{mSize.x-1, 0};
            mTarget.write(p, symbols[5]);
        }

        mTarget.move_cursor(mPosition + curspp::vector2i{0, mSize.y - 1});
        mTarget.write(symbols[2]);
        for (int i = 1; i < mSize.x - 1; ++i)
        {
            mTarget.write(symbols[4]);
        }
        mTarget.write(symbols[3]);
    }

private:
    curspp::window &mTarget;
    curspp::vector2i mPosition;
    curspp::vector2i mSize;
};

class text_box
{
public:
    text_box(curspp::window &target, std::string label, curspp::vector2i pos, int labelSpace, int valueSpace)
        : mTarget(target)
        , mLabel(label)
        , mValue()
        , mPosition(pos)
        , mLabelSpace(labelSpace)
        , mValueSpace(valueSpace)
    {
    }

    std::string_view value()
    {
        return mValue;
    }
    void value(std::string_view v)
    {
        if (v.size() >= static_cast<size_t>(mValueSpace))
        {
            mValue.replace(0, mValue.size(), v.substr(0, mValueSpace));
        }
        else
        {
            mValue.clear();
            mValue.insert(0, mValueSpace - v.size(), ' ');
            mValue.append(v);
        }
    }

    void render(bool full)
    {
        if (full)
        {
            auto lpos = mPosition + curspp::vector2i{mLabelSpace - static_cast<int>(mLabel.size()), 0};
            mTarget.write(lpos, mLabel);
        }
        auto vpos = mPosition;
        vpos.x += mLabelSpace;
        mTarget.write(vpos, mValue, curspp::attr::normal, curspp::color_pair{0});
    }

private:
    curspp::window &mTarget;
    std::string mLabel;
    std::string mValue;
    curspp::vector2i mPosition;
    int mLabelSpace;
    int mValueSpace;
};

class input_field
{
public:
    input_field(curspp::window &target, curspp::vector2i pos, int size)
        : mTarget(target)
        , mContent()
        , mPosition(pos)
        , mSize(size)
        , mContentOffset(0)
    {
    }

    std::string_view content_view() const
    {
        return mContent;
    }

    void append(char c)
    {
        mContent.push_back(c);
        if (mContent.size() >= static_cast<size_t>(mSize))
        {
            ++mContentOffset;
        }
    }

    void pop_back()
    {
        if (!mContent.empty())
        {
            if (mContent.size() >= static_cast<size_t>(mSize))
            {
                --mContentOffset;
            }
            mContent.pop_back();
        }
    }

    void clear()
    {
        mContent.clear();
        mContentOffset = 0;
    }

    void render()
    {
        auto drawRange = static_cast<std::string_view>(mContent)
            .substr(mContentOffset);
        mTarget.write(mPosition, drawRange);
        if (mContent.size() < static_cast<size_t>(mSize - 1))
        {
            for (auto i = curspp::vector2i{static_cast<int>(mContent.size()), 0};
                i.x < mSize - 1; ++i.x)
            {
                mTarget.write(mPosition + i, U' ');
            }
        }
    }

private:
    curspp::window &mTarget;
    std::string mContent;
    curspp::vector2i mPosition;
    int mSize;
    int mContentOffset;
    bool mNeedsRedraw;
};

class vector_display
{
    static constexpr curspp::vector2i offset_x {2, 1};
    static constexpr curspp::vector2i offset_y {12, 1};
    static constexpr curspp::vector2i offset_z {22, 1};
public:
    vector_display(curspp::window &target, curspp::vector2i pos, std::string label)
        : mTarget(target)
        , mLabel(label)
        , mPos(pos)
        , mX(target, "x:", pos + offset_x, 2, 8)
        , mY(target, "y:", pos + offset_y, 2, 8)
        , mZ(target, "z:", pos + offset_z, 2, 8)
    {
    }

    void value(vector3i v)
    {
        mX.value(std::to_string(v.x));
        mY.value(std::to_string(v.y));
        mZ.value(std::to_string(v.z));
    }
    void render(bool full)
    {
        if (full)
        {
            mTarget.write(mPos, mLabel);
        }
        mX.render(full);
        mY.render(full);
        mZ.render(full);
    }

private:
    curspp::window &mTarget;
    std::string mLabel;
    curspp::vector2i mPos;
    text_box mX;
    text_box mY;
    text_box mZ;
};

class frontend
{
    enum class page
    {
        p1 = 0,
        stats,
    };
public:
    frontend(curspp::window &target, reactionwheel::message_port &motorPort);

    bool live();

    void update_accelleration(vector3i raw);
    void update_gyro(vector3i raw);
    void update_queue_load(int load);

    void swallow(reactionwheel::driver_status_message &msg);

private:
    void update_stopped();

    class page1
    {
    public:
        curspp::window &mWnd;

        page1(curspp::window &wnd);
        void render(bool full);

        vector_display mAccelDp;
        vector_display mGyroDp;
        text_box mAppStopped;

        text_box mAngle;
    };

    class driver_stats_page
    {
    public:
        curspp::window &mWnd;

        driver_stats_page(curspp::window &wnd);
        void render(bool full);

        text_box mOverHeat;
        text_box mOverCurrent;
        text_box mMotorLock;

        text_box mMotorSpeed;
        text_box mMotorPeriod;
        text_box mMotorKate;
        text_box mSupplyVoltage;
        text_box mSpeedCmd;
        text_box mSpeedCmdBuffer;

        text_box mFaultCurrentLimit;
        text_box mFaultAbnormalSpeed;
        text_box mFaultAbnormalKate;
        text_box mFaultNoMotor;
        text_box mFaultStuckOpenLoop;
        text_box mFaultStuckClosedLoop;

        text_box mAppStopped;
    };

    curspp::window &mWnd;
    reactionwheel::message_port &mMotorPort;

    ucmdp::command_tree mEval;

    std::chrono::steady_clock::time_point mLastUpdate;

    page1 mPage1;
    driver_stats_page mDriverPage;
    std::function<void()> mRenderFunc;
    page mDisplayedPage = page::p1;
    input_field mCmdInput;

    //text_box 
};
