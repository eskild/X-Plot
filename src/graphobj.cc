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

#include "graphobj.h"
#include "basewin.h"
#include <stdio.h>
#include <float.h>

CppStr FormatNumber(const Complex &c)
{
  char s[1024];
//  sprintf(s, "%0.4f%+0.4fi", ::real(c), ::imag(c));
  sprintf(s, "(%0.4f,%0.4f)", ::real(c), ::imag(c));
  return CppStr(s);
}

CppStr FormatNumber(const double &d)
{
  char s[1024];
  sprintf(s, "%0.4f", d);
  return CppStr(s);
}

//****************************************************************************
// DisplayPoint
//
// Features:
//
//
//
//
//
//
//****************************************************************************

ostream& operator << (ostream& os, const rDisplayPoint p)
{
  return os << '(' << p.x << ',' << p.y << ')';
}

istream& operator >> (istream& is, rDisplayPoint p)
	{
  char ch = 0;
  is >> ch;
  assert(ch == '(');
  is >> p.x;
  assert(is.good());
  is >> ch;
  assert(ch == ',');
  is >> p.y;
  assert(is.good());
  return is;
}    

//****************************************************************************
// DrawHandler
//
// Features:
//
//
//
//
//
//
//****************************************************************************

DrawHandler::DrawHandler(pDisplayMapperBase m) : drawmode(off),
	linestyle(solid), arrowstyle(none), mapper(m)
{
  assert(mapper != 0);
}

DrawHandler::~DrawHandler()
{
}

DrawHandler& DrawHandler::SetDrawmode(Drawmode m)
{
  drawmode = m;
  return *this;
}

DrawHandler::Drawmode DrawHandler::GetDrawmode()
{
  return drawmode;
}

DisplayPoint DrawHandler::Map(const Complex &c)
{
  return mapper->Map(c);
}

Complex DrawHandler::Map(DisplayPoint p)
{
  return mapper->Map(p);
}

DrawHandler& DrawHandler::Centre(const Complex &c)
{
  mapper->Centre(c);
  return *this;
}

DrawHandler& DrawHandler::SetScreenGeometry(DisplayPoint dp, int dpi)
{
  mapper->SetScreenGeometry(dp,dpi);
  return *this;
}

DrawHandler& DrawHandler::GetScreenGeometry(DisplayPoint &dp)
{
  mapper->GetScreenGeometry(dp);
  return *this;
}

DrawHandler& DrawHandler::SetVirtualGeometry(const Complex lower_left,
					     const Complex upper_right)
{
  mapper->SetVirtualGeometry(lower_left, upper_right);
  return *this;
}

DrawHandler& DrawHandler::GetVirtualGeometry(Complex& l_l, Complex& u_r)
{
  mapper->GetVirtualGeometry(l_l, u_r);
  return *this;
}

DisplayPoint DrawHandler::GetLowerLeftCornerOfDisplay()
{
  return mapper->GetLowerLeftCornerOfDisplay();
}

DisplayPoint DrawHandler::GetUpperRightCornerOfDisplay()
{
  return mapper->GetUpperRightCornerOfDisplay();
}

int DrawHandler::Cm_to_Dots(double cm)
{
  return mapper->Cm_to_Dots(cm);
}

DrawHandler& DrawHandler::Busy(bool)
{
  return *this;
}

DrawHandler& DrawHandler::SetLinestyle(Linestyle s)
{
  linestyle = s;
  return *this;
}

DrawHandler& DrawHandler::SelectColor(long)
{
  return *this;
}

DrawHandler& DrawHandler::SelectColorBlack()
{
  return *this;
}

DrawHandler& DrawHandler::SelectColorWhite()
{
  return *this;
}

DrawHandler& DrawHandler::SelectBackgroundColor(BackgroundColor)
{
  return *this;
}

DrawHandler& DrawHandler::SelectArrowstyle(Arrowstyle a)
{
  arrowstyle = a;
  return *this;
}

DrawHandler& DrawHandler::CalcArrowPoints(const Complex &p1, const Complex &p2,
					  DisplayPoint &from,
					  DisplayPoint &to_centre,
					  DisplayPoint &to_end)
{
  if (p1 != p2) {  // only arrow if there is a line!
    Complex BA = p2-p1;
    double  argBA = arg(BA);
    Complex C = (arrowstyle == centred) ? p1 + BA/2.0 : p2;	  // common point of arrow lines
    Complex D = C - polar(mapper->ArrowSegmentLength(), -M_PI_4 + argBA);
    Complex E = C - polar(mapper->ArrowSegmentLength(),  M_PI_4 + argBA);

    from      = Map(D);
    to_centre = Map(C);
    to_end    = Map(E);
  }
  else
    from = to_centre = to_end = Map(p1);

  return *this;
}

