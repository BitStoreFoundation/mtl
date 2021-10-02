#include "matrix.hpp"

using namespace tvd;
using std::cout;
using std::endl;
using matrix_3xn_t = detail::matrix_3xn_t<std::ptrdiff_t>

int main() 
{
  matrix_3xn_t m1 
  {   0, 1, 2, 
      3, 4, 5,
      6, 7, 8   };
  
  cout << m1 << endl;
  
  return 0;
}