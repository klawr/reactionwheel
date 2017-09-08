// Copyright 2016-2017 Henrik Steffen Gaßmann
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//  http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// Limitations under the License.
// /////////////////////////////////////////////////////////////////////////////
#pragma once

#include<type_traits>

template< class T >
std::false_type allow_enum_bitset(T&&);

template< class E >
class enum_bitset
{
public:
    static_assert(decltype(allow_enum_bitset(std::declval<E>()))::value);

    using enum_type = E;
    using value_type = typename std::underlying_type<enum_type>::type;
    using type = enum_bitset<enum_type>;

    constexpr enum_bitset() = default;
    constexpr enum_bitset(enum_type value)
        : mValue(static_cast<value_type>(value))
    {
    }
    constexpr explicit enum_bitset(value_type value)
        : mValue(value)
    {
    }

    constexpr explicit operator bool() const
    {
        return mValue != value_type();
    }

    constexpr type operator==(type rhs) const
    {
        return mValue == rhs.mValue;
    }
    constexpr type operator!=(type rhs) const
    {
        return mValue != rhs.mValue;
    }

    constexpr type operator&(type rhs) const
    {
        return type(mValue & rhs.mValue);
    }
    constexpr type & operator&=(type rhs)
    {
        mValue &= rhs.mValue;
        return *this;
    }

    constexpr type operator|(type rhs) const
    {
        return type(mValue | rhs.mValue);
    }
    constexpr type & operator|=(type rhs)
    {
        mValue |= rhs.mValue;
        return *this;
    }

    constexpr type operator^(type rhs) const
    {
        return type(mValue ^ rhs.mValue);
    }
    constexpr type & operator^=(type rhs)
    {
        mValue ^= rhs.mValue;
        return *this;
    }

    constexpr type operator~() const
    {
        return type(~mValue);
    }

private:
    value_type mValue;
};

template< class E, class = std::enable_if_t<decltype(allow_enum_bitset(std::declval<E>()))::value> >
constexpr enum_bitset<E> operator|(E lhs, E rhs)
{
    using bitset_type = ::enum_bitset<E>;
    using value_type = typename bitset_type::value_type;
    return bitset_type(static_cast<value_type>(lhs) | static_cast<value_type>(rhs));
}
template< class E, class = std::enable_if_t<decltype(allow_enum_bitset(std::declval<E>()))::value> >
constexpr enum_bitset<E> operator|(E lhs, enum_bitset<E> rhs)
{
    return rhs | lhs;
}

template< class E, class = std::enable_if_t<decltype(allow_enum_bitset(std::declval<E>()))::value> >
constexpr enum_bitset<E> operator&(E lhs, E rhs)
{
    using bitset_type = ::enum_bitset<E>;
    using value_type = typename bitset_type::value_type;
    return bitset_type(static_cast<value_type>(lhs) & static_cast<value_type>(rhs));
}
template< class E, class = std::enable_if_t<decltype(allow_enum_bitset(std::declval<E>()))::value> >
constexpr enum_bitset<E> operator&(E lhs, enum_bitset<E> rhs)
{
    return rhs & lhs;
}

template< class E, class = std::enable_if_t<decltype(allow_enum_bitset(std::declval<E>()))::value> >
constexpr enum_bitset<E> operator^(E lhs, E rhs)
{
    using bitset_type = ::enum_bitset<E>;
    using value_type = typename bitset_type::value_type;
    return bitset_type(static_cast<value_type>(lhs) ^ static_cast<value_type>(rhs));
}
template< class E, class = std::enable_if_t<decltype(allow_enum_bitset(std::declval<E>()))::value> >
constexpr enum_bitset<E> operator^(E lhs, enum_bitset<E> rhs)
{
    return rhs ^ lhs;
}