DrawHandler& DrawHandler::BeginCapture()
{
  return *this;
}

DrawHandler& DrawHandler::EndCapture()
{
  return *this;
}

DrawHandler& DrawHandler::BeginDrawing()
{
  return *this;
}

DrawHandler& DrawHandler::EndDrawing()
{
  return *this;
}

//****************************************************************************
// GraphicObjectBase
//
// Features:
//
//
//
//
//
//
//****************************************************************************

long GraphicObjectBase::_id = 0L;

GraphicObjectBase::GraphicObjectBase(pDrawHandler *h, int p)
  : ppdrawhandler(h), points(p), rubber(no)
{
  id = _id ++;
}

void GraphicObjectBase::SetPoints(int n)
{
  assert(n > 1);
  points = n;
}

int GraphicObjectBase::GetPoints()
{
  return points;
}

void GraphicObjectBase::RubberInit(DisplayPoint)
{
  rubber = first;
  temp_drawmode = (*ppdrawhandler)->GetDrawmode();
  (*ppdrawhandler)->SetDrawmode(DrawHandler::reverse).BeginCapture();
}

void GraphicObjectBase::RubberTo(DisplayPoint)
{
  if (rubber == not_first) Draw(); else rubber = not_first;
  // Doesn't undraw on first call
}

void GraphicObjectBase::RubberEnd()
{
  rubber = no;
  (*ppdrawhandler)->SetDrawmode(DrawHandler::on);
  Draw();
  (*ppdrawhandler)->SetDrawmode(temp_drawmode).EndCapture();
}

void GraphicObjectBase::Draw()
{
  (*ppdrawhandler)->SelectColor(GetInstanceId()).BeginDrawing();
}

void GraphicObjectBase::DrawToMap(rComplexPointsSegmentList pts, pCodeSegment, rErrorObj)
{
  pts.segmentlist.Empty();
  pts.color = GetInstanceId();
}

//****************************************************************************
// GraphicLine
//
// Features:
//
//
//
//
//
//
//****************************************************************************

GraphicLine::GraphicLine(pDrawHandler *h, int p) : GraphicObjectBase(h, p)
{
}

void GraphicLine::Draw()
{
  GraphicObjectBase::Draw();
  (*ppdrawhandler)->DrawLine( p1, p2 ).EndDrawing();
}

void GraphicLine::DrawToMap(rComplexPointsSegmentList pts, pCodeSegment code, rErrorObj err)
{
  GraphicObjectBase::DrawToMap(pts, code, err);
  pts.segmentlist.Grow(1);
  pts.segmentlist[0].Grow(points);
  Complex step = (p2 - p1) / Complex(points-1,0);	// points-1 to make line of correct length
  for (int i=0; i<points; i++)
    pts.segmentlist[0][i] = code->Run(p1 + step * Complex(i,0), err);
}

void GraphicLine::RubberInit(DisplayPoint p)
{
  GraphicObjectBase::RubberInit(p);
  p1 = p2 = (*ppdrawhandler)->Map(p);
}

void GraphicLine::RubberTo(DisplayPoint p)
{
  GraphicObjectBase::RubberTo(p);
  p2 = (*ppdrawhandler)->Map(p);
  Draw();
}

CppStr GraphicLine::RubberInfo()
{
  return FormatNumber(p1) + " -> " + FormatNumber(p2);
}

void GraphicLine::SetValues(Complex &_p1, Complex &_p2)
{
  p1 = _p1;
  p2 = _p2;
}

void GraphicLine::GetExtent(Complex& lower_left, Complex& upper_right)
{
  lower_left  = p1;
  upper_right = p2; 
}

//****************************************************************************
// GraphicCircle
//
// Features:
//
//
//
//
//
//
//****************************************************************************

GraphicCircle::GraphicCircle(pDrawHandler *h, int p) : GraphicObjectBase(h, p)
{
}

