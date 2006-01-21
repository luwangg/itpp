/*!
 * \file 
 * \brief Schur decomposition test program
 * \author Adam Piatyszek
 *
 * $Date$
 * $Revision$
 *
 * -------------------------------------------------------------------------
 *
 * IT++ - C++ library of mathematical, signal processing, speech processing,
 *        and communications classes and functions
 *
 * Copyright (C) 1995-2005  (see AUTHORS file for a list of contributors)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * -------------------------------------------------------------------------
 */

#include <itpp/itbase.h>

using namespace itpp;
using namespace std;


#if defined(HAVE_LAPACK) || defined(HAVE_MKL)

int main()
{
  cout << "==========================================" << endl;
  cout << "   Test of Schur decomposition routines   " << endl;
  cout << "==========================================" << endl;
  {
    cout << "Real matrix" << endl;
    mat A = randn(5,5);
    mat T, U;
    schur(A, U, T);

    cout << "A = " << round_to_zero(A) << endl;
    cout << "U = " << round_to_zero(U) << endl;
    cout << "T = " << round_to_zero(T) << endl;
    cout << "norm(A - U*T*U^T) = " << 
      round_to_zero(norm(A - (U * T * transpose(U))), 1e-13) << endl;
    cout << "only T = " << round_to_zero(schur(A)) << endl;

  }
  {
    cout << endl << "Complex matrix" << endl;
    cmat A = randn_c(5,5);
    cmat T, U;
    schur(A, U, T);

    cout << "A = " << round_to_zero(A) << endl;
    cout << "U = " << round_to_zero(U) << endl;
    cout << "T = " << round_to_zero(T) << endl;
    cout << "norm(A - U*T*U^T) = " << 
      round_to_zero(norm(A - (U * T * hermitian_transpose(U))), 1e-13) << endl;
    cout << "only T = " << round_to_zero(schur(A)) << endl;
  }

  return 0;
}

#else

int main() { 
  cerr << "Error: LAPACK (or MKL) is needed for this test program" << endl;
  return 1;
}

#endif