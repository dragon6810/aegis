#pragma once

#include "Game.h"

class TickPropertyBase
{
public:
    virtual void push(void) = 0;
};

template <typename T>
class TickProperty : public TickPropertyBase
{
public:
    TickProperty(void) { Game::GetGame().tickprops.push_back((void*) this); }
    TickProperty(const T& val) { snap(val); Game::GetGame().tickprops.push_back((void*) this); }
    ~TickProperty(void)
    {
        std::vector<void*> *_tickprops;

        _tickprops = &Game::GetGame().tickprops;
        _tickprops->erase(std::remove(_tickprops->begin(), _tickprops->end(), (void*) this), _tickprops->end());
    }

    TickProperty& operator=(const T& val) { set(val); return *this; }
    operator T() const { return get(); }

    TickProperty& operator+=(const T& val) { cur += val; return *this; }
    TickProperty& operator-=(const T& val) { cur -= val; return *this; }
    TickProperty& operator*=(const T& val) { cur *= val; return *this; }
    TickProperty& operator/=(const T& val) { cur /= val; return *this; }


    void snap(const T& val) { cur = last = lastlast = val; initialized = true; }
    void push(void) override { lastlast = last; last = cur; }

private:
    T cur, last, lastlast;
    bool initialized = false;

    void set(const T& val)
    {
        if(!initialized)
            snap(val);
        else
            cur = val;
    }

    T get(void) const { return lastlast + (last - lastlast) * Game::GetGame().intertick; };
};
