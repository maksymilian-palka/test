//  (C) Copyright Gennadiy Rozental 2001.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/test for the library home page.
//
/// @file
/// Defines @ref test_case_counter
// ***************************************************************************

#ifndef BOOST_TEST_TREE_TEST_CASE_COLLECTOR_HPP_100211GER
#define BOOST_TEST_TREE_TEST_CASE_COLLECTOR_HPP_100211GER

// Boost.Test
#include <boost/test/detail/config.hpp>
#include <boost/test/utils/class_properties.hpp>

#include <boost/test/tree/test_unit.hpp>
#include <boost/test/tree/visitor.hpp>

#include <boost/test/detail/suppress_warnings.hpp>

//____________________________________________________________________________//

namespace boost {
namespace unit_test {

// ************************************************************************** //
// **************                test_case_counter             ************** //
// ************************************************************************** //

///! Counts the number of enabled test cases
class test_case_collector : public test_tree_visitor {
public:
    // Constructor
    // @param ignore_disabled ignore the status when counting
    test_case_collector(bool only_ids = true, bool ignore_status = false)
    : p_count( 0 )
    , m_only_ids(only_ids)
    , m_ignore_status(ignore_status)
    {}

    BOOST_READONLY_PROPERTY( counter_t, (test_case_collector)) p_count;
    const std::vector<std::string>& testcases() const { return m_testcases; }
    const test_unit_id_list& ids() const { return m_testcase_ids; }
private:
    // test tree visitor interface
    virtual void    visit( test_case const& tc )
    {
        if (m_ignore_status || tc.is_enabled())
        {
            ++p_count.value;
            if (!m_only_ids) {
                m_testcases.push_back(tc.full_name());
            }
            m_testcase_ids.push_back(tc.p_id);
        }
    }
    virtual bool    test_suite_start( test_suite const& ts )    { return m_ignore_status || ts.is_enabled(); }

    std::vector<std::string> m_testcases;
    test_unit_id_list m_testcase_ids;
    bool m_only_ids;
    bool m_ignore_status;
};

} // namespace unit_test
} // namespace boost

#include <boost/test/detail/enable_warnings.hpp>

#endif // BOOST_TEST_TREE_TEST_CASE_COUNTER_HPP_100211GER

