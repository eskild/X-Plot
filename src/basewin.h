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

#ifndef __BASEWIN_H__
#define __BASEWIN_H__

#include "defs.h"
#include "graphobj.h"
#include "dynaarra.h"

typedef DynaArray<pGraphicObjectBase>	apGraphObj;	// array of pointers

// CLASSDEF(BaseWindow) is in graphobj.h
CLASSDEF(BaseGraphWindow);
CLASSDEF(BaseDomainWindow);
CLASSDEF(BaseRangeWindow);
CLASSDEF(BaseCoordWindow);
CLASSDEF(WindowingSystemInfo);
CLASSDEF(SuperBox);

// Whenever porting to a new Windowing System (MS Windows, X etc.) a class
// derived from WindowingSystemInfo should be constructed and set up in the
// class constructors of classes derived from BaseWindow. BaseWindow takes
// care of destroying the WindowingSystemInfo object. 
class WindowingSystemInfo {
public:
};

// Calculate the size of a rectangular box containing a number of points.
// The result is returned in the format: Lower left corner of box, Upper
// right corner of the box.
class SuperBox {
public:
			SuperBox(const Complex &first);
			SuperBox();

	SuperBox&	AddPoint(const Complex &point);
	SuperBox&	GetResult(Complex &ll, Complex &ur);
private:
	Complex	lower_left, upper_right;
	bool	valid;
};

class BaseWindow {
public:
			BaseWindow() : drawhandler(0)	{ }
			BaseWindow(pDrawHandler);
	virtual	       ~BaseWindow();

	virtual	void	Redraw()	= 0;	// Redraw window
	virtual	void	Clear()		= 0;	// Clear data and window

	virtual	void	ClearWindow();		// Clears window but not data

	virtual	void	Busy();
        virtual	void	NotBusy();

	virtual	void	GetScreenGeometry(DisplayPoint&);
	virtual	void	SetScreenGeometry(DisplayPoint, int=0);

	pWindowingSystemInfo	winsysinfo;

	virtual	pDrawHandler	SetDrawHandler(pDrawHandler the_new);

protected:
	pDrawHandler		drawhandler;
};

class BaseCoordWindow : public BaseWindow {
public:
			BaseCoordWindow(pDrawHandler);

	virtual	void	Redraw();
	virtual	void	Clear();		

	virtual	void	DomainText(const CppStr&);
	virtual	void	RangeText (const CppStr&);
protected:
	CppStr		domain, range;
};

class BaseGraphWindow : public BaseWindow {
public:
			BaseGraphWindow(pDrawHandler);

	virtual	void	AutoScale()	= 0;	// Fit contents in window
	virtual void    Redraw();

	virtual	void	ZoomIn();
	virtual	void	ZoomOut();

                void    SetZoomFactor(int newfactor);
	virtual void    SetIdleState();
	virtual void    SetZoomState();         // Do Zoom Area next time mouse is active
	virtual void    SetRubberState();       // Do Rubber Band next time mouse is active
	virtual void    SetShiftState(bool);    // If cTRUE, do Centre at next mouse press
	virtual void    SetControlState(bool);  // If cTRUE, do Zoom In/Out at next mouse press
                bool    IsIdleState();
	        bool    IsZoomState();
		bool    IsRubberState();
		bool    IsShiftState();
                bool    IsControlState();

	virtual	void	PointerInit(DisplayPoint);
	virtual	void	PointerTo(DisplayPoint);
	virtual	void	PointerEnd();
	virtual void    PointerTo_Range(const DisplayPoint) = 0;  // Moves Cursor in Range window
	virtual void    PointerRightInit(const DisplayPoint);

	virtual	void	Centre(const Complex&);	// Centre contents around the coord. Maintain scales.

        virtual	void	DrawAxis();
	virtual void    DrawCoords();

	virtual void    DrawTextInCoordWindow(const CppStr&) = 0;

	virtual	void	SetVirtualGeometry(const Complex lower_left,
					   const Complex upper_right);
	virtual	void	GetVirtualGeometry(Complex& l_l, Complex& u_r);

