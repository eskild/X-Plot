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

#include "xdraw.h"

#include <Xm/RowColumn.h>
#include <Xm/Xm.h>
#include <Xm/PushB.h>
#include <Xm/MainW.h>
#include <Xm/SelectioB.h>
#include <Xm/CascadeB.h>
#include <Xm/Frame.h>
#include <Xm/FileSB.h>
#include <Xm/MessageB.h>
#include <Xm/ToggleBG.h>
#include <Xm/SeparatoG.h>
#include <Xm/Text.h>
#include <Xm/DrawingA.h>
#include <Xm/Label.h>

int XDrawHandler::LineWidth = 1;

static const char *ColorNames[NumberOfColors] = {
  "Black", "White",
  "Red", "Green", "Blue", "Orange", "Yellow", "Pink", "Magenta",
  "Cyan", "Brown", "LimeGreen", "Turquoise", "Violet", "Wheat",
  "Purple" };

XDrawHandler::XDrawHandler(pDisplayMapperBase map) : DrawHandler(map), gc(NULL)
{ 
  linestyle = solid;
}

XDrawHandler::~XDrawHandler()
{
  XFreeGC(display, gc);
}

DrawHandler& XDrawHandler::AttachToWindow(pBaseWindow pbw)
{
  area    = ((pXSysInfo)pbw->winsysinfo)->area;
  display = XtDisplay(area);
  window  = XtWindow(area);
  screen  = XtScreen(area);

  XGCValues gcv;
  gcv.foreground = XWhitePixelOfScreen(screen);
  gcv.background = XBlackPixelOfScreen(screen);
  gc = XCreateGC(display, RootWindowOfScreen(screen), GCForeground | GCBackground, &gcv);

  font = XLoadQueryFont(display, "fixed");
  fontheight = font->ascent + font->descent;

  XSetWindowBackground(display, window, XBlackPixelOfScreen(screen));

  Dimension width, height;
  XtVaGetValues(area, XmNwidth, &width, XmNheight, &height, NULL);

  mapper->SetScreenGeometry( DisplayPoint( width-1, height-1 ) );
  mapper->SetVirtualGeometry( Complex(-1,-1), Complex(1,1) );

  Colormap colormap = DefaultColormapOfScreen(screen);
  for (int i=0; i<NumberOfColors; i++) {
    XColor realrgb;
    if (!XAllocNamedColor(display, colormap, ColorNames[i], &colors[i], &realrgb))
      XtWarning("X-Plot: Couldn't allocate color.");
  }

  return *this;
}

DrawHandler& XDrawHandler::Clear()
{
  XClearWindow(display, window);   // does not generate expose event
  return *this;
}

DrawHandler& XDrawHandler::SelectColor(long index)
{
  DrawHandler::SelectColor(index);
  index = ( index % (NumberOfColors-2) )  +  2;  // Exclude Black and White
  XSetForeground(display, gc, colors[index].pixel);
  return *this;
}

DrawHandler& XDrawHandler::SelectColorBlack()
{
  DrawHandler::SelectColorBlack();
  XSetForeground(display, gc, colors[0].pixel);
  return *this;
}

DrawHandler& XDrawHandler::SelectColorWhite()
{
  DrawHandler::SelectColorWhite();
  XSetForeground(display, gc, colors[1].pixel);
  return *this;
}

DrawHandler& XDrawHandler::SelectBackgroundColor(BackgroundColor c)
{
  DrawHandler::SelectBackgroundColor(c);
  XSetWindowBackground(display, window, (c==black) ? XBlackPixelOfScreen(screen)
		                                   : XWhitePixelOfScreen(screen));
  return *this;
}

DrawHandler& XDrawHandler::SetLinestyle(Linestyle s)
{
  DrawHandler::SetLinestyle(s);
  XSetLineAttributes(display, gc, LineWidth, (s == solid) ? LineSolid : LineOnOffDash, CapButt, JoinMiter);
  return *this;
}

DrawHandler& XDrawHandler::SetDrawmode(Drawmode m)
{
  DrawHandler::SetDrawmode(m);
  XGCValues values;
  unsigned  long mask = GCFunction;

  switch (m) {
    case on      : values.function = GXcopy;		break;
    case off     : values.function = GXnoop;		break;
    case reverse : values.function = GXinvert;		break;
  }

  XChangeGC(display, gc, mask, &values);
  return *this;
}

DrawHandler& XDrawHandler::DrawCross(const Complex &centre)
{
  DisplayPoint c = Map(centre);
  XDrawLine(display, window, gc, c.x-5, c.y-5,  c.x+5, c.y+5);
  XDrawLine(display, window, gc, c.x+5, c.y-5,  c.x-5, c.y+5);
  return *this;
}

DrawHandler& XDrawHandler::DrawArrow(const Complex &p1, const Complex &p2)
{
  DisplayPoint from, to_centre, to_end;

  CalcArrowPoints(p1, p2, from, to_centre, to_end);

  XDrawLine(display, window, gc, from.x, from.y, to_centre.x, to_centre.y);
  XDrawLine(display, window, gc, to_centre.x, to_centre.y, to_end.x, to_end.y);

  return *this;
}

DrawHandler& XDrawHandler::DrawLine(const Complex &p1, const Complex &p2)
{
  DisplayPoint from = Map(p1);
  DisplayPoint to   = Map(p2);

  XDrawLine(display, window, gc, from.x, from.y, to.x, to.y);

  if (arrowstyle != none) DrawArrow(p1, p2);

  return *this;
}

static void MakeULandLR(DisplayPoint &c1, DisplayPoint &c2)
{
  DisplayPoint ul(Min(c1.x, c2.x), Min(c1.y, c2.y));
  DisplayPoint lr(Max(c1.x, c2.x), Max(c1.y, c2.y));
  c1 = ul;
  c2 = lr;
}

DrawHandler& XDrawHandler::DrawRectangle(const Complex &p1, const Complex &p2)
{  
  DisplayPoint d1 = Map(p1);
  DisplayPoint d2 = Map(p2);

  MakeULandLR(d1, d2);

  XDrawRectangle(display, window, gc, d1.x, d1.y, d2.x-d1.x, d2.y-d1.y);

  if (arrowstyle != none)
    DrawArrow(p1, Complex( ::real(p2), ::imag(p1) ) );

  return *this;
}

DrawHandler& XDrawHandler::DrawCircle(const Complex &c, const double &radius)
{
  Complex r               = Complex(radius, -radius);
  DisplayPoint upperleft  = Map( c - r );
  DisplayPoint lowerright = Map( c + r );
  int side_length         = lowerright.x - upperleft.x;
  XDrawArc(display, window, gc, upperleft.x, upperleft.y, side_length, side_length, 0, 360*64);

  if (arrowstyle != none)				// circles are ccw.
    DrawArrow(c+Complex(radius,radius), c+Complex(-radius,radius));

  return *this;
}

DrawHandler& XDrawHandler::DrawDot(const Complex &p)
{
  DisplayPoint c = Map(p);
  XDrawRectangle(display, window, gc, c.x-1, c.y-1, 3, 3);
  return *this;
}

DrawHandler& XDrawHandler::DrawText(int lineno, const CppStr& s)
{
  int x = 6;
  int y = (lineno+1)*(fontheight+4) + 6;
  XClearArea(display, window, x, y-fontheight, 0, fontheight + 2, False);
  XDrawString(display, window, gc, x, y, s.Ptr(), s.len()); 
  return *this;
}

DrawHandler& XDrawHandler::DrawText(const Complex &lo_left, const CppStr& s)
{
  DisplayPoint dp = Map(lo_left);
  XDrawString(display, window, gc, dp.x, dp.y, s.Ptr(), s.len()); 
  return *this;
}

DrawHandler& XDrawHandler::SetVirtualGeometry(const Complex l_l,
					      const Complex u_r)
{
  DrawHandler::SetVirtualGeometry(l_l, u_r);
  XClearArea(display, window, 0,0,0,0, True);    // Clears window and generates Expose event
  return *this;
}

DrawHandler& XDrawHandler::Centre(const Complex &c)
{
  DrawHandler::Centre(c);
  XClearArea(display, window, 0,0,0,0, True);
  return *this;
}

// EOF
