// Created by mars on 2/8/24.

#ifndef MARS_BASE_TIMESTAMP_H
#define MARS_BASE_TIMESTAMP_H

#include "copyable.h"
#include <string>

namespace mars {
namespace base {

class Timestamp : public copyable{
public:
    Timestamp() : m_microSecondsSinceEpoch(0) {}
    explicit Timestamp(int64_t microSecondsSinceEpoch) : m_microSecondsSinceEpoch(microSecondsSinceEpoch) {}

    std::string toString() const;
      std::string toFormattedString() const;

    int64_t microSecondsSinceEpoch() const { return m_microSecondsSinceEpoch; }
    time_t secondsSinceEpoch() const { return static_cast<time_t>(m_microSecondsSinceEpoch / kMicroSecondsPerSecond); }

    bool valid() const { return m_microSecondsSinceEpoch > 0; }

    static Timestamp now();
    static Timestamp invalid(){
        return Timestamp();
    }

  void swap(Timestamp& that)
  {
    std::swap(m_microSecondsSinceEpoch, that.m_microSecondsSinceEpoch);
  }

    static const int kMicroSecondsPerSecond = 1000 * 1000;

private:
    int64_t m_microSecondsSinceEpoch;
};

inline Timestamp addTime(Timestamp timestamp, double seconds)
{
  int64_t delta = static_cast<int64_t>(seconds * Timestamp::kMicroSecondsPerSecond);
  return Timestamp(timestamp.microSecondsSinceEpoch() + delta);
}

inline bool operator<(Timestamp lhs, Timestamp rhs)
{
  return lhs.microSecondsSinceEpoch() < rhs.microSecondsSinceEpoch();
}

inline bool operator==(Timestamp lhs, Timestamp rhs)
{
  return lhs.microSecondsSinceEpoch() == rhs.microSecondsSinceEpoch();
}

} // namespace base
} // namespace mars

#endif // MARS_BASE_TIMESTAMP_H
