
#include "tests.hpp"
#include "../math.hpp"

BOOST_AUTO_TEST_SUITE( test_suite_sparse_vector )

BOOST_AUTO_TEST_CASE( test_math_lee )
{ try {
  mtl::numeric::matrix_t<std::ptrdiff_t> map
  {  1, 1, 1, 0, 1, 0,
     0, 1, 1, 1, 1, 1,
     0, 0, 1, 0, 0, 0,
     1, 1, 1, 1, 0, 1,
     1, 0, 1, 1, 1, 1,
     1, 1, 1, 0, 0, 0  };
  BOOST_TEST_MESSAGE( "map :\n" << map );

  auto create_way = [&map]( auto const& way, auto const& map_for_check )
  {
    BOOST_TEST_MESSAGE( "min way :\n" << way ); 
    auto m = map;
    MTL_FOREACH( i, std::size( way ) )
        m[ way[i][2] ][ way[i][1] ] = 7;
    BOOST_TEST_MESSAGE( "way in map :\n" << map );
    BOOST_CHECK( m == map_for_check );
  };

  mtl::numeric::matrix_t<std::ptrdiff_t> map_for_check
  {  7, 7, 7, 0, 1, 0,
     0, 1, 7, 1, 1, 1,
     0, 0, 7, 0, 0, 0,
     1, 1, 7, 1, 0, 7,
     1, 0, 7, 7, 7, 7,
     1, 1, 1, 0, 0, 0  };

  BOOST_TEST_MESSAGE( "testing lee (1) : x0 = 0, y0 = 0, x1 = 5, y1 = 3" );
  create_way( mtl::lee( map, 0, 0, 5, 3, 1 ), map_for_check );

  map_for_check =
  {  1, 1, 7, 0, 1, 0,
     0, 1, 7, 1, 1, 1,
     0, 0, 7, 0, 0, 0,
     1, 1, 7, 1, 0, 1,
     1, 0, 7, 7, 1, 1,
     1, 1, 1, 0, 0, 0  };

  BOOST_TEST_MESSAGE( "testing lee (2) : x0 = 2, y0 = 2, x1 = 3, y1 = 4" );
  create_way( mtl::lee( map, 2, 2, 3, 4, 1 ), map_for_check );

  map_for_check =
  {  1, 1, 1, 0, 1, 0,
     0, 1, 7, 7, 7, 1,
     0, 0, 7, 0, 0, 0,
     1, 1, 7, 1, 0, 1,
     1, 0, 7, 1, 1, 1,
     7, 7, 7, 0, 0, 0  };

  BOOST_TEST_MESSAGE( "testing lee (3) : x0 = 4, y0 = 1, x1 = 0, y1 = 5" );
  create_way( mtl::lee( map, 4, 1, 0, 5, 1 ), map_for_check );
} MTL_CATCH }

BOOST_AUTO_TEST_CASE( test_math_move )
{ try {
  mtl::numeric::matrix3_t<std::ptrdiff_t> m
  {  10, 15, 1,
     20, 25, 1  };
  BOOST_TEST_MESSAGE( "m :\n" << m );
  BOOST_TEST_MESSAGE( "testing move  (1) : x0 = 12, y0 = 12, x1 = 52, y1 = 71" );
  mtl::move( m, 12, 12, 52, 71 );
  BOOST_TEST_MESSAGE( "m :\n" << m );
  BOOST_TEST_MESSAGE( "testing move  (2) : x0 = 52, y0 = 71" );
  mtl::move( m, 52, 71 );
  BOOST_TEST_MESSAGE( "m :\n" << m );
} MTL_CATCH }

BOOST_AUTO_TEST_CASE( test_math_rotate )
{ try {
  mtl::numeric::matrix3_t<std::ptrdiff_t> m
  {  10, 15, 1,
     20, 25, 1  };
  BOOST_TEST_MESSAGE( "m :\n" << m );
  BOOST_TEST_MESSAGE( "testing rotate  (1) : angle = 90, x0 = 20, y0 = 25" );
  mtl::rotate( m, k_pi/2, 20, 25 );
  BOOST_TEST_MESSAGE( "m :\n" << m );
  BOOST_TEST_MESSAGE( "testing rotate  (2) : angle = 45" );
  mtl::rotate( m, k_pi/4 );
  BOOST_TEST_MESSAGE( "m :\n" << m );
} MTL_CATCH }

BOOST_AUTO_TEST_CASE( test_math_scale)
{ try {
  mtl::numeric::matrix3_t<std::ptrdiff_t> m
  {  10, 15, 1,
     20, 25, 1  };
  BOOST_TEST_MESSAGE( "m :\n" << m );
  BOOST_TEST_MESSAGE( "testing scale  (1) : kx = 2, ky = 1" );
  mtl::scale( m, 2, 1 );
  BOOST_TEST_MESSAGE( "testing scale  (2) : kx = 0.5, ky = 2" );
  mtl::scale( m, 0.5, 2 );
} MTL_CATCH }

BOOST_AUTO_TEST_SUITE_END()