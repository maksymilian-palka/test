//  (C) Copyright Gennadiy Rozental 2003-2015.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/test for the library home page.
//
//  File        : $RCSfile$
//
//  Version     : $Revision: 62023 $
//
//  Description : unit test for class properties facility
// ***************************************************************************

// Boost.Test
#define BOOST_TEST_MODULE Boost.Test run by name/label implementation test
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_parameters.hpp>
#include <boost/test/tree/test_case_counter.hpp>
#include <boost/test/tree/test_case_collector.hpp>
#include <boost/test/tree/traverse.hpp>

namespace utf = boost::unit_test;


void A() {}
void B() {}
void C() {}
void D() {}
void E() {}
void F() {}
void longnameA() {}
void Blongname() {}

//____________________________________________________________________________//

void
test_count( utf::test_suite* master_ts, char const** argv, int argc, unsigned expected )
{
    argc /= sizeof(char*);

    BOOST_TEST_INFO( argv[1] );
    if( argc > 2 )
        BOOST_TEST_INFO( argv[2] );

    utf::runtime_config::init( argc, (char**)argv );
    utf::framework::impl::setup_for_execution( *master_ts );

    utf::test_case_counter tcc;
    utf::traverse_test_tree( master_ts->p_id, tcc );
    BOOST_TEST( tcc.p_count == expected );
}

void
test_names( utf::test_suite* master_ts, char const** argv, int argc, boost::unit_test::const_string expected )
{
    using namespace boost::unit_test;
    argc /= sizeof(char*);

    BOOST_TEST_INFO( argv[1] );
    if( argc > 2 )
        BOOST_TEST_INFO( argv[2] );

    utf::runtime_config::init( argc, (char**)argv );
    utf::framework::impl::setup_for_execution( *master_ts );

    utf::test_case_collector tcc(false);
    utf::traverse_test_tree( master_ts->p_id, tcc );

    int expected_count = 0;
    while (!expected.empty())
    {
        std::size_t idx = expected.find(",");
        const_string tc_name;
        if (idx != const_string::npos) {
            tc_name = expected.substr(0, idx);
            expected.trim_left(idx + 1);
        } else {
            tc_name = expected;
            expected.clear();
        }

        tc_name.trim(" ");
        bool found = std::find(tcc.testcases().begin(), tcc.testcases().end(), tc_name) != tcc.testcases().end();
        BOOST_TEST(found);
        ++expected_count;
    }
    BOOST_TEST(expected_count == tcc.ids().size());
}

void expect_exception(utf::test_suite *master_ts, char const **argv, int argc,
                      boost::unit_test::const_string exception_text) {
  argc /= sizeof(char *);

  BOOST_TEST_INFO(argv[1]);
  if (argc > 2)
    BOOST_TEST_INFO(argv[2]);

  try {
    utf::runtime_config::init(argc, (char **)argv);
    utf::framework::impl::setup_for_execution(*master_ts);
    BOOST_TEST(false);
  } catch (const std::exception &exception) {
    BOOST_TEST(exception.what() == exception_text);
  }
}

