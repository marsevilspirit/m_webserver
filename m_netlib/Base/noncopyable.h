// Created by mars on 2/8/24.

#ifndef MARS_BASE_NONCOPYABLE_H
#define MARS_BASE_NONCOPYABLE_H

namespace mars {

struct base_token {};

class noncopyable : base_token {
protected:
    noncopyable() = default;
    ~noncopyable() = default;

    noncopyable(const noncopyable&) = delete;
    noncopyable& operator=(const noncopyable&) = delete;
};

} // namespace mars

#endif // MARS_BASE_NONCOPYABLE_H
