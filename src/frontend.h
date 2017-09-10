#pragma once

#include <string>

#include <cursespp/cursespp.hpp>
#include "vector3.h"
#include "drv10975.hpp"
#include "console_parser.hpp"

class box
{
public:
    box(curspp::window target, curspp::vector2i pos, curspp::vector2i size)
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

    void value(std::string v)
    {
        mValue = v;
    }

    void render(bool full)
    {
        if (full)
        {
            auto lpos = mPosition + curspp::vector2i{mLabelSpace - static_cast<int>(mLabel.size()), 0};
            mTarget.write(lpos, mLabel);
        }
        auto vpos = mPosition + curspp::vector2i{mLabelSpace + mValueSpace - static_cast<int>(mValue.size()), 0};
        for (curspp::vector2i i = mPosition + curspp::vector2i{mLabelSpace, 0};
             i.x < vpos.x; ++i.x)
        {
            mTarget.write(i, U' ');
        }
        mTarget.write(vpos, mValue);
    }

private:
    curspp::window &mTarget;
    std::string mLabel;
    std::string mValue;
    curspp::vector2i mPosition;
    int mLabelSpace;
    int mValueSpace;
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
    frontend(curspp::window &target);

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

    page1 mPage1;
    driver_stats_page mDriverPage;
    std::function<void()> mRenderFunc;
    page mDisplayedPage = page::p1;

    std::string mCurrentCmdContent;

    //text_box 
};
