#include "TickProperty.h"

template <typename T>
TickProperty<T>::TickProperty(void)
{
    Game::GetGame().tickprops.push_back((void*) this);
}

template <typename T>
TickProperty<T>::TickProperty(const T& val)
{
    snap(val); 
    Game::GetGame().tickprops.push_back((void*) this);
}

template <typename T>
TickProperty<T>::~TickProperty(void)
{
    std::vector<void*> *_tickprops;

    _tickprops = &Game::GetGame().tickprops;
    _tickprops->erase(std::remove(_tickprops->begin(), _tickprops->end(), (void*) this), _tickprops->end());
}

template <typename T>
TickProperty<T>& TickProperty<T>::operator=(const T& val)
{
    set(val);
    return *this;
}

template <typename T>
TickProperty<T>::operator T(void) const
{
    return get();
}

template <typename T>
TickProperty<T>& TickProperty<T>::operator+=(const T& val)
{
    cur += val;
    return *this;
}

template <typename T>
TickProperty<T>& TickProperty<T>::operator-=(const T& val)
{
    cur -= val;
    return *this;
}

template <typename T>
TickProperty<T>& TickProperty<T>::operator*=(const T& val)
{
    cur *= val;
    return *this;
}

template <typename T>
TickProperty<T>& TickProperty<T>::operator/=(const T& val)
{
    cur /= val;
    return *this;
}

template <typename T>
void TickProperty<T>::snap(const T& val)
{
    cur = last = lastlast = val;
    initialized = true;
}

template <typename T>
void TickProperty<T>::push(void)
{
    lastlast = last;
    last = cur;
}

template <typename T>
void TickProperty<T>::set(const T& val)
{
    if(!initialized)
        snap(val);
    else
        cur = val;
}

template <typename T>
T TickProperty<T>::get(void) const
{
    return lastlast + (last - lastlast) * Game::GetGame().intertick;
}