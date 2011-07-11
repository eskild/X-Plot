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

#ifndef __XDRAW_H__
#define __XDRAW_H__

#include <Complex.h>

#include "graphobj.h"
#include "basewin.h"

#include <X11/Intrinsic.h>

#ifdef Complex
#undef Complex
#endif

CLASSDEF(XDrawHandler);
CLASSDEF(XSysInfo);

#define NumberOfColors 16

class XDrawHandler : public DrawHandler {
public:
		XDrawHandler(pDisplayMapperBase);
  virtual      ~XDrawHandler();

  virtual	DrawHandler&	AttachToWindow(pBaseWindow);
  
  virtual	DrawHandler&	Clear();
  virtual	DrawHandler&	DrawCircle(const Complex &centre,
					   const double& radius);
  virtual	DrawHandler&	DrawLine(const Complex& p1,
					 const Complex& p2);
  virtual	DrawHandler&	DrawRectangle(const Complex& p1,
					      const Complex& p2);
  virtual	DrawHandler&	DrawDot(const Complex &p);
  virtual	DrawHandler&	DrawCross(const Complex& centre);
  virtual	DrawHandler&	DrawArrow(const Complex &tangent_start,
					  const Complex &tangent_end);
  virtual	DrawHandler&	DrawText(int lineno, const CppStr& text);
  virtual	DrawHandler&	DrawText(const Complex &, const CppStr&);
  virtual	DrawHandler&	SetDrawmode(Drawmode);
  virtual	DrawHandler&	SetLinestyle(Linestyle);
  virtual	DrawHandler&	SelectColor(long);
  virtual	DrawHandler&	SelectColorBlack();
  virtual	DrawHandler&	SelectColorWhite();
  virtual       DrawHandler&    SelectBackgroundColor(BackgroundColor);
  virtual       DrawHandler&    Centre(const Complex&);
  virtual	DrawHandler&	SetVirtualGeometry(const Complex lower_left,
					   	   const Complex upper_right);

  static int 	LineWidth;

protected:
  Widget        area;
  Display       *display;
  Window        window;
  Screen        *screen;
  GC            gc;
  XFontStruct   *font;
  int           fontheight;

  XColor        colors[NumberOfColors];
};

class XSysInfo : public WindowingSystemInfo {
public:
  XSysInfo(Widget a) : area(a) { }

  Widget area;   // The Drawing Area widget
};

#endif

// EOF
