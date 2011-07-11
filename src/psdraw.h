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

#ifndef __PSDRAW_H__
#define __PSDRAW_H__

#include "graphobj.h"
#include "basewin.h"
#include <fstream>

CLASSDEF(PSOptions);
CLASSDEF(PSDrawHandler);

class PSOptions {
public:
	PSOptions();
	double  dpi;                          // dots per inch, defaults to 72
	double	paper_width, paper_height;    // all dimensions are in inches
	double  margin_top, margin_bottom, margin_left, margin_right, margin_windows;
	CppStr	command_prefix;
	CppStr	ps_coord_setup;
	CppStr	outputfilename;		      // if "" then use temp file

        double  In_to_Cm(double d)      { return d*2.54; }
	double  Cm_to_In(double d)      { return d/2.54; }
	double  In_to_Dots(double d)    { return d*(double)dpi;  }
	double  Cm_to_Dots(double d)    { return In_to_Dots(Cm_to_In(d)); }

	double  Origo_X(bool domain);
	double  Origo_Y();
	double  WindowSideLength(bool domain);

private:
	double  work_height;
	double  work_width;
	double  domain_side_length;
	double  range_side_length;
};

class RGB_Info {
public:
  RGB_Info() : red(0.0), green(0.0), blue(0.0) {}
  RGB_Info(double r, double g, double b) : red(r), green(g), blue(b) {}

  double red, green, blue;

  // All values are range [0.0;1.0]
};

#define MAX_PS_COLORS 10

class PSDrawHandler : public DrawHandler {
public:
		PSDrawHandler(bool this_handles_the_domain_window, rPSOptions opts,
			      const CppStr &creator, const CppStr &text);
  virtual      ~PSDrawHandler();

  // New functions:
		ofstream*	CreateFile();
		PSDrawHandler&	CloseFile();
		PSDrawHandler&	AttachToFile(ofstream*);

		bool		Good();		// cTRUE while things are ok
  virtual	PSDrawHandler&	CreatePrologue();
  virtual	PSDrawHandler&	CreateEpilogue();
  virtual	PSDrawHandler&	SetLocation();
  virtual       PSDrawHandler&  ResetLocation();

  // Unused functions:
  virtual	DrawHandler&	AttachToWindow(pBaseWindow);
  virtual	DrawHandler&	Clear();
  virtual       DrawHandler&    Centre(const Complex&);

  // Used functions:
  virtual	DrawHandler&	DrawCircle(const Complex &centre,
					   const double& radius);
  virtual	DrawHandler&	DrawLine(const Complex& p1, const Complex& p2);
  virtual	DrawHandler&	DrawRectangle(const Complex& p1,
					      const Complex& p2);
  virtual	DrawHandler&	DrawDot(const Complex &p);
  virtual	DrawHandler&	DrawCross(const Complex& centre);
  virtual	DrawHandler&	DrawArrow(const Complex &tangent_start,
					  const Complex &tangent_end);
  virtual	DrawHandler&	DrawText(int lineno, const CppStr& text);
  virtual	DrawHandler&	DrawText(const Complex &, const CppStr&);
  virtual	DrawHandler&	SetLinestyle(Linestyle);
  virtual	DrawHandler&	SelectColor(long);
  virtual	DrawHandler&	SelectColorBlack();
  virtual	DrawHandler&	SelectColorWhite();

  virtual	DrawHandler&	BeginDrawing();		// Called to initialize a drawing sequence
  virtual	DrawHandler&	EndDrawing();		// Called when a drawing seq. is done

        static  bool use_color;

protected:
		PSDrawHandler&	ps_moveto(const DisplayPoint &p);
		PSDrawHandler&	ps_lineto(const DisplayPoint &p);
		PSDrawHandler&	ps_rlineto(const DisplayPoint &p);
		PSDrawHandler&	ps_moveto_lineto(const DisplayPoint &p1, const DisplayPoint &p2);
		PSDrawHandler&	ps_stroke();
		PSDrawHandler&	ps_newpath();
		PSDrawHandler&	ps_closepath();

		PSDrawHandler&  ReformatText();

  ofstream	*psfile;
  DisplayPoint	last_coord;
  bool		domain;		// cTRUE if this relates to the domain window
  rPSOptions	opts;
  CppStr        text;                         // text descrbing the plot
  CppStr        creator;
  RGB_Info      rgb_info[MAX_PS_COLORS];
};

#endif

// EOF
