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

#ifndef __GRAPHOBJ_H__
#define __GRAPHOBJ_H__

#include "defs.h"
#include "points.h"
#include "cppstr.h"
#include <Complex.h>
#include <iostream>

const int nTypesOfGraphicalObjects = 8;

CLASSDEF(BaseWindow);	// Actually this belongs in 'basewin.h'.

CLASSDEF(DisplayPoint);
CLASSDEF(DisplayMapperBase);
CLASSDEF(LinearDisplayMapper);
CLASSDEF(DrawHandler);
CLASSDEF(GraphicObjectBase);

CppStr FormatNumber(const Complex &);       // can't be 'const' on PC's
CppStr FormatNumber(const double  &);

class DisplayPoint {
public:
		DisplayPoint()				{ }
		DisplayPoint(int X, int Y) : x(X), y(Y)	{ }

	DisplayPoint&	operator += (const DisplayPoint &p)
				{ x += p.x;  y += p.y;  return *this; }

	DisplayPoint&	operator - (int d)
				{ x -= d;  y -= d;  return *this; }

	bool		operator != (const DisplayPoint &p)
				{ return bool(p.x != x  ||  p.y != y); }

	bool		operator == (const DisplayPoint &p)
				{ return bool(p.x == x  &&  p.y == y); }

  		int x, y;

	friend	ostream& operator << (ostream&, const rDisplayPoint);
        friend	istream& operator >> (istream&, rDisplayPoint);       
};

class DisplayMapperBase {
public:
			DisplayMapperBase();

	// The Get/SetScreenGeometry parameter is the highest coordinate in a (0,0)
	// based window system. The actual size in pixels is p.x + 1, p.y + 1.
	virtual	void	SetScreenGeometry(DisplayPoint p, int dpi=0);
	virtual	void	SetVirtualGeometry(const Complex lower_left,
					   const Complex upper_right);

	virtual	void	GetVirtualGeometry(Complex& l_l, Complex& u_r);
	virtual	void	GetScreenGeometry(DisplayPoint&);

	virtual	void		Centre(const Complex &)	= 0;
	virtual	DisplayPoint	Map(const Complex &)	= 0;
	virtual	Complex		Map(const DisplayPoint)	= 0;

		double		ArrowSegmentLength()	{ return arrow_segment_length; }

        // Return device coordinates for upper right and lower left corder of display, respectively:
	virtual DisplayPoint    GetUpperRightCornerOfDisplay() = 0;
        virtual DisplayPoint    GetLowerLeftCornerOfDisplay()  = 0;

	virtual	int             Cm_to_Dots(double cm);

protected:
	virtual	void		calc_internals()	= 0;

	DisplayPoint		grid;
	Complex			low_left, up_right;
	double			arrow_segment_length;  // length of one line in an arrow
	int                     dpi;                   // dots-per-inch on screen
};

class LinearDisplayMapper : public DisplayMapperBase {
public:
	virtual	void		Centre(const Complex &);
	virtual	DisplayPoint	Map(const Complex &);
	virtual	Complex		Map(const DisplayPoint);

	virtual DisplayPoint    GetUpperRightCornerOfDisplay();
        virtual DisplayPoint    GetLowerLeftCornerOfDisplay();

protected:
	virtual	void		calc_internals();

	double	step_x, step_y;
};

class LinearDisplayMapper2 : public LinearDisplayMapper {
public:
	virtual	void		Centre(const Complex &);
	virtual	DisplayPoint	Map(const Complex &);
	virtual	Complex		Map(const DisplayPoint);

	virtual DisplayPoint    GetUpperRightCornerOfDisplay();
        virtual DisplayPoint    GetLowerLeftCornerOfDisplay();
};

class DrawHandler {
public:
			DrawHandler(pDisplayMapperBase);
	virtual        ~DrawHandler();

	enum	Drawmode	{ on, off, reverse };
	enum	Linestyle	{ solid, dashed };
        enum	Arrowstyle	{ none, centred, endpoint };
	enum    BackgroundColor { black, white };  // the possible background colors

	virtual	DrawHandler&	AttachToWindow(pBaseWindow) = 0;

	virtual	DrawHandler&	Clear()	= 0;

	virtual	DrawHandler&	DrawCircle(const Complex &centre,
					   const double &radius) = 0;
	virtual	DrawHandler&	DrawLine(const Complex &p1, 
					 const Complex &p2) = 0;
	virtual	DrawHandler&	DrawRectangle(const Complex &p1,
					      const Complex &p2) = 0;
	virtual	DrawHandler&	DrawDot(const Complex &p) = 0;
	virtual	DrawHandler&	DrawText(int lineno, const CppStr& text) = 0;
	virtual	DrawHandler&	DrawText(const Complex &, const CppStr&) = 0;

	virtual	DrawHandler&	DrawCross(const Complex &centre) = 0;
	virtual	DrawHandler&	DrawArrow(const Complex &tangent_start,
					  const Complex &tangent_end) = 0;