BOOST_AUTO_TEST_CASE( test_run_by_name )
{
    utf::test_suite* master_ts = BOOST_TEST_SUITE("local master");

    utf::test_suite* ts1 = BOOST_TEST_SUITE("ts1");
    ts1->add( BOOST_TEST_CASE( &C ) );
    ts1->add( BOOST_TEST_CASE( &D ) );

    utf::test_suite* ts2 = BOOST_TEST_SUITE("ts2");
    ts2->add( BOOST_TEST_CASE( &A ) );
    ts2->add( BOOST_TEST_CASE( &C ) );
    ts2->add( BOOST_TEST_CASE( &longnameA ) );
    ts2->add( ts1 );

    master_ts->add( BOOST_TEST_CASE( &A ) );
    master_ts->add( BOOST_TEST_CASE( &B ) );
    master_ts->add( BOOST_TEST_CASE( &longnameA ) );
    master_ts->add( BOOST_TEST_CASE( &Blongname ) );
    master_ts->add( ts2 );

    master_ts->p_default_status.value = utf::test_unit::RS_ENABLED;
    utf::framework::finalize_setup_phase( master_ts->p_id );
    {
        utf::framework::impl::setup_for_execution( *master_ts );
        utf::test_case_counter tcc;
        utf::traverse_test_tree( master_ts->p_id, tcc );
        BOOST_TEST( tcc.p_count == 9U );
    }

    {
        char const* argv[] = { "a.exe", "--run_test=*" };
        test_count( master_ts, argv, sizeof(argv), 9 );
    }

    {
        char const* argv[] = { "a.exe", "--run_test=!*" };
        test_count( master_ts, argv, sizeof(argv), 0 );
    }

    {
        char const* argv[] = { "a.exe", "--run_test=*/*" };
        test_count( master_ts, argv, sizeof(argv), 5 );
    }

    {
        char const* argv[] = { "a.exe", "--run_test=!*/*" };
        test_count( master_ts, argv, sizeof(argv), 4 );
    }

    {
        char const* argv[] = { "a.exe", "--run_test=*/*/*" };
        test_count( master_ts, argv, sizeof(argv), 2 );
    }
    {
        char const* argv[] = { "a.exe", "--run_test=!*/*/*" };
        test_count( master_ts, argv, sizeof(argv), 7 );
    }

    {
        char const* argv[] = { "a.exe", "--run_test=klmn" };
        test_count( master_ts, argv, sizeof(argv), 0 );
    }

    {
        char const* argv[] = { "a.exe", "--run_test=!klmn" };
        test_count( master_ts, argv, sizeof(argv), 9 );
    }

    {
        char const* argv[] = { "a.exe", "--run_test=A" };
        test_count( master_ts, argv, sizeof(argv), 1 );
    }

    {
        char const* argv[] = { "a.exe", "--run_test=!A" };
        test_count( master_ts, argv, sizeof(argv), 8 );
    }

    {
        char const* argv[] = { "a.exe", "--run_test=*A" };
        test_count( master_ts, argv, sizeof(argv), 2 );
    }

    {
        char const* argv[] = { "a.exe", "--run_test=!*A" };
        test_count( master_ts, argv, sizeof(argv), 7 );
    }

    {
        char const* argv[] = { "a.exe", "--run_test=B*" };
        test_count( master_ts, argv, sizeof(argv), 2 );
    }

    {
        char const* argv[] = { "a.exe", "--run_test=!B*" };
        test_count( master_ts, argv, sizeof(argv), 7 );
    }

    {
        char const* argv[] = { "a.exe", "--run_test=*ngn*" };
        test_count( master_ts, argv, sizeof(argv), 2 );
    }

    {
        char const* argv[] = { "a.exe", "--run_test=ts2" };
        test_count( master_ts, argv, sizeof(argv), 5 );
    }

    {
        char const* argv[] = { "a.exe", "--run_test=!ts2" };
        test_count( master_ts, argv, sizeof(argv), 4 );
    }

    {
        char const* argv[] = { "a.exe", "--run_test=ts2/*" };
        test_count( master_ts, argv, sizeof(argv), 5 );
    }

    {
        char const* argv[] = { "a.exe", "--run_test=!ts2/*" };
        test_count( master_ts, argv, sizeof(argv), 4 );
    }

    {
        char const* argv[] = { "a.exe", "--run_test=ts2/C" };
        test_count( master_ts, argv, sizeof(argv), 1 );
    }

    {
        char const* argv[] = { "a.exe", "--run_test=!ts2/C" };
        test_count( master_ts, argv, sizeof(argv), 8 );
    }

    {
        char const* argv[] = { "a.exe", "--run_test=ts2/*A" };
        test_count( master_ts, argv, sizeof(argv), 2 );
    }

    {
        char const* argv[] = { "a.exe", "--run_test=!ts2/*A" };
        test_count( master_ts, argv, sizeof(argv), 7 );
    }

    {
        char const* argv[] = { "a.exe", "--run_test=ts2/ts1" };
        test_count( master_ts, argv, sizeof(argv), 2 );
    }

    {
        char const* argv[] = { "a.exe", "--run_test=!ts2/ts1" };
        test_count( master_ts, argv, sizeof(argv), 7 );
    }

    {
        char const* argv[] = { "a.exe", "--run_test=ts2/ts1/C" };
        test_count( master_ts, argv, sizeof(argv), 1 );
    }

    {
        char const* argv[] = { "a.exe", "--run_test=!ts2/ts1/C" };
        test_count( master_ts, argv, sizeof(argv), 8 );
    }

    {
        char const* argv[] = { "a.exe", "--run_test=ts2/ts1/*D*" };
        test_count( master_ts, argv, sizeof(argv), 1 );
    }

    {
        char const* argv[] = { "a.exe", "--run_test=!ts2/ts1/*D*" };
        test_count( master_ts, argv, sizeof(argv), 8 );
    }

    {
        char const* argv[] = { "a.exe", "--run_test=A,B" };
        test_count( master_ts, argv, sizeof(argv), 2 );
    }

    {
        char const* argv[] = { "a.exe", "--run_test=!A,B" };
        test_count( master_ts, argv, sizeof(argv), 7 );
    }

    {
        char const* argv[] = { "a.exe", "--run_test=*A,B" };
        test_count( master_ts, argv, sizeof(argv), 3 );
    }

    {
        char const* argv[] = { "a.exe", "--run_test=!*A,B" };
        test_count( master_ts, argv, sizeof(argv), 6 );
    }

    {
        char const* argv[] = { "a.exe", "--run_test=ts2/C,ts1" };
        test_count( master_ts, argv, sizeof(argv), 3 );
    }

    {
        char const* argv[] = { "a.exe", "--run_test=!ts2/C,ts1" };
        test_count( master_ts, argv, sizeof(argv), 6 );
    }

    {
        char const* argv[] = { "a.exe", "--run_test=ts2/C,ts1/D" };
        test_count( master_ts, argv, sizeof(argv), 1 );
    }

    {
        char const* argv[] = { "a.exe", "--run_test=!ts2/C,ts1/D" };
        test_count( master_ts, argv, sizeof(argv), 8 );
    }

    {
        char const* argv[] = { "a.exe", "--run_test=A", "--run_test=B" };
        test_count( master_ts, argv, sizeof(argv), 2 );
    }

    {
        char const* argv[] = { "a.exe", "--run_test=!A", "--run_test=!B" };
        test_count( master_ts, argv, sizeof(argv), 7 );
    }

    {
        char const* argv[] = { "a.exe", "--run_test=A", "--run_test=ts2/ts1/C" };
        test_count( master_ts, argv, sizeof(argv), 2 );
    }

    {
        char const* argv[] = { "a.exe", "--run_test=A", "--run_test=ts2/ts1/C,D" };
        test_count( master_ts, argv, sizeof(argv), 3 );
    }

    {
        char const* argv[] = { "a.exe", "--run_test=A", "--run_test=ts2/ts1/C,D:longnameA" };
        test_count( master_ts, argv, sizeof(argv), 4 );
    }

    {
        char const* argv[] = { "a.exe", "--run_test=ts2", "--run_test=!ts2/*A" };
        test_count( master_ts, argv, sizeof(argv), 3 );
    }
}

