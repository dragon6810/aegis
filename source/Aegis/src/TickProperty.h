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
    T cur, last, lastlast;
    bool initialized = false;

    void set(const T& val);

    T get(void) const;
};

#include "TickProperty.tpp"
