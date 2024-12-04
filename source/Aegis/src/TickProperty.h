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
    T cur, last, lastlast;

    TickProperty(void);
    TickProperty(const T& val);
    ~TickProperty(void);

    TickProperty& operator=(const T& val);
    operator T(void) const;

    TickProperty& operator+=(const T& val);
    TickProperty& operator-=(const T& val);
    TickProperty& operator*=(const T& val);
    TickProperty& operator/=(const T& val);


    void snap(const T& val);
    void push(void) override;

private:
    bool initialized = false;

    void set(const T& val);

    T get(void) const;
};

#include "TickProperty.tpp"