	virtual	DrawHandler&	BeginCapture();		// Capture mouse
	virtual	DrawHandler&	EndCapture();		// Release mouse

	virtual	DrawHandler&	BeginDrawing();		// Called to initialize a drawing sequence
	virtual	DrawHandler&	EndDrawing();		// Called when a drawing seq. is done

	virtual	DrawHandler&	Busy(bool busy);	// Indicate that the program is busy if cTRUE

	virtual	DrawHandler&	SetDrawmode(Drawmode);
	virtual	Drawmode 	GetDrawmode();
	virtual	DrawHandler&	SetLinestyle(Linestyle);
	virtual	DrawHandler&	SelectColor(long);
	virtual	DrawHandler&	SelectColorBlack();
	virtual	DrawHandler&	SelectColorWhite();
	virtual DrawHandler&    SelectBackgroundColor(BackgroundColor);
	virtual	DrawHandler&	SelectArrowstyle(Arrowstyle);
	virtual	DrawHandler&	CalcArrowPoints(const Complex &p1,
						const Complex &p2,
						DisplayPoint &from,
						DisplayPoint &to_centre,
						DisplayPoint &to_end);

			// Note: SelectColor() can't select black and white.

	virtual	DrawHandler&	Centre(const Complex &);
		DisplayPoint	Map(const Complex &);
		Complex		Map(const DisplayPoint);

	virtual	DrawHandler&	SetScreenGeometry(DisplayPoint,int=0);
	virtual	DrawHandler&	SetVirtualGeometry(const Complex lower_left,
						   const Complex upper_right);
	virtual	DrawHandler&	GetVirtualGeometry(Complex& l_l, Complex& u_r);
	virtual	DrawHandler&	GetScreenGeometry(DisplayPoint&);
	virtual DisplayPoint    GetUpperRightCornerOfDisplay();
        virtual DisplayPoint    GetLowerLeftCornerOfDisplay();

        virtual int             Cm_to_Dots(double cm);
protected:
	Drawmode		drawmode;
	Linestyle		linestyle;
	Arrowstyle		arrowstyle;
	pDisplayMapperBase      mapper;
};

const int GR_LINE        = 0;
const int GR_CIRCLE      = 1;
const int GR_RECTANGLE   = 2;
const int GR_GRID        = 3;
const int GR_CONCCIRCLES = 4;
const int GR_ITERATION   = 5;
const int GR_PARAMPLOT   = 6;
const int GR_FREEHAND    = 7;

class GraphicObjectBase {
public:
			GraphicObjectBase(pDrawHandler *, int points);

	virtual	void	Draw();
	virtual	void	DrawToMap(rComplexPointsSegmentList,
				  pCodeSegment,
				  rErrorObj);

	virtual	void	RubberInit(DisplayPoint p);
	virtual	void	RubberTo(DisplayPoint p)		= 0;
	virtual	void	RubberEnd();
	virtual	CppStr	RubberInfo()				= 0;

        virtual int     IsA() = 0;

	virtual	void	GetExtent(Complex& lower_left,
				  Complex& upper_right)		= 0;

		void	SetPoints(int n);
		int	GetPoints();

                long	GetInstanceId()		{ return id; }

protected:
	enum	RubberState	{ no, first, not_first };

	pDrawHandler		*ppdrawhandler;	// Pointer to Pointer to Handler used to draw the object.
                                                // This way, when the creating window changes mapper, the
                                                // objects to as well.
	int			points;		// When mapping this object to a 
	                                        // ComplexPoints, use this many points
	RubberState		rubber;
	DrawHandler::Drawmode	temp_drawmode;

private:
	static	long	_id;		// each GraphicObjectBase instance gets unique id
	long		id;		// this is this object's id; the above is a counter
};

class GraphicLine : public GraphicObjectBase {
public:
			GraphicLine(pDrawHandler *, int points);

	virtual	void	Draw();
	virtual	void	DrawToMap(rComplexPointsSegmentList, pCodeSegment, rErrorObj);

	virtual	void	RubberInit(DisplayPoint p);
	virtual	void	RubberTo(DisplayPoint p);
	virtual	CppStr	RubberInfo();

		void	SetValues(Complex &p1, Complex &p2);
	virtual	void	GetExtent(Complex& lower_left, Complex& upper_right);		

	virtual int     IsA() { return GR_LINE; }

protected:
	Complex		p1, p2;
};

class GraphicCircle : public GraphicObjectBase {
public:
			GraphicCircle(pDrawHandler *, int points);

	virtual	void	Draw();
	virtual	void	DrawToMap(rComplexPointsSegmentList, pCodeSegment, rErrorObj);

	virtual	void	RubberInit(DisplayPoint p);
	virtual	void	RubberTo(DisplayPoint p);
	virtual	CppStr	RubberInfo();

