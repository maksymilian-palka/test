//  (C) Copyright Raffi Enficiaud 2019.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/test for the library home page.
//
//  Description : timer and elapsed types
// ***************************************************************************

#ifndef BOOST_TEST_UTILS_TIMER_HPP
#define BOOST_TEST_UTILS_TIMER_HPP

#include <boost/test/unit_test_parameters.hpp>
#include <ctime>
#include <sstream>

# ifdef BOOST_NO_STDC_NAMESPACE
  namespace std { using ::clock_t; using ::clock; }
# endif

namespace boost {
namespace unit_test {
namespace timer {

  struct elapsed_time
  {
    typedef boost::int_least64_t nanosecond_type;

    nanosecond_type wall;
    void clear() { wall = 0; }
  };

  inline double
  microsecond_wall_time( elapsed_time const& elapsed )
  {
      return elapsed.wall / 1E3;
  }

  inline double
  second_wall_time( elapsed_time const& elapsed )
  {
      return elapsed.wall / 1E9;
  }

  //! Simple timing class
  //!
  //! This class measures the wall clock time.
  class timer
  {
  public:
    timer()
    {
        _start_time = std::clock();
    }
    void restart()
    {
        _start_time = std::clock();
    }

    // return elapsed time in seconds
    elapsed_time elapsed() const
    {
      static const double to_nano_seconds = 1E9 / CLOCKS_PER_SEC;
      elapsed_time return_value;
      return_value.wall = static_cast<elapsed_time::nanosecond_type>(double(std::clock() - _start_time) * to_nano_seconds);
      return return_value;
    }

   private:
      std::clock_t _start_time;
  };


//____________________________________________________________________________//

} // namespace timer
} // namespace unit_test
} // namespace boost

#endif // BOOST_TEST_UTILS_TIMER_HPP