		bool    GetCoordsMode();
		void    SetCoordsMode(bool on);
		bool    GetAxisMode();
                void    SetAxisMode(bool on);
		bool    GetDirectedMode();
		void    SetDirectedMode(bool on);
protected:
	virtual	void	ZoomAreaInit(DisplayPoint);
	virtual	void	ZoomAreaTo(DisplayPoint);
	virtual	void	ZoomAreaEnd();

	enum		{ doing_rubber, doing_zoom, doing_shift, doing_control, idle }	state;
	Complex		zoom_corner1, zoom_corner2;	// corners of zoom box
	bool            draw_coords;
	bool            draw_axis;
	bool		draw_directed;			// draw directed objects?
private:
	virtual	void	RubberInit(DisplayPoint);
	virtual	void	RubberTo(DisplayPoint);
	virtual	void	RubberEnd();

        int             zoom_factor;
};

class BaseDomainWindow : public BaseGraphWindow {
public:
        enum	DrawMode { draw_freehand=0,
			   draw_parameterplot,
			   draw_iteratepoint,
			   draw_lines,
			   draw_rectangle,
			   draw_grid,
			   draw_circle,
			   draw_concentriccircles };
	// Note! the ordering SHOULD BE THE SAME AS THE ORDERING OF THE 'DOMAIN' DIALOG BOX
	//       (from top and down).

        enum    DeltaType { delta_grid_x, delta_grid_y, delta_concentric_circles };
	// Note! delta_concentric_circles should ALWAYS BE THE LAST ENTRY in the enumeration!

			BaseDomainWindow(pDrawHandler);
		       ~BaseDomainWindow();

	virtual	void	Redraw();	// Redraw entire area using 'elements'
	virtual	void	Clear();
	virtual	void	AutoScale();

	DrawMode        GetDrawMode();
	void            SetDrawMode(DrawMode newmode);

	void            SetPointsPlotDrawMode(bool draw_as_points);
	bool            GetPointsPlotDrawMode();

	virtual GraphicParamPlot* SelectParamPlot() = 0;

	int             NoOfPoints(DrawMode type, int newvalue = 0);

	double          DeltaValue(DeltaType type, double newvalue = 0.0);

	pGraphicObjectBase GetFirst();
	pGraphicObjectBase GetNext();

protected:
	virtual	void	RubberInit(DisplayPoint);
	virtual	void	RubberTo(DisplayPoint);
	virtual	void	RubberEnd();
	virtual void    RubberCalcPoints() = 0;  // Called by RubberEnd() to calc new points

	DrawMode	        drawmode;
	bool                    points_drawmode;

	double                  paramplot_tfrom, paramplot_tto;
	pCodeSegment            pparamplot_code;

        DynaArray<int>          aPoints;        // Number of points for graphical objects
	DynaArray<double>       aDelta;         // Delta values for graphical objects

	apGraphObj		elements;
	DynaIdx			n_elements;	// Number of elements

	DynaIdx                 next;           // Used by GetFirst() and GetNext()

	pGraphicObjectBase	rubber_obj;	// object being "rubbered"
};

class BaseRangeWindow : public BaseGraphWindow {
public:
			BaseRangeWindow(pDrawHandler);
		       ~BaseRangeWindow();

	enum DrawMode { draw_points, draw_lines, draw_directed_lines };

	virtual	void	Redraw();
	virtual	void	Clear();
	virtual	void	AutoScale();

	virtual	void	ClearWindow();		// Clears window but not data

	virtual	void	SetCursor(const Complex&);
	virtual	void	RemoveCursor();

	virtual	void	AddPoints(pComplexPointsSegmentList);

	virtual	void	SetScreenGeometry(DisplayPoint, int=0);
	virtual	void	SetVirtualGeometry(const Complex lower_left,
					   const Complex upper_right);

	virtual void     SetDrawMode(DrawMode newmode);
	virtual DrawMode GetDrawMode();
protected:
	virtual	void	DrawASegmentList(pComplexPointsSegmentList);

	DynaArray<pComplexPointsSegmentList>	pointslist;

	Complex					old_cursor;
	bool					old_cursor_valid;

	DrawMode                                drawmode;
};

#endif

// EOF