	virtual	void	GetExtent(Complex& lower_left, Complex& upper_right);
		void	SetValues(Complex &c, double &r);

	virtual int     IsA() { return GR_CIRCLE; }
protected:
	Complex		centre;
	double		radius;
};

class GraphicConcentricCircles : public GraphicObjectBase {
public:
			GraphicConcentricCircles(pDrawHandler *, int points, double delta_radius);

	virtual	void	Draw();
	virtual	void	DrawToMap(rComplexPointsSegmentList, pCodeSegment, rErrorObj);

	virtual	void	RubberInit(DisplayPoint p);
	virtual	void	RubberTo(DisplayPoint p);
	virtual	CppStr	RubberInfo();

	virtual	void	GetExtent(Complex& lower_left, Complex& upper_right);
		void	SetValues(Complex &c, double &r, double &dr);

	virtual int     IsA() { return GR_CONCCIRCLES; }
protected:
	Complex		centre;
	double		radius;
	double		delta_radius;
};

class GraphicRectangle : public GraphicObjectBase {
public:
			GraphicRectangle(pDrawHandler *, int points);

	virtual	void	Draw();
	virtual	void	DrawToMap(rComplexPointsSegmentList, pCodeSegment, rErrorObj);

	virtual	void	RubberInit(DisplayPoint p);
	virtual	void	RubberTo(DisplayPoint p);
	virtual	CppStr	RubberInfo();

	virtual	void	GetExtent(Complex& lower_left, Complex& upper_right);
		void	SetValues(Complex &_p1, Complex &_p2);

	virtual int     IsA() { return GR_RECTANGLE; }
protected:
	Complex		p1, p2;		// diagonal corners
};


class GraphicGrid : public GraphicObjectBase {
public:
			GraphicGrid(pDrawHandler *, int points, double griddistx, double griddisty);

	virtual	void	Draw();
	virtual	void	DrawToMap(rComplexPointsSegmentList, pCodeSegment, rErrorObj);

	virtual	void	RubberInit(DisplayPoint p);
	virtual	void	RubberTo(DisplayPoint p);
	virtual	CppStr	RubberInfo();

	virtual	void	GetExtent(Complex& lower_left, Complex& upper_right);
		void	SetValues(Complex &_p1, Complex &_p2, double &_dx, double &_dy);

	virtual int     IsA() { return GR_GRID; }
protected:
	Complex		p1, p2;
	double		dist_x, dist_y;
};

/* GraphicPointsBase is abstract; these methods need implementation in
 * derived classes:
 *
 *  RubberTo()
 *  RubberInfo()
 *  IsA()
 */

class GraphicPointsBase : public GraphicObjectBase {
public:
  virtual ~GraphicPointsBase() { }
  enum PointsDrawmode { edm_points, edm_directedlines, edm_lines };

  virtual void  DrawAsPoints(PointsDrawmode, int upper = -1);
  
  virtual void GetExtent(Complex& lower_left, Complex& upper_right);
protected:
  GraphicPointsBase(pDrawHandler *, int);
  Complex extent_ll, extent_ur;
  ComplexPointsSegment pts;
};

class GraphicIteration : public GraphicPointsBase {
public:
  GraphicIteration(pDrawHandler *, int);

  virtual void Draw();

  virtual void DrawToMap(rComplexPointsSegmentList, pCodeSegment, rErrorObj);

  virtual void RubberInit(DisplayPoint p);
  virtual void RubberTo(DisplayPoint p);
  virtual CppStr RubberInfo();

  virtual int IsA() { return GR_ITERATION; }
protected:
  Complex origin;
};

class GraphicParamPlot : public GraphicPointsBase {
public:
  GraphicParamPlot(pDrawHandler *, int, pCodeSegment, double, double, rErrorObj);

  virtual void Draw();

  virtual void DrawToMap(rComplexPointsSegmentList, pCodeSegment, rErrorObj);

  virtual void RubberTo(DisplayPoint p);
  virtual CppStr RubberInfo();

  virtual int IsA() { return GR_PARAMPLOT; }
protected:
  Complex origin;
};

class GraphicFreehand : public GraphicPointsBase {
public:
  GraphicFreehand(pDrawHandler *);

  void SetDrawmodeForDraw(PointsDrawmode);  // drawmode to use for calls to Draw()

  virtual void Draw();

  virtual void DrawToMap(rComplexPointsSegmentList, pCodeSegment, rErrorObj);

  virtual void RubberInit(DisplayPoint p);
  virtual void RubberTo(DisplayPoint p);
  virtual void RubberEnd();
  virtual CppStr RubberInfo();

  virtual int IsA() { return GR_FREEHAND; }
protected:
  int last_used;     // index of last used point in GraphicPointsBase::pts
  Complex current;   // mose recent point given to RubberInit() or RubberTo()
  PointsDrawmode drawmode;
};

#endif

// EOF