//____________________________________________________________________________//

BOOST_AUTO_TEST_CASE( test_run_by_label )
{
    utf::test_case* tc;

    utf::test_suite* master_ts = BOOST_TEST_SUITE("local master");

    utf::test_suite* ts1 = BOOST_TEST_SUITE("ts1");
    ts1->add_label( "f2" );
    ts1->add( tc=BOOST_TEST_CASE( &C ) );
    tc->add_label( "L1" );
    ts1->add( tc=BOOST_TEST_CASE( &D ) );
    tc->add_label( "L1" );
    tc->add_label( "l2" );

    utf::test_suite* ts2 = BOOST_TEST_SUITE("ts2");
    ts2->add_label( "FAST" );
    ts2->add( tc=BOOST_TEST_CASE( &A ) );
    tc->add_label( "L1" );
    ts2->add( BOOST_TEST_CASE( &C ) );
    ts2->add( BOOST_TEST_CASE( &longnameA ) );
    ts2->add( ts1 );

    master_ts->add( BOOST_TEST_CASE( &A ) );
    master_ts->add( BOOST_TEST_CASE( &B ) );
    master_ts->add( tc=BOOST_TEST_CASE( &longnameA ) );
    tc->add_label( "f2" );
    master_ts->add( BOOST_TEST_CASE( &Blongname ) );
    master_ts->add( ts2 );


    master_ts->p_default_status.value = utf::test_unit::RS_ENABLED;
    utf::framework::finalize_setup_phase( master_ts->p_id );

    {
        char const* argv[] = { "a.exe", "--run_test=@L1" };
        test_count( master_ts, argv, sizeof(argv), 3 );
    }

    {
        char const* argv[] = { "a.exe", "--run_test=!@INV" };
        test_count( master_ts, argv, sizeof(argv), 9 );
    }

    {
        char const* argv[] = { "a.exe", "--run_test=!@L1" };
        test_count( master_ts, argv, sizeof(argv), 6 );
    }

    {
        char const* argv[] = { "a.exe", "--run_test=@l2" };
        test_count( master_ts, argv, sizeof(argv), 1 );
    }

    {
        char const* argv[] = { "a.exe", "--run_test=@inval" };
        test_count( master_ts, argv, sizeof(argv), 0 );
    }

    {
        char const* argv[] = { "a.exe", "--run_test=@FAST" };
        test_count( master_ts, argv, sizeof(argv), 5 );
    }

    {
        char const* argv[] = { "a.exe", "--run_test=@f2" };
        test_count( master_ts, argv, sizeof(argv), 3 );
    }

    {
        char const* argv[] = { "a.exe", "--run_test=@L1", "--run_test=@l2" };
        test_count( master_ts, argv, sizeof(argv), 3 );
    }

    {
        char const* argv[] = { "a.exe", "--run_test=@L1", "--run_test=!@l2" };
        test_count( master_ts, argv, sizeof(argv), 2 );
    }
}