void GraphicCircle::Draw()
{
  GraphicObjectBase::Draw();
  (*ppdrawhandler)->DrawCircle( centre, radius ).EndDrawing();
}

void GraphicCircle::DrawToMap(rComplexPointsSegmentList pts, pCodeSegment code, rErrorObj err)
{
  GraphicObjectBase::DrawToMap(pts, code, err);
  pts.segmentlist.Empty();  pts.segmentlist.Grow(1);
  pts.segmentlist[0].Grow(points);
  Complex theta(0, (2*M_PI) / (points-1));	// points-1 to close circle
  for (int i=0; i<points; i++)
    pts.segmentlist[0][i] = code->Run(centre + radius * exp(Complex(i,0)*theta), err);
}

void GraphicCircle::RubberInit(DisplayPoint p)
{
  GraphicObjectBase::RubberInit(p);
  centre = (*ppdrawhandler)->Map(p);
  radius = 0;
}

void GraphicCircle::RubberTo(DisplayPoint p)
{
  GraphicObjectBase::RubberTo(p);
  radius = sqrt( norm( centre - (*ppdrawhandler)->Map(p) ) );
  Draw();
}

CppStr GraphicCircle::RubberInfo()
{
  return CppStr("Centre=") + FormatNumber(centre) + "; Radius=" + FormatNumber(radius);
}

void GraphicCircle::SetValues(Complex &_c, double &_r)
{
  centre = _c;
  radius = _r;
}

void GraphicCircle::GetExtent(Complex& lower_left, Complex& upper_right)
{
  lower_left  = centre - Complex(radius, radius);
  upper_right = centre + Complex(radius, radius);
}

//****************************************************************************
// GraphicConcentricCircles
//
// Features:
//
//
//
//
//
//
//****************************************************************************

GraphicConcentricCircles::GraphicConcentricCircles(pDrawHandler *h, int p, double dr)
  : GraphicObjectBase(h, p), delta_radius(dr)
{
  assert(dr > 0.0);
}

void GraphicConcentricCircles::Draw()
{
  GraphicObjectBase::Draw();
  int n_circles = int(radius / delta_radius);
  double r;
  for (int i=1; i<n_circles+1; i++) {
    r = delta_radius * i;
    (*ppdrawhandler)->DrawCircle( centre, r ).EndDrawing();
  }
}

void GraphicConcentricCircles::DrawToMap(rComplexPointsSegmentList pts, pCodeSegment code, rErrorObj err)
{
  GraphicObjectBase::DrawToMap(pts, code, err);

  int n_circles = int(radius / delta_radius);
  int i;

  pts.segmentlist.Grow(n_circles);
  for (i=0; i<n_circles; i++) pts.segmentlist[i].Grow(points);

  Complex theta(0, (2*M_PI) / (points-1));	// points-1 to close circle

  Complex *exp_list = new Complex[points];
  assert(exp_list != 0);
  for (i=0; i<points; i++)
	  exp_list[i] = exp(Complex(i,0)*theta);

  double r;
  for (i=0; i<n_circles; i++) {
    r = delta_radius * (i + 1);
    for (int j=0; j<points; j++)
      pts.segmentlist[i][j] = code->Run(centre + r * exp_list[j], err);
  }

  delete [] exp_list;
}

void GraphicConcentricCircles::RubberInit(DisplayPoint p)
{
  GraphicObjectBase::RubberInit(p);
  centre = (*ppdrawhandler)->Map(p);
  radius = 0;
}

void GraphicConcentricCircles::RubberTo(DisplayPoint p)
{
  GraphicObjectBase::RubberTo(p);
  radius = sqrt( norm( centre - (*ppdrawhandler)->Map(p) ) );
  Draw();
}

CppStr GraphicConcentricCircles::RubberInfo()
{
  return CppStr("Centre=") + FormatNumber(centre) + "; Radius=" + FormatNumber(radius);
}

void GraphicConcentricCircles::SetValues(Complex &_c, double &_r, double &_dr)
{
  centre       = _c;
  assert(_r > 0.0);
  radius       = _r;
  assert(_dr > 0.0);
  delta_radius = _dr;
}

void GraphicConcentricCircles::GetExtent(Complex& lower_left, Complex& upper_right)
{
  int n_circles = int(radius / delta_radius);

  Complex delta(delta_radius, delta_radius);

  delta *= n_circles;

  lower_left  = centre - delta; 
  upper_right = centre + delta; 
}

