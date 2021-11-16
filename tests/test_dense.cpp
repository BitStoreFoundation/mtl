
#include "tests.hpp"
#include "../numeric/dense.hpp"

    struct dense_vector_fixture
    {
	    mtl::numeric::vector_n_t<std::ptrdiff_t, 9> v;
      dense_vector_fixture()
        : v( { 1, 2, 2, 4, 5, 6, 7, 8, 9 } )
      { BOOST_TEST_MESSAGE( "vector construct :\n" << v << "end\n" ); }
    };

    struct dense_matrix_fixture
    {
	    mtl::numeric::matrix3_t<std::ptrdiff_t> m;
      dense_matrix_fixture()
        : m( { 1, 2, 2, 
               4, 5, 6, 
               7, 8, 9 } )
      { BOOST_TEST_MESSAGE( "matrix construct :\n" << m << "end\n" ); }
    };

BOOST_AUTO_TEST_SUITE( test_suite_dense_vector )

BOOST_FIXTURE_TEST_CASE( test_dense_vector_create, dense_vector_fixture )
{ try {
  BOOST_TEST_MESSAGE( "checking mtl::dense_vector<std::ptrdiff_t, 9> v" );
  BOOST_REQUIRE_NO_THROW( v[3] == 4 );
  BOOST_REQUIRE_NO_THROW( v[2] == 2 );
  BOOST_REQUIRE( std::size( v ) == 9 );
} MTL_CATCH }

BOOST_FIXTURE_TEST_CASE( test_dense_vector_copy_and_compare, dense_vector_fixture )
{ try {
  decltype( v ) v_( v );
  BOOST_TEST_MESSAGE( "v_ :" );
  BOOST_TEST_MESSAGE( v_ );
  BOOST_TEST_MESSAGE( "checking v != v_" );
  BOOST_REQUIRE( v == v_ );
  BOOST_TEST_MESSAGE( "!( checking v == v_ )" );
  BOOST_REQUIRE( !( v != v_ ) );
} MTL_CATCH }

BOOST_FIXTURE_TEST_CASE( test_dense_vector_assign_and_move, dense_vector_fixture )
{ try {
  BOOST_TEST_MESSAGE( "assignment v to v1" );
  decltype( v ) v_;
  v_ = v;
  BOOST_TEST_MESSAGE( "v_ :" );
  BOOST_TEST_MESSAGE( v_ );
  BOOST_CHECK( v == v_ );
} MTL_CATCH }

BOOST_FIXTURE_TEST_CASE( test_dense_vector_operations, dense_vector_fixture )
{ try {
  BOOST_TEST_MESSAGE( "copying v to v_" );
  decltype( v ) v_( v );
  BOOST_TEST_MESSAGE( "checking v_ *= 1" );
  v_ *= 1;
  BOOST_TEST_MESSAGE( "v_ :" );
  BOOST_TEST_MESSAGE( v_ );
  BOOST_CHECK( v == v_ );
  BOOST_CHECK( v_[3] == 4 );
  BOOST_CHECK( v_[2] == 2 );
  BOOST_CHECK( std::size( v_ ) == 9 );
  BOOST_TEST_MESSAGE( "checking v_ /= 1" );
  v_ /= 1;
  BOOST_TEST_MESSAGE( "v_ :" );
  BOOST_TEST_MESSAGE( v_ );
  BOOST_CHECK( v == v_ );
  BOOST_CHECK( v_[3] == 4 );
  BOOST_CHECK( v_[2] == 2 );
  BOOST_CHECK( std::size( v_ ) == 9 );
  BOOST_TEST_MESSAGE( "checking v_ /= 0" );
  BOOST_CHECK_THROW( v_ /= 0, mtl::exception_t );
  BOOST_TEST_MESSAGE( v_ );
  BOOST_TEST_MESSAGE( "checking v_ += empty" );
  decltype( v ) empty;
  BOOST_TEST_MESSAGE( empty );
  v_ += empty;
  BOOST_TEST_MESSAGE( "v_ :" );
  BOOST_TEST_MESSAGE( v_ );
  BOOST_CHECK( v == v_ );
  BOOST_CHECK( v_[3] == 4 );
  BOOST_CHECK( v_[2] == 2 );
  BOOST_CHECK( std::size( v_ ) == 9 );
  BOOST_TEST_MESSAGE( "checking v_ -= empty" );
  v_ -= empty;
  BOOST_TEST_MESSAGE( "v_ :" );
  BOOST_TEST_MESSAGE( v_ );
  BOOST_CHECK( v == v_ );
  BOOST_CHECK( v_[3] == 4 );
  BOOST_CHECK( v_[2] == 2 );
  BOOST_CHECK( std::size( v_ ) == 9 );
} MTL_CATCH }

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE( test_suite_dense_matrix )

