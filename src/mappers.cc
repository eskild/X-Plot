//    X-Plot  --  a program for visualizing complex functions
//
//    Copyright (C) 1996-2011 Martin Eskildsen.
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//    Comments, suggestions, questions to:
//    	     Martin Eskildsen
//    	     eskild.opensource@gmail.com
//
//
//    Programming by Martin Eskildsen, (then) student at the
//    Technical University of Denmark.

#include "basewin.h"
#include "calcenv.h"

//****************************************************************************
// DisplayMapperBase
//
// Features:
//
//
//
//
//
//
//****************************************************************************


DisplayMapperBase::DisplayMapperBase()
{
  grid.x = 1;	// default: 2x2 screen grid! Only to provide safe value
  grid.y = 1;	// for derived classes.
  low_left = Complex(-1.0, -1.0);
  up_right = Complex( 1.0,  1.0);
  dpi      = 96;
}

int DisplayMapperBase::Cm_to_Dots(double cm)
{
  return int(dpi/2.54 * cm);
}

// g is the highest coordinate in a (0,0) based window system. The actual size
// in pixels is g.x + 1, g.y + 1.
void DisplayMapperBase::SetScreenGeometry(DisplayPoint g, int d_p_i)
{
  assert(grid.x>0  &&  grid.y>0);
  grid = g;
  grid.x++;
  grid.y++;
  if (d_p_i > 0) dpi = d_p_i;
}

void DisplayMapperBase::SetVirtualGeometry(const Complex lower_left,
					   const Complex upper_right)
{
  SuperBox b(lower_left);  // make sure low_left really IS at the lower left corner
  b.AddPoint(upper_right).GetResult(low_left, up_right);
  calc_internals();
}

void DisplayMapperBase::GetVirtualGeometry(Complex& l_l, Complex& u_r)
{
  l_l = low_left;
  u_r = up_right;
}

void DisplayMapperBase::GetScreenGeometry(DisplayPoint &dp)
{
  dp.x = grid.x - 1;
  dp.y = grid.y - 1;
}

//****************************************************************************
// LinearDisplayMapper
//
// Features:
//	A mapper for linear x- and y-axis. It also works when the window is
//	resized, as the ratios don't change - it's just the window dimensions
//	that change.
//
//	Note that this mapper assumes (0,0) at the LOWER left corner!
//****************************************************************************

void LinearDisplayMapper::calc_internals()
{
  assert(grid.x>0  &&  grid.y>0);

  if (setjmp(MathErrorJumpBuf) == -1) return;

  Complex delta;
  delta = up_right - low_left;
  step_x =  ::real(delta) / grid.x;
  step_y =  ::imag(delta) / grid.y;

  // Make physical space square:
  step_x = (step_x > step_y) ? step_x : step_y;
  step_y = step_x;

  // Centre at the longer axis (ie. if length < height, centre height)
  double s_g_x = step_x * grid.x;
  double s_g_y = step_y * grid.y;
  if (s_g_x > ::real(delta)) {  // wider: centre width
    double d = (s_g_x - ::real(delta)) / 2.0;
    low_left -= d;
    up_right += d;
  }
  else if (s_g_y > ::imag(delta)) { // higher: centre height
    Complex d(0, (s_g_y - ::imag(delta)) / 2.0);
    low_left -= d;
    up_right += d;
  }

  // Calc arrow segment length: (defaults to 0.2 cm)
  DisplayPoint zero = Map(Complex(0.0, 0.0));
  arrow_segment_length = ::real(Map(DisplayPoint( zero.x + Cm_to_Dots(0.2), 0)));
}

void LinearDisplayMapper::Centre(const Complex &c)
{
  if (setjmp(MathErrorJumpBuf) == -1) return;

  Complex win_ll = Map(DisplayPoint(0, grid.y-1));
  Complex win_ur = Map(DisplayPoint(grid.x-1, 0));

  Complex delta = (win_ur - win_ll) / 2.0;

  SetVirtualGeometry(c - delta, c + delta);
}

DisplayPoint LinearDisplayMapper::Map(const Complex &c)
{
  if (setjmp(MathErrorJumpBuf) == -1) return DisplayPoint(1, 1);

  Complex diff = c - low_left;
  return DisplayPoint( int( ::real(diff)/step_x ), 
		       int( grid.y- ::imag(diff)/step_y ) );
}

Complex LinearDisplayMapper::Map(DisplayPoint p)
{
  if (setjmp(MathErrorJumpBuf) == -1) return Complex(1.0, 1.0);

  p.y = (grid.y-1) - p.y; // now, both coord sys have (0,0) at lower left corner

  return Complex( p.x*step_x, p.y*step_y ) + low_left;
}

DisplayPoint LinearDisplayMapper::GetUpperRightCornerOfDisplay()
{
  return DisplayPoint(grid.x-1, 0);
}

DisplayPoint LinearDisplayMapper::GetLowerLeftCornerOfDisplay()
{
  return DisplayPoint(0, grid.y-1);
}

//****************************************************************************
// LinearDisplayMapper2
//
// Features:
//	A mapper for linear x- and y-axis. It also works when the window is
//	resized, as the ratios don't change - it's just the window dimensions
//	that change.
//
//	Note that this mapper assumes (0,0) at the LOWER left corner!
//****************************************************************************

void LinearDisplayMapper2::Centre(const Complex &c)
{
  if (setjmp(MathErrorJumpBuf) == -1) return;

  Complex win_ll = Map(DisplayPoint(0, 0));
  Complex win_ur = Map(DisplayPoint(grid.x-1, grid.y-1));

  Complex delta = (win_ur - win_ll) / 2.0;

  SetVirtualGeometry(c - delta, c + delta);
}

DisplayPoint LinearDisplayMapper2::Map(const Complex &c)
{
  if (setjmp(MathErrorJumpBuf) == -1) return DisplayPoint(1, 1);

  Complex diff = c - low_left;
  return DisplayPoint( int( ::real(diff)/step_x ), 
		       int( ::imag(diff)/step_y ) );
}

Complex LinearDisplayMapper2::Map(DisplayPoint p)
{
  if (setjmp(MathErrorJumpBuf) == -1) return Complex(1.0, 1.0);

  return Complex( p.x*step_x, p.y*step_y ) + low_left;
}

DisplayPoint LinearDisplayMapper2::GetUpperRightCornerOfDisplay()
{
  return DisplayPoint(grid.x-1, grid.y-1);
}

DisplayPoint LinearDisplayMapper2::GetLowerLeftCornerOfDisplay()
{
  return DisplayPoint(0, 0);
}

// EOF