//____________________________________________________________________________//

BOOST_AUTO_TEST_CASE( test_dependency_handling )
{
    utf::test_case* tc;
    utf::test_case* tcB;

    //            M             |
    //         /  |  \          |
    //        /   |   \         |
    //     TS2   TS4  TS3       |
    //     / \    |   / \       |
    //    C   D  TS1 E   F      |
    //           / \            |
    //          A   B           |
    //
    //  D => TS1
    //  B => F

    utf::test_suite* master_ts = BOOST_TEST_SUITE("local master");

    utf::test_suite* ts1 = BOOST_TEST_SUITE("ts1");
    ts1->add( BOOST_TEST_CASE( &A ) );
    ts1->add( tcB=BOOST_TEST_CASE( &B ) );

    utf::test_suite* ts2 = BOOST_TEST_SUITE("ts2");
    ts2->add( BOOST_TEST_CASE( &C ) );
    ts2->add( tc=BOOST_TEST_CASE( &D ) );
    tc->depends_on( ts1 );

    utf::test_suite* ts3 = BOOST_TEST_SUITE("ts3");
    ts3->add( BOOST_TEST_CASE( &E ) );
    ts3->add( tc=BOOST_TEST_CASE( &F ) );
    tcB->depends_on( tc );

    utf::test_suite* ts4 = BOOST_TEST_SUITE("ts4");
    ts4->add( ts1 );

    master_ts->add( ts2 );
    master_ts->add( ts3 );
    master_ts->add( ts4 );

    master_ts->p_default_status.value = utf::test_unit::RS_ENABLED;
    utf::framework::finalize_setup_phase( master_ts->p_id );

    {
        char const* argv[] = { "a.exe", "--run_test=ts2/C" };
        test_count( master_ts, argv, sizeof(argv), 1 );
    }

    {
        char const* argv[] = { "a.exe", "--run_test=ts3" };
        test_count( master_ts, argv, sizeof(argv), 2 );
    }

    {
        char const* argv[] = { "a.exe", "--run_test=ts2/C" };
        test_count( master_ts, argv, sizeof(argv), 1 );
    }

    {
        char const* argv[] = { "a.exe", "--run_test=ts4/ts1/B" };
        test_count( master_ts, argv, sizeof(argv), 2 );
    }

    {
        char const* argv[] = { "a.exe", "--run_test=ts4/ts1" };
        test_count( master_ts, argv, sizeof(argv), 3 );
    }

    {
        char const* argv[] = { "a.exe", "--run_test=ts2" };
        test_count( master_ts, argv, sizeof(argv), 5 );
    }

    {
        char const* argv[] = { "a.exe", "--run_test=!ts3/F" };
        test_count( master_ts, argv, sizeof(argv), 5 );
    }

    {
        char const* argv[] = { "a.exe", "--run_test=!*/ts1" };
        test_count( master_ts, argv, sizeof(argv), 4 );
    }
}