BOOST_FIXTURE_TEST_CASE( test_dense_matrix_create, dense_matrix_fixture )
{ try {
  BOOST_TEST_MESSAGE( "checking mtl::dense_matrix<std::ptrdiff_t> m" );
  BOOST_CHECK( m.size() == 3 );
  BOOST_CHECK( m.csize() == 3 );
  BOOST_TEST_MESSAGE( m[0][2] );
  BOOST_TEST_MESSAGE( m[1][0] );
  BOOST_REQUIRE_NO_THROW( m[0][2] == 2 );
  BOOST_CHECK_NO_THROW( m[1][0] == 4 );
} MTL_CATCH }

BOOST_FIXTURE_TEST_CASE( test_dense_matrix_copy_and_compare, dense_matrix_fixture )
{ try {
  decltype( m ) m_( m );
  BOOST_TEST_MESSAGE( "m_ :" );
  BOOST_TEST_MESSAGE( m_ );
  BOOST_TEST_MESSAGE( "checking m != m_" );
  BOOST_REQUIRE( m == m_ );
  BOOST_TEST_MESSAGE( "!( checking m == m_ )" );
  BOOST_REQUIRE( !( m != m_ ) );
} MTL_CATCH }

BOOST_FIXTURE_TEST_CASE( test_dense_matrix_assign_and_move, dense_matrix_fixture )
{ try {
  BOOST_TEST_MESSAGE( "assignment m to m_" );
  decltype( m ) m_;
  m_ = m;
  BOOST_TEST_MESSAGE( "m_ :" );
  BOOST_TEST_MESSAGE( m_ );
  BOOST_CHECK( m == m_ );
} MTL_CATCH }

BOOST_FIXTURE_TEST_CASE( test_dense_matrix_operations, dense_matrix_fixture )
{ try {
  BOOST_TEST_MESSAGE( "copying m to m_" );
  decltype( m ) m_( m );
  BOOST_TEST_MESSAGE( "checking m_ *= 1" );
  m_ *= 1;
  BOOST_TEST_MESSAGE( "m_ :" );
  BOOST_TEST_MESSAGE( m_ );
  BOOST_CHECK( m == m_ );
  BOOST_CHECK( std::size( m_ ) == 3 );
  BOOST_TEST_MESSAGE( "checking m_ /= 1" );
  m_ /= 1;
  BOOST_TEST_MESSAGE( "m_ :" );
  BOOST_TEST_MESSAGE( m_ );
  BOOST_CHECK( m == m_ );
  BOOST_CHECK( std::size( m_ ) == 3 );
  BOOST_TEST_MESSAGE( "checking m_ /= 0" );
  BOOST_CHECK_THROW( m_ /= 0, mtl::exception_t );
  BOOST_TEST_MESSAGE( "checking m_ += empty" );
  decltype( m ) empty( 3 );
  BOOST_TEST_MESSAGE( empty );
  m_ += empty;
  BOOST_TEST_MESSAGE( "m_ :" );
  BOOST_TEST_MESSAGE( m_ );
  BOOST_CHECK( m == m_ );
  BOOST_CHECK( std::size( m_ ) == 3 );
  BOOST_TEST_MESSAGE( "checking m_ -= empty" );
  m_ -= empty;
  BOOST_TEST_MESSAGE( "m_ :" );
  BOOST_TEST_MESSAGE( m_ );
  BOOST_CHECK( m == m_ );
  BOOST_CHECK( std::size( m_ ) == 3 );
  BOOST_TEST_MESSAGE( "m_[0] += m[0] :" );
  m_[0] += m[0];
  BOOST_TEST_MESSAGE( "m_ :" );
  BOOST_TEST_MESSAGE( m_ );
  BOOST_CHECK( m != m_ );
  BOOST_CHECK( std::size( m_ ) == 3 );
  mtl::numeric::vector3_t<std::ptrdiff_t> v( m[0] );
  BOOST_TEST_MESSAGE( "m_[0] -= v :" );
  m_[0] -= v;
  BOOST_TEST_MESSAGE( "m_ :" );
  BOOST_TEST_MESSAGE( m_ );
  BOOST_CHECK( m == m_ );
  BOOST_CHECK( std::size( m_ ) == 3 );
  BOOST_TEST_MESSAGE( "m_ = m1*m2 :" );
  decltype( m ) m1 
  {   1, 1, 1,
      1, 1, 1,
      1, 1, 1   };
  BOOST_TEST_MESSAGE( "m1 :" );
  BOOST_TEST_MESSAGE( m1 );
  auto m2( m1 );
  BOOST_TEST_MESSAGE( "m2 :" );
  BOOST_TEST_MESSAGE( m2 );
  m_ = m2*m1;
  BOOST_TEST_MESSAGE( "m_ :" );
  BOOST_TEST_MESSAGE( m_ );
  BOOST_TEST_MESSAGE( "m2 *= m1 :" );
  m2 *= m1;
  BOOST_TEST_MESSAGE( "m2 :" );
  BOOST_TEST_MESSAGE( m2 );
  BOOST_CHECK( m2 == m_ );
  BOOST_CHECK( std::size( m2 ) == 3 );
} MTL_CATCH }

BOOST_AUTO_TEST_SUITE_END()