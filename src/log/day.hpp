//
// Created by Rakesh on 2019-05-16.
//

#pragma once

#include <chrono>
#include <iosfwd>
#include <iomanip>

namespace nanolog
{
  struct Day
  {
    Day()
    {
      const auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
      auto gmtime = std::gmtime(&now);
      year = gmtime->tm_year + 1900;
      month = gmtime->tm_mon + 1;
      day = gmtime->tm_mday;
    }

    ~Day() = default;
    Day(const Day&) = default;
    Day(Day&&) = default;
    Day& operator=(const Day&) = default;
    Day& operator=(Day&&) = default;

    bool operator==(const Day& rhs) const
    {
      return day == rhs.day && month == rhs.month && year == rhs.year;
    }

    bool operator!=(const Day& rhs) const
    {
      return !(*this == rhs);
    }

    int16_t year;
    int16_t month;
    int16_t day;
  };
}

std::ostream& operator<<(std::ostream& s, const nanolog::Day& day)
{
  s << day.year << '-'
    << std::setfill('0') << std::setw(2) << day.month << '-'
    << std::setfill('0') << std::setw(2) << day.day;
  return s;
}