BOOST_AUTO_TEST_CASE(test_run_by_expression_compatibility_1) {
  utf::test_case *tc;

  //
  // ts2 (@FAST): A(@L1), C, longnameA,
  //         suite ts1 (@f2) : C (@L1), D(@L1, @l2),
  // root suite: A, B, longnameA(@f2), Blongname
  //
  // + root
  //  \ - A
  //  | - B
  //  | - longnameA, @f2
  //  | - Blongname
  //  |
  //  + ts2, @FAST
  //   \ - A, @L1
  //   | - C
  //   | - longnameA
  //   |
  //   + ts1, @f2
  //     - C, @L1
  //     - D, @L1 @l2
  //
  //

  utf::test_suite *master_ts = BOOST_TEST_SUITE("local master");

  utf::test_suite *ts1 = BOOST_TEST_SUITE("ts1");
  ts1->add_label("f2");
  ts1->add(tc = BOOST_TEST_CASE(&C));
  tc->add_label("L1");
  ts1->add(tc = BOOST_TEST_CASE(&D));
  tc->add_label("L1");
  tc->add_label("l2");

  utf::test_suite *ts2 = BOOST_TEST_SUITE("ts2");
  ts2->add_label("FAST");
  ts2->add(tc = BOOST_TEST_CASE(&A));
  tc->add_label("L1");
  ts2->add(BOOST_TEST_CASE(&C));
  ts2->add(BOOST_TEST_CASE(&longnameA));
  ts2->add(ts1);

  master_ts->add(BOOST_TEST_CASE(&A));
  master_ts->add(BOOST_TEST_CASE(&B));
  master_ts->add(tc = BOOST_TEST_CASE(&longnameA));
  tc->add_label("f2");
  master_ts->add(BOOST_TEST_CASE(&Blongname));
  master_ts->add(ts2);

  master_ts->p_default_status.value = utf::test_unit::RS_ENABLED;
  utf::framework::finalize_setup_phase(master_ts->p_id);

  {
    char const *argv[] = {"a.exe", "--run_test=(@L1)"};
    test_count(master_ts, argv, sizeof(argv), 3);
  }

  {
    char const *argv[] = {"a.exe", "--run_test=!(@INV)"};
    test_count(master_ts, argv, sizeof(argv), 9);
  }

  {
    char const *argv[] = {"a.exe", "--run_test=(!@INV)"};
    test_count(master_ts, argv, sizeof(argv), 9);
  }

  {
    char const *argv[] = {"a.exe", "--run_test=!(@L1)"};
    test_count(master_ts, argv, sizeof(argv), 6);
  }

  {
    char const *argv[] = {"a.exe", "--run_test=(!@L1)"};
    test_count(master_ts, argv, sizeof(argv), 6);
  }

  {
    char const *argv[] = {"a.exe", "--run_test=(@l2)"};
    test_count(master_ts, argv, sizeof(argv), 1);
  }

  {
    char const *argv[] = {"a.exe", "--run_test=(@inval)"};
    test_count(master_ts, argv, sizeof(argv), 0);
  }

  {
    char const *argv[] = {"a.exe", "--run_test=(@FAST)"};
    test_count(master_ts, argv, sizeof(argv), 5);
  }

  {
    char const *argv[] = {"a.exe", "--run_test=(@f2)"};
    test_count(master_ts, argv, sizeof(argv), 3);
  }

  {
    char const *argv[] = {"a.exe", "--run_test=(@L1)", "--run_test=(@l2)"};
    test_count(master_ts, argv, sizeof(argv), 3);
  }

  {
    char const *argv[] = {"a.exe", "--run_test=(@L1)", "--run_test=!(@l2)"};
    test_count(master_ts, argv, sizeof(argv), 2);
  }
}

