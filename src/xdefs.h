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

#ifndef __XDEFS_H__
#define __XDEFS_H__

#include "basewin.h"
#include "userdefs.h"
#include "stdfuncs.h"
#include "stdconst.h"
#include "parser.h"
#include "code.h"
#include "cppstr.h"

#include "actionarea.h"
#include "xdraw.h"
#include "psdraw.h"

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <assert.h>

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



//===============================================================================================
//===== CLASS DECLARATIONS ======================================================================
//===============================================================================================

//----- Classdef's ------------------------------------------------------------------------------

CLASSDEF(TheRootWindow);
CLASSDEF(CoordWindow);
CLASSDEF(DomainWindow);
CLASSDEF(RangeWindow);

CLASSDEF(RadioInfo);

CLASSDEF(TheScanner);



class TheScanner : public Scanner {
public:
  CppStr the_input_line;
protected:
  virtual CppStr GetScanLine();
};

class TheRootWindow {
public:
  TheRootWindow(Widget& TopLevel);
  virtual ~TheRootWindow();

  Widget Window();

  pCodeSegment GetMapCodeSegment();

  void ParseMapExpression(bool recalc);

  CppStr map_expression;   // holds the current map expression

private:
  void SetupMenu();
  
  pCodeSegment map_code;

  Widget window;
  Widget menu_bar;
};

class Xcommon {
public:
  Widget ShellWindow()    { return shell; }
protected:
  Xcommon() : shell(NULL), area(NULL) { }

  Widget shell;          // The Shell (ie. the window with the border etc.)
  Widget area;           // The Drawing Area
};

class XcommonDomainRange : public Xcommon {
public:
  virtual void SetShiftKeyState(bool)   = 0;
  virtual void SetControlKeyState(bool) = 0;
};

class CoordWindow : public BaseCoordWindow, public Xcommon {
public:
  CoordWindow(Widget parent);
  void Resize();
  virtual void Redraw(int);
};

class DomainWindow : public BaseDomainWindow, public XcommonDomainRange {
public:
  DomainWindow(Widget parent);

  virtual void Redraw(int);
  virtual void RubberCalcPoints();
  virtual void PointerInit(DisplayPoint dp);
  virtual void PointerTo_Range(DisplayPoint dp);

  virtual void DrawTextInCoordWindow(const CppStr&);

  virtual GraphicParamPlot* SelectParamPlot();

  void Resize();

  virtual void SetShiftKeyState(bool);
  virtual void SetControlKeyState(bool);

  void Busy(bool);
};


class RangeWindow : public BaseRangeWindow, public XcommonDomainRange {
public:
  RangeWindow(Widget parent);

  virtual void Redraw(int count);
  virtual void SetDrawMode(DrawMode);

  void Resize();

  virtual void DrawTextInCoordWindow(const CppStr&);

  virtual void PointerTo_Range(DisplayPoint dp);

  virtual void SetShiftKeyState(bool);
  virtual void SetControlKeyState(bool);
};

class RadioInfo {
public:
  RadioInfo()
    : shell(NULL), button_no(0)  { }
  RadioInfo(Widget sh, int button)
    : shell(sh), button_no(button) { }

  Widget         shell;
  int            button_no;
};


//===============================================================================================
//===== VARIOUS DEFINITIOINS ====================================================================
//===============================================================================================

//----- Useful X variables ----------------------------------------------------------------------

extern XtAppContext app_context;    // The Application Context

//----- Globals ---------------------------------------------------------------------------------

extern pTheScanner        scanner;
extern pTheRootWindow     root_win;
extern pCoordWindow       coord_win;
extern pDomainWindow      domain_win;
extern pRangeWindow       range_win;


extern pPSOptions opts;

void Tell(const CppStr s);

//----- Constants -------------------------------------------------------------------------------

const int zoom_factor = 2;

extern const char *UserDir;          // defaults to current dir.



void LoadLibrary(CppStr filename);
bool Confirm(const char *s, bool isQuestion = true);
void Tell(const CppStr s);
void TextInputDialog(char *titletext, char *infotext, CppStr *presult);
void FormulaDialog();
void DomainDialog();
void RangeDialog();
void PrintDialog();
void PrintSetupDialog();
bool ParamPlotDialog(CppStr&, double&, double&, int&);

#endif

// EOF