//****************************************************************************
// GraphicRectangle
//
// Features:
//
//
//
//
//
//
//****************************************************************************

GraphicRectangle::GraphicRectangle(pDrawHandler *h, int p) : GraphicObjectBase(h, p)
{
}

void GraphicRectangle::Draw()
{
  GraphicObjectBase::Draw();
  (*ppdrawhandler)->DrawRectangle( p1, p2 ).EndDrawing();
}

void GraphicRectangle::DrawToMap(rComplexPointsSegmentList pts, pCodeSegment code, rErrorObj err)
{
  GraphicObjectBase::DrawToMap(pts, code, err);
  pts.segmentlist.Grow(4);
  int i;
  for (i=0; i<4; i++) pts.segmentlist[i].Grow(points);
  Complex delta = p2 - p1;
  double deltax =  ::real(delta);
  double deltay =  ::imag(delta);
  double stepx  = deltax / (points-1);
  double stepy  = deltay / (points-1);
  Complex upperleft  = p1;
  Complex upperright = p1 + deltax;
  Complex lowerright = p2;
  Complex lowerleft  = p1 + Complex(0, deltay);
  for (i=0; i<points; i++) pts.segmentlist[0][i] = code->Run(upperleft  + (stepx * i), err);
  for (i=0; i<points; i++) pts.segmentlist[1][i] = code->Run(upperright + Complex(0, stepy * i), err);
  for (i=0; i<points; i++) pts.segmentlist[2][i] = code->Run(lowerright + (stepx * -i), err);
  for (i=0; i<points; i++) pts.segmentlist[3][i] = code->Run(lowerleft  + Complex(0, stepy * -i), err);
}

void GraphicRectangle::RubberInit(DisplayPoint p)
{
  GraphicObjectBase::RubberInit(p);
  p1 = p2 = (*ppdrawhandler)->Map(p);
}

void GraphicRectangle::RubberTo(DisplayPoint p)
{
  GraphicObjectBase::RubberTo(p);
  p2 = (*ppdrawhandler)->Map(p);
  Draw();
}

CppStr GraphicRectangle::RubberInfo()
{
  return CppStr("Corner A=") + FormatNumber(p1) + "; Corner B=" + FormatNumber(p2);
}

void GraphicRectangle::SetValues(Complex &_p1, Complex &_p2)
{
  p1 = _p1;
  p2 = _p2;
}

void GraphicRectangle::GetExtent(Complex& lower_left, Complex& upper_right)
{
  lower_left  = p1;
  upper_right = p2; 
}

//****************************************************************************
// GraphicGrid
//
// Features:
//
//
//
//
//
//
//****************************************************************************

GraphicGrid::GraphicGrid(pDrawHandler *h, int p, double dx, double dy)
  : GraphicObjectBase(h, p), dist_x(dx), dist_y(dy)
{
  assert(dist_x > 0.0);
  assert(dist_y > 0.0);
}

void GraphicGrid::Draw()
{
  GraphicObjectBase::Draw();

  Complex delta = p2 - p1;
  double delta_x = ::real(delta);
  double delta_y = ::imag(delta);

  double step_x  = (delta_x < 0.0) ? -dist_x : dist_x;
  double step_y  = (delta_y < 0.0) ? -dist_y : dist_y;

  int lines_x    = (int)Abs(delta_x / dist_x) + 1;
  int lines_y    = (int)Abs(delta_y / dist_y) + 1;
  int i;

  // Horizontal lines:
  for (i=0; i<lines_y; i++) {
    Complex left  = p1 + Complex(0, step_y*i);
    Complex right = Complex( ::real(p2),  ::imag(left));
    (*ppdrawhandler)->DrawLine(left, right);
  }

  // Vertical lines:
  for (i=0; i<lines_x; i++) {
    Complex top = p1 + step_x*i;
    Complex bot = Complex( ::real(top),  ::imag(p2));
    (*ppdrawhandler)->DrawLine(top, bot);
  }

  (*ppdrawhandler)->EndDrawing();
}