BOOST_AUTO_TEST_CASE(test_run_by_expression_compatibility_2) {
  utf::test_suite *master_ts = BOOST_TEST_SUITE("local master");

  utf::test_suite *ts1 = BOOST_TEST_SUITE("ts1");
  ts1->add(BOOST_TEST_CASE(&C));
  ts1->add(BOOST_TEST_CASE(&D));

  utf::test_suite *ts2 = BOOST_TEST_SUITE("ts2");
  ts2->add(BOOST_TEST_CASE(&A));
  ts2->add(BOOST_TEST_CASE(&C));
  ts2->add(BOOST_TEST_CASE(&longnameA));
  ts2->add(ts1);

  master_ts->add(BOOST_TEST_CASE(&A));
  master_ts->add(BOOST_TEST_CASE(&B));
  master_ts->add(BOOST_TEST_CASE(&longnameA));
  master_ts->add(BOOST_TEST_CASE(&Blongname));
  master_ts->add(ts2);

  master_ts->p_default_status.value = utf::test_unit::RS_ENABLED;

  {
    char const *argv[] = {"a.exe", "--run_test=(*)"};
    test_count(master_ts, argv, sizeof(argv), 9);
  }

  {
    char const *argv[] = {"a.exe", "--run_test=(!*)"};
    test_count(master_ts, argv, sizeof(argv), 0);
  }

  {
    char const *argv[] = {"a.exe", "--run_test=(*/*)"};
    test_count(master_ts, argv, sizeof(argv), 5);
  }

  {
    char const *argv[] = {"a.exe", "--run_test=(!*/*)"};
    test_count(master_ts, argv, sizeof(argv), 4);
  }

  {
    char const *argv[] = {"a.exe", "--run_test=(*/*/*)"};
    test_count(master_ts, argv, sizeof(argv), 2);
  }

  {
    char const *argv[] = {"a.exe", "--run_test=(!*/*/*)"};
    test_count(master_ts, argv, sizeof(argv), 7);
  }

  {
    char const *argv[] = {"a.exe", "--run_test=(klmn)"};
    test_count(master_ts, argv, sizeof(argv), 0);
  }

  {
    char const *argv[] = {"a.exe", "--run_test=(!klmn)"};
    test_count(master_ts, argv, sizeof(argv), 9);
  }

  {
    char const *argv[] = {"a.exe", "--run_test=(A)"};
    test_count(master_ts, argv, sizeof(argv), 1);
  }

  {
    char const *argv[] = {"a.exe", "--run_test=(!A)"};
    test_count(master_ts, argv, sizeof(argv), 8);
  }

  {
    char const *argv[] = {"a.exe", "--run_test=(*A)"};
    test_count(master_ts, argv, sizeof(argv), 2);
  }

  {
    char const *argv[] = {"a.exe", "--run_test=(!*A)"};
    test_count(master_ts, argv, sizeof(argv), 7);
  }

  {
    char const *argv[] = {"a.exe", "--run_test=(B*)"};
    test_count(master_ts, argv, sizeof(argv), 2);
  }

  {
    char const *argv[] = {"a.exe", "--run_test=(!B*)"};
    test_count(master_ts, argv, sizeof(argv), 7);
  }

  {
    char const *argv[] = {"a.exe", "--run_test=(*ngn*)"};
    test_count(master_ts, argv, sizeof(argv), 2);
  }

  {
    char const *argv[] = {"a.exe", "--run_test=(!*ngn*)"};
    test_count(master_ts, argv, sizeof(argv), 7);
  }

  {
    char const *argv[] = {"a.exe", "--run_test=(ts2)"};
    test_count(master_ts, argv, sizeof(argv), 5);
  }

  {
    char const *argv[] = {"a.exe", "--run_test=(!ts2)"};
    test_count(master_ts, argv, sizeof(argv), 4);
  }

  {
    char const *argv[] = {"a.exe", "--run_test=(ts2/*)"};
    test_count(master_ts, argv, sizeof(argv), 5);
  }

  {
    char const *argv[] = {"a.exe", "--run_test=(!ts2/*)"};
    test_count(master_ts, argv, sizeof(argv), 4);
  }

  {
    char const *argv[] = {"a.exe", "--run_test=(ts2/C)"};
    test_count(master_ts, argv, sizeof(argv), 1);
  }

  {
    char const *argv[] = {"a.exe", "--run_test=(!ts2/C)"};
    test_count(master_ts, argv, sizeof(argv), 8);
  }

  {
    char const *argv[] = {"a.exe", "--run_test=(ts2/*A)"};
    test_count(master_ts, argv, sizeof(argv), 2);
  }

  {
    char const *argv[] = {"a.exe", "--run_test=(!ts2/*A)"};
    test_count(master_ts, argv, sizeof(argv), 7);
  }

  {
    char const *argv[] = {"a.exe", "--run_test=(ts2/ts1)"};
    test_count(master_ts, argv, sizeof(argv), 2);
  }

  {
    char const *argv[] = {"a.exe", "--run_test=(!ts2/ts1)"};
    test_count(master_ts, argv, sizeof(argv), 7);
  }

  {
    char const *argv[] = {"a.exe", "--run_test=(ts2/ts1/C)"};
    test_count(master_ts, argv, sizeof(argv), 1);
  }

  {
    char const *argv[] = {"a.exe", "--run_test=(!ts2/ts1/C)"};
    test_count(master_ts, argv, sizeof(argv), 8);
  }

  {
    char const *argv[] = {"a.exe", "--run_test=(ts2/ts1/*D*)"};
    test_count(master_ts, argv, sizeof(argv), 1);
  }

  {
    char const *argv[] = {"a.exe", "--run_test=(!ts2/ts1/*D*)"};
    test_count(master_ts, argv, sizeof(argv), 8);
  }

  {
    char const *argv[] = {"a.exe", "--run_test=(A,B)"};
    test_count(master_ts, argv, sizeof(argv), 2);
  }

  {
    char const *argv[] = {"a.exe", "--run_test=(!A,B)"};
    test_count(master_ts, argv, sizeof(argv), 7);
  }

  {
    char const *argv[] = {"a.exe", "--run_test=(*A,B)"};
    test_count(master_ts, argv, sizeof(argv), 3);
  }

  {
    char const *argv[] = {"a.exe", "--run_test=(!*A,B)"};
    test_count(master_ts, argv, sizeof(argv), 6);
  }

  {
    char const *argv[] = {"a.exe", "--run_test=(ts2/C,ts1)"};
    test_count(master_ts, argv, sizeof(argv), 3);
  }

  {
    char const *argv[] = {"a.exe", "--run_test=(ts2/C,ts1/D)"};
    test_count(master_ts, argv, sizeof(argv), 1);
  }

  {
    char const *argv[] = {"a.exe", "--run_test=(!ts2/C,ts1/D)"};
    test_count(master_ts, argv, sizeof(argv), 8);
  }

  {
    char const *argv[] = {"a.exe", "--run_test=(A)", "--run_test=(B)"};
    test_count(master_ts, argv, sizeof(argv), 2);
  }

  {
    char const *argv[] = {"a.exe", "--run_test=(!(A ; B))"};
    test_count(master_ts, argv, sizeof(argv), 7);
  }

  {
    char const *argv[] = {"a.exe", "--run_test=(A)", "--run_test=(ts2/ts1/C)"};
    test_count(master_ts, argv, sizeof(argv), 2);
  }

  {
    char const *argv[] = {"a.exe", "--run_test=(A)",
                          "--run_test=(ts2/ts1/C,D)"};
    test_count(master_ts, argv, sizeof(argv), 3);
  }

  {
    char const *argv[] = {"a.exe", "--run_test=(A)",
                          "--run_test=(ts2/ts1/C,D):(longnameA)"};
    test_count(master_ts, argv, sizeof(argv), 4);
  }

  {
    char const *argv[] = {"a.exe", "--run_test=(ts2 & !(ts2/*A))"};
    test_count(master_ts, argv, sizeof(argv), 3);
  }
}