void GraphicGrid::DrawToMap(rComplexPointsSegmentList pts, pCodeSegment code, rErrorObj err)
{
  GraphicObjectBase::DrawToMap(pts, code, err);

  Complex delta = p2 - p1;
  double delta_x =  ::real(delta);
  double delta_y =  ::imag(delta);

  double step_x  = (delta_x < 0.0) ? -dist_x : dist_x;
  double step_y  = (delta_y < 0.0) ? -dist_y : dist_y;

  int lines_x    = (int)Abs(delta_x / dist_x) + 1;
  int lines_y    = (int)Abs(delta_y / dist_y) + 1;

  pts.segmentlist.Grow(lines_x + lines_y);
  int i;
  for (i=0; i<lines_x + lines_y; i++) pts.segmentlist[i].Grow(points);

  // Horizontal lines:
  for (i=0; i<lines_y; i++) {
    Complex left  = p1 + Complex(0, step_y*i);
    Complex right = Complex( ::real(p2),  ::imag(left));
    double s_x =  ::real(right-left) / (points-1);
    for (int j=0; j<points; j++)
      pts.segmentlist[i][j] = code->Run(left + j*s_x, err);
  }

  // Vertical lines:
  for (i=0; i<lines_x; i++) {
    Complex top = p1 + step_x*i;
    Complex bot = Complex( ::real(top),  ::imag(p2));
    Complex s_y = Complex(0,  ::imag(bot-top) / (points-1));
    for (int j=0; j<points; j++)
      pts.segmentlist[i+lines_y][j] = code->Run(top + Complex(j,0)*s_y, err);
  }
}

void GraphicGrid::RubberInit(DisplayPoint p)
{
  GraphicObjectBase::RubberInit(p);
  p1 = p2 = (*ppdrawhandler)->Map(p);
}

void GraphicGrid::RubberTo(DisplayPoint p)
{
  GraphicObjectBase::RubberTo(p);
  p2 = (*ppdrawhandler)->Map(p);
  Draw();
}

CppStr GraphicGrid::RubberInfo()
{
  return CppStr("Corner A=") + FormatNumber(p1) + "; Corner B=" + FormatNumber(p2);
}

void GraphicGrid::SetValues(Complex &_p1, Complex &_p2, double &_dx, double &_dy)
{
  p1     = _p1;
  p2     = _p2;
  assert(dist_x > 0.0);
  assert(dist_y > 0.0);
  dist_x = _dx;
  dist_y = _dy;
}

void GraphicGrid::GetExtent(Complex& lower_left, Complex& upper_right)
{
  lower_left  = p1;
  upper_right = p2;
}

//****************************************************************************
// GraphicPointsBase
//
// Features:
//
//
//
//
//
//
//****************************************************************************

GraphicPointsBase::GraphicPointsBase(pDrawHandler *h, int p)
  : GraphicObjectBase(h, p)
{
}

void GraphicPointsBase::DrawAsPoints(PointsDrawmode dm, int upper)
{
  GraphicObjectBase::Draw();

  if (upper == -1)
    upper = pts.NoOfElements();

  if (dm == edm_points) {
    for (int i=0; i<upper; i++)
      (*ppdrawhandler)->DrawDot(pts[i]);
  }
  else {
    (*ppdrawhandler)->SelectArrowstyle(dm == edm_directedlines ? DrawHandler::centred
				       : DrawHandler::none);
    Complex from, to;
    from = pts[0];
    for (int i=1; i<upper; i++) {
      to = pts[i];
      (*ppdrawhandler)->DrawLine(from, to);
      from = to;
    }
  }
  (*ppdrawhandler)->EndDrawing();
}

void GraphicPointsBase::GetExtent(Complex& lower_left, Complex& upper_right)
{
  lower_left  = extent_ll;
  upper_right = extent_ur;
}

//****************************************************************************
// GraphicIteration
//
// Features:
//
//
//
//
//
//
//****************************************************************************

GraphicIteration::GraphicIteration(pDrawHandler *h, int p)
  : GraphicPointsBase(h, p)
{
}

void GraphicIteration::Draw()
{
  GraphicPointsBase::Draw();
  (*ppdrawhandler)->BeginDrawing();
  (*ppdrawhandler)->DrawDot(origin);
  (*ppdrawhandler)->EndDrawing();
}

void GraphicIteration::DrawToMap(rComplexPointsSegmentList rangepts,
				 pCodeSegment code, rErrorObj err)
{
  GraphicObjectBase::DrawToMap(rangepts, code, err);
  rangepts.segmentlist.Grow(1);
  rangepts.segmentlist[0].Grow(points);
  pts.Empty();
  pts.Grow(points);

  Complex v = origin;
  SuperBox b;
  for (int i=0; i<points; i++) {
    b.AddPoint(v);
    pts[i] = v;
    v = rangepts.segmentlist[0][i] = code->Run(v, err);
  }
  b.GetResult(extent_ll, extent_ur);
}

void GraphicIteration::RubberInit(DisplayPoint p)
{
  GraphicObjectBase::RubberInit(p);
  origin = (*ppdrawhandler)->Map(p);
}

void GraphicIteration::RubberTo(DisplayPoint p)
{
  GraphicObjectBase::RubberTo(p);
  origin = (*ppdrawhandler)->Map(p);
  Draw();
}

CppStr GraphicIteration::RubberInfo()
{
  return CppStr("Origin=") + FormatNumber(origin);
}

//****************************************************************************
// GraphicParamPlot
//
// Features:
//
//
//
//
//
//
//****************************************************************************

GraphicParamPlot::GraphicParamPlot(pDrawHandler *h, int p,
				   pCodeSegment pdomaincode,
				   double tfrom, double tto,
				   rErrorObj err)
  : GraphicPointsBase(h, p)
{
  pts.Grow(p);

  double step = (tto - tfrom) / (points-1);
  double n = tfrom;

  SuperBox b;

  for (int i=0; i<p; i++, n += step)
    b.AddPoint( pts[i] = pdomaincode->Run(n, err) );

  b.GetResult(extent_ll, extent_ur);
}

void GraphicParamPlot::Draw()
{
  // do nothing
}

void GraphicParamPlot::DrawToMap(rComplexPointsSegmentList rangepts,
				 pCodeSegment code, rErrorObj err)
{
  GraphicObjectBase::DrawToMap(rangepts, code, err);
  rangepts.segmentlist.Grow(1);
  rangepts.segmentlist[0].Grow(points);

  assert(pts.NoOfElements() == points);

  for (int i=0; i<points; i++)
    rangepts.segmentlist[0][i] = code->Run(pts[i], err);
}

void GraphicParamPlot::RubberTo(DisplayPoint p)
{
}

CppStr GraphicParamPlot::RubberInfo()
{
  return CppStr("(N/A)");
}

//****************************************************************************
// GraphicFreehand
//
// Features:
//
//
//
//
//
//
//****************************************************************************

GraphicFreehand::GraphicFreehand(pDrawHandler *h)
  : GraphicPointsBase(h, 0), drawmode(edm_points)
{
}

void GraphicFreehand::SetDrawmodeForDraw(PointsDrawmode dm)
{
  drawmode = dm;
}


void GraphicFreehand::Draw()
{
  GraphicPointsBase::Draw();
  DrawAsPoints(drawmode, points);
}

void GraphicFreehand::DrawToMap(rComplexPointsSegmentList rangepts,
				 pCodeSegment code, rErrorObj err)
{
  GraphicObjectBase::DrawToMap(rangepts, code, err);
  rangepts.segmentlist.Grow(1);
  rangepts.segmentlist[0].Grow(points);

  SuperBox b;
  for (int i=0; i<points; i++) {
    b.AddPoint(pts[i]);
    rangepts.segmentlist[0][i] = code->Run(pts[i], err);
  }
  b.GetResult(extent_ll, extent_ur);
}

void GraphicFreehand::RubberInit(DisplayPoint p)
{
  GraphicObjectBase::RubberInit(p);

  points = 0;
  pts.Empty();
  pts.Grow(10);

  pts[points++] = current = (*ppdrawhandler)->Map(p);
}

void GraphicFreehand::RubberTo(DisplayPoint p)
{
  GraphicObjectBase::RubberTo(p);

  if (points >= pts.NoOfElements())
    pts.Grow(10);

  pts[points++] = current = (*ppdrawhandler)->Map(p);

  Draw();
}

void GraphicFreehand::RubberEnd()
{
  if (points != pts.NoOfElements()) {

    assert(points < pts.NoOfElements());

    ComplexPointsSegment tmp;
    int i;

    tmp.Grow(points);
    for (i=0; i<points; i++)
      tmp[i] = pts[i];

    pts.Empty();
    pts.Grow(points);
    for (i=0; i<points; i++)
      pts[i] = tmp[i];
  }
  GraphicObjectBase::RubberEnd();
}

CppStr GraphicFreehand::RubberInfo()
{
  return FormatNumber(current);
}

// EOF