BOOST_AUTO_TEST_CASE(test_run_by_expression_label) {
  utf::test_case *tc;

  // + root
  //  \ - A, @lg1
  //  | - B, @lg1, @lr1
  //  | - longnameA, @lr1
  //  | - Blongname
  //  |
  //  + ts1, @lts, @lts1
  //  |\ - A, @cat1, @cat_a
  //  || - B, @cat1, @cat_b
  //  || - longnameA
  //  ||
  //  |+ ts11, @lts11
  //  |  - C, @cat_a
  //  |
  //  + ts2, @lts2, @lts
  //   \ - D, @cat1, @cat_a
  //   | - E, @cat1, @cat_e
  //   | - F, @cat2
  //

  utf::test_suite *master_ts = BOOST_TEST_SUITE("root");

  utf::test_suite *ts11 = BOOST_TEST_SUITE("ts11");
  ts11->add_label("lts11");
  ts11->add(tc = BOOST_TEST_CASE(&C));
  tc->add_label("cat_a");

  utf::test_suite *ts1 = BOOST_TEST_SUITE("ts1");
  ts1->add_label("lts");
  ts1->add_label("lts1");
  ts1->add(tc = BOOST_TEST_CASE(&A));
  tc->add_label("cat1");
  tc->add_label("cat_a");
  ts1->add(tc = BOOST_TEST_CASE(&B));
  tc->add_label("cat1");
  tc->add_label("cat_b");
  ts1->add(BOOST_TEST_CASE(&longnameA));
  ts1->add(ts11);

  utf::test_suite *ts2 = BOOST_TEST_SUITE("ts2");
  ts2->add_label("lts");
  ts2->add_label("lts2");
  ts2->add(tc = BOOST_TEST_CASE(&D));
  tc->add_label("cat1");
  tc->add_label("cat_a");
  ts2->add(tc = BOOST_TEST_CASE(&E));
  tc->add_label("cat1");
  tc->add_label("cat_e");
  ts2->add(tc = BOOST_TEST_CASE(&F));
  tc->add_label("cat2");

  master_ts->add(tc = BOOST_TEST_CASE(&A));
  tc->add_label("lg1");
  master_ts->add(tc = BOOST_TEST_CASE(&B));
  tc->add_label("lg1");
  tc->add_label("lr1");
  master_ts->add(tc = BOOST_TEST_CASE(&longnameA));
  tc->add_label("lr1");
  master_ts->add(BOOST_TEST_CASE(&Blongname));
  master_ts->add(ts1);
  master_ts->add(ts2);

  master_ts->p_default_status.value = utf::test_unit::RS_ENABLED;
  utf::framework::finalize_setup_phase(master_ts->p_id);

  // intersection operator

  {
    char const *argv[] = {"a.exe", "--run_test=((((@lg1)) & !@lr1))"};
    test_names(master_ts, argv, sizeof(argv), "root/A");
  }

  {
    char const *argv[] = {"a.exe", "--run_test=((@cat1) & (@cat_e))"};
    test_names(master_ts, argv, sizeof(argv), "root/ts2/E");
  }

  {
    char const *argv[] = {"a.exe", "--run_test=(@cat1 & @cat_e)"};
    test_names(master_ts, argv, sizeof(argv), "root/ts2/E");
  }

  {
    char const *argv[] = {"a.exe", "--run_test=(@lts11)"};
    test_names(master_ts, argv, sizeof(argv), "root/ts1/ts11/C");
  }

  // union operator
  {
    char const *argv[] = {"a.exe", "--run_test=(@lg1; @lr1)"};
    test_names(master_ts, argv, sizeof(argv), "root/A, root/B, root/longnameA");
  }

  // mixed
  {
    char const *argv[] = {"a.exe", "--run_test=((@cat1 & @cat_a); @cat_e)"};
    test_names(master_ts, argv, sizeof(argv),
               "root/ts1/A, root/ts2/D, root/ts2/E");
  }

  {
    char const *argv[] = {"a.exe", "--run_test=(@cat1 & @cat_a; @cat_e)"};
    test_names(master_ts, argv, sizeof(argv),
               "root/ts1/A, root/ts2/D, root/ts2/E");
  }

  {
    char const *argv[] = {"a.exe", "--run_test=(@cat_e; @lr1 & @lg1)"};
    test_names(master_ts, argv, sizeof(argv), "root/B");
  }

  {
    char const *argv[] = {"a.exe", "--run_test=(@cat_e; (@lr1 & @lg1))"};
    test_names(master_ts, argv, sizeof(argv), "root/ts2/E, root/B");
  }

  {
      char const *argv[] = {"a.exe", "--run_test=(@lts & !(@cat_a ; @cat_b ; @cat_e))"};
      test_names(master_ts, argv, sizeof(argv), "root/ts1/longnameA, root/ts2/F");
  }

  // error handling
  {
    char const *argv[] = {"a.exe", "--run_test=(((@cat_a))"};
    expect_exception(master_ts, argv, sizeof(argv),
                     "Mismatch between '(' and ')' brackets.");
  }

  {
    char const *argv[] = {"a.exe", "--run_test=(@cat_a & & @cat_b)"};
    expect_exception(
        master_ts, argv, sizeof(argv),
        "Consecutive operators ('&', ';') not allowed, found after: @cat_a");
  }

  {
    char const *argv[] = {"a.exe", "--run_test=(@cat_a & @cat_b ;; cat_c)"};
    expect_exception(
        master_ts, argv, sizeof(argv),
        "Consecutive operators ('&', ';') not allowed, found after: @cat_b");
  }

  {
    char const *argv[] = {"a.exe", "--run_test=(@cat_a & @cat_b ;& cat_c)"};
    expect_exception(
        master_ts, argv, sizeof(argv),
        "Consecutive operators ('&', ';') not allowed, found after: @cat_b");
  }

  {
    char const *argv[] = {"a.exe", "--run_test=(@cat_a & @cat_b &; cat_c)"};
    expect_exception(
        master_ts, argv, sizeof(argv),
        "Consecutive operators ('&', ';') not allowed, found after: @cat_b");
  }

  {
    char const *argv[] = {"a.exe",
                          "--run_test=(@cat_a & @cat_b ; cat_c !! cat_d)"};
    expect_exception(
        master_ts, argv, sizeof(argv),
        "Consecutive operator '!' not allowed, found after: cat_c");
  }

  {
    char const *argv[] = {"a.exe",
                          "--run_test=(@cat_a & @cat_b ; cat_c !& cat_d)"};
    expect_exception(
        master_ts, argv, sizeof(argv),
        "Inverse operator '!' not allowed before '&', found after: cat_c");
  }

  {
    char const *argv[] = {"a.exe",
                          "--run_test=(@cat_a & @cat_b ; cat_c !; cat_d)"};
    expect_exception(
        master_ts, argv, sizeof(argv),
        "Inverse operator '!' not allowed before ';', found after: cat_c");
  }
}
//____________________________________________________________________________//

// EOF

