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
#include "defs.h"
#include <stdio.h>

SuperBox::SuperBox() : valid(cFALSE)
{
}

SuperBox::SuperBox(const Complex &first)
: lower_left(first), upper_right(first), valid(cTRUE)
{
}

SuperBox& SuperBox::AddPoint(const Complex &p)
{
  if (setjmp(MathErrorJumpBuf) == -1) return *this;

  Complex c(p);

  if (valid) {
    lower_left = Complex( Min( ::real(lower_left ), ::real(c)),
			  Min( ::imag(lower_left ), ::imag(c)) );
    upper_right= Complex( Max( ::real(upper_right), ::real(c)),
			  Max( ::imag(upper_right), ::imag(c)) );
  }
  else {
    lower_left = upper_right = c;
    valid = cTRUE;
  }
  return *this;
}

SuperBox& SuperBox::GetResult(Complex &ll, Complex &ur)
{
  assert(valid);
  ll = lower_left;
  ur = upper_right;
  return *this;
}

//----- BaseWindow -----------------------------------------------------------

#define DETERMINE_POINTS_DRAWMODE 				\
      GraphicPointsBase::PointsDrawmode drawmode;		\
      if (GetPointsPlotDrawMode())				\
	drawmode = GraphicPointsBase::edm_points;		\
      else							\
	if (GetDirectedMode())					\
	  drawmode = GraphicPointsBase::edm_directedlines;	\
	else							\
	  drawmode = GraphicPointsBase::edm_lines;


BaseWindow::BaseWindow(pDrawHandler pDH) : winsysinfo(0), drawhandler(pDH)
{
  assert(pDH != 0);
}

BaseWindow::~BaseWindow()
{
  delete drawhandler;
  delete winsysinfo;
}

void BaseWindow::ClearWindow()
{
  drawhandler->Clear();
}

void BaseWindow::Busy()
{
  drawhandler->Busy(cTRUE);
}

void BaseWindow::NotBusy()
{
  drawhandler->Busy(cFALSE);
}

void BaseWindow::SetScreenGeometry(DisplayPoint dp, int dpi)
{
  drawhandler->SetScreenGeometry(dp, dpi);
}

void BaseWindow::GetScreenGeometry(DisplayPoint &dp)
{
  drawhandler->GetScreenGeometry(dp);
}

pDrawHandler BaseWindow::SetDrawHandler(pDrawHandler the_new)
{
  if (the_new != NULL) {	// Install new and return old
    Complex ll, ur;
    drawhandler->GetVirtualGeometry(ll, ur);

    pDrawHandler temp = drawhandler;
    drawhandler = the_new;

    drawhandler->SetVirtualGeometry(ll, ur);
    return temp;
  }
  else return drawhandler;	// NULL argument => return current
}

//----- BaseCoordWindow ------------------------------------------------------

BaseCoordWindow::BaseCoordWindow(pDrawHandler pDH) : BaseWindow(pDH)
{
}

void BaseCoordWindow::Redraw()
{
  drawhandler->SelectColorBlack().DrawText(0, domain).DrawText(1, range);
}

void BaseCoordWindow::Clear()
{
  DomainText("");
  RangeText("");
}

void BaseCoordWindow::DomainText(const CppStr &s)
{
  domain = s;
  drawhandler->SelectColorBlack().DrawText(0, domain);
}

void BaseCoordWindow::RangeText(const CppStr &s)
{
  range = s;
  drawhandler->SelectColorBlack().DrawText(1, range);
}

//----- BaseGraphWindow ------------------------------------------------------

BaseGraphWindow::BaseGraphWindow(pDrawHandler pDH) 
  : BaseWindow(pDH), state(idle), draw_coords(cFALSE), draw_axis(cTRUE),
    draw_directed(cFALSE), zoom_factor(2)
{
}

void BaseGraphWindow::SetVirtualGeometry(const Complex lower_left,
				    const Complex upper_right)
{
  drawhandler->SetVirtualGeometry(lower_left, upper_right);
}

void BaseGraphWindow::GetVirtualGeometry(Complex& l_l, Complex& u_r)
{
  drawhandler->GetVirtualGeometry(l_l, u_r);
}

void BaseGraphWindow::SetCoordsMode(bool newmode)
{
  draw_coords = newmode;
}

bool BaseGraphWindow::GetCoordsMode()
{
  return draw_coords;
}

void BaseGraphWindow::SetAxisMode(bool newmode)
{
  draw_axis = newmode;
}

bool BaseGraphWindow::GetAxisMode()
{
  return draw_axis;
}

void BaseGraphWindow::SetDirectedMode(bool newmode)
{
  draw_directed = newmode;
}

bool BaseGraphWindow::GetDirectedMode()
{
  return draw_directed;
}

void BaseGraphWindow::Centre(const Complex& c)
{
  drawhandler->Centre(c);
}

void BaseGraphWindow::ZoomIn()
{
  if (setjmp(MathErrorJumpBuf) == -1) return;

  Complex l_l, u_r;

  GetVirtualGeometry(l_l, u_r);
  double delta_x = ( ::real(u_r) -  ::real(l_l)) / zoom_factor / 2.0;
  double delta_y = ( ::imag(u_r) -  ::imag(l_l)) / zoom_factor / 2.0;
  Complex delta  = Complex(delta_x, delta_y);
  l_l = l_l + delta; 
  u_r = u_r - delta;
  SetVirtualGeometry(l_l, u_r);
}

void BaseGraphWindow::ZoomOut()
{
  if (setjmp(MathErrorJumpBuf) == -1) return;

  Complex l_l, u_r;

  GetVirtualGeometry(l_l, u_r);
  double delta_x = ( ::real(u_r) -  ::real(l_l)) * zoom_factor / 4.0;
  double delta_y = ( ::imag(u_r) -  ::imag(l_l)) * zoom_factor / 4.0;
  Complex delta  = Complex(delta_x, delta_y);
  l_l = l_l - delta; 
  u_r = u_r + delta;
  SetVirtualGeometry(l_l, u_r);
}

void BaseGraphWindow::Redraw()
{
  if (draw_axis)   DrawAxis();
  if (draw_coords) DrawCoords();
}

void BaseGraphWindow::DrawAxis()
{
  DisplayPoint zero = drawhandler->Map(Complex(0.0,0.0));
  DisplayPoint ll   = drawhandler->GetLowerLeftCornerOfDisplay();
  DisplayPoint ur   = drawhandler->GetUpperRightCornerOfDisplay();

  drawhandler->SetDrawmode(DrawHandler::on)
  	      .SetLinestyle(DrawHandler::dashed)
	      .SelectArrowstyle(DrawHandler::endpoint)
	      .SelectColor(0L)
              .DrawLine( drawhandler->Map(DisplayPoint(ll.x, zero.y)),
			 drawhandler->Map(DisplayPoint(ur.x, zero.y)) )
  	      .DrawLine( drawhandler->Map(DisplayPoint(zero.x, ll.y)),
			 drawhandler->Map(DisplayPoint(zero.x, ur.y)) )
	      .SetLinestyle(DrawHandler::solid)
	      .SelectArrowstyle(DrawHandler::none);
}

void BaseGraphWindow::DrawCoords()
{
  DisplayPoint zero = drawhandler->Map(Complex(0.0,0.0));
  DisplayPoint ll   = drawhandler->GetLowerLeftCornerOfDisplay();
  DisplayPoint ur   = drawhandler->GetUpperRightCornerOfDisplay();

  Complex left, right, top, bottom;

  int dx_positive = (ur.x - ll.x) / Abs(ur.x - ll.x) * drawhandler->Cm_to_Dots(0.1);
  int dy_positive = (ur.y - ll.y) / Abs(ur.y - ll.y) * drawhandler->Cm_to_Dots(0.1);

  left   = drawhandler->Map(DisplayPoint(ll.x + dx_positive, zero.y + dy_positive));
  right  = drawhandler->Map(DisplayPoint(ur.x - dx_positive, zero.y + dy_positive));
  top    = drawhandler->Map(DisplayPoint(zero.x + dx_positive, ur.y - dy_positive));
  bottom = drawhandler->Map(DisplayPoint(zero.x + dx_positive, ll.y + dy_positive));

  drawhandler->SelectColorWhite();  // White text on black background
  drawhandler->DrawText(left, FormatNumber(::real(left)));
//  drawhandler->DrawText(right, FormatNumber(::real(right)));
//  drawhandler->DrawText(top, FormatNumber(::imag(top)));
  drawhandler->DrawText(bottom, FormatNumber(::imag(bottom)));

  // Note: right and bottom coordinates are badly drawn, due to an incompleteness in
  // the DrawHandler::DrawText(Complex, CppStr) function. It should be possible to
  // adjust the text so that the point given can relate to any of the four text corners:
  //
  //   1---------2
  //   |  text   |
  //   3---------4
}

void BaseGraphWindow::PointerRightInit(DisplayPoint dp)
{
  if (setjmp(MathErrorJumpBuf) == -1) return;
  switch(state) {
  case doing_shift   : Centre( drawhandler->Map(dp) );
		       break;
  case doing_control : ZoomOut();
		       break;
  default:
    break;
  }
}

void BaseGraphWindow::PointerInit(DisplayPoint dp)
{
  if (setjmp(MathErrorJumpBuf) == -1) return;
  switch(state) {
  case doing_zoom    : ZoomAreaInit(dp);
		       break;
  case doing_rubber  : RubberInit(dp);
		       break;
  case doing_shift   : Centre( drawhandler->Map(dp) );
		       break;
  case doing_control : ZoomIn();
		       break;
  default:
    break;
  }
}

void BaseGraphWindow::PointerTo(DisplayPoint dp)
{
  PointerTo_Range(dp);
  switch(state) {
  case doing_zoom   : ZoomAreaTo(dp);
		      break;
  case doing_rubber : RubberTo(dp);
		      break;
  default:
    break;
  }
}

void BaseGraphWindow::PointerEnd()
{
  switch(state) {
  case doing_zoom   : ZoomAreaEnd();
		      break;
  case doing_rubber : RubberEnd();
		      break;
  default:
    break;
  }
}

void BaseGraphWindow::RubberInit(DisplayPoint)
{
  assert(0);
}

void BaseGraphWindow::RubberTo(DisplayPoint)
{
  assert(0);
}

void BaseGraphWindow::RubberEnd()
{
  assert(0);
}

void BaseGraphWindow::ZoomAreaInit(DisplayPoint dp)
{
  if (setjmp(MathErrorJumpBuf) == -1) return;

  zoom_corner1 = zoom_corner2 = drawhandler->Map(dp);
  drawhandler->BeginCapture()
	      .SetDrawmode(DrawHandler::reverse)	// set up appearance
	      .SetLinestyle(DrawHandler::dashed)
	      .SelectArrowstyle(DrawHandler::none);
}

void BaseGraphWindow::ZoomAreaTo(DisplayPoint dp)
{
  if (setjmp(MathErrorJumpBuf) == -1) return;
  assert(state == doing_zoom);

  drawhandler->SetDrawmode(DrawHandler::reverse)		// set up appearance
	      .SetLinestyle(DrawHandler::dashed)
	      .SelectArrowstyle(DrawHandler::none)
  	      .DrawRectangle(zoom_corner1, zoom_corner2);	// undraw old

  zoom_corner2 = drawhandler->Map(dp);

  drawhandler->DrawRectangle(zoom_corner1, zoom_corner2);	// draw new
}

void BaseGraphWindow::ZoomAreaEnd()
{
  if (setjmp(MathErrorJumpBuf) == -1) return;

  assert(state == doing_zoom);
  state = idle;
  drawhandler->DrawRectangle(zoom_corner1, zoom_corner2);	// undraw

  SuperBox b;
  b.AddPoint(zoom_corner1).AddPoint(zoom_corner2)
   .GetResult(zoom_corner1, zoom_corner2);

  if (zoom_corner1 != zoom_corner2)
    drawhandler->SetVirtualGeometry(zoom_corner1, zoom_corner2)
      .SetLinestyle(DrawHandler::solid)
      .EndCapture();
}

void BaseGraphWindow::SetZoomFactor(int newfactor)
{
  assert(newfactor > 0);
  zoom_factor = newfactor;
}

void BaseGraphWindow::SetIdleState()
{
  state = idle;
// !!! Should be refined.
}

void BaseGraphWindow::SetZoomState()
{
  if (state == idle) state = doing_zoom;
}

void BaseGraphWindow::SetRubberState()
{
  if (state == idle) state = doing_rubber;
}

void BaseGraphWindow::SetShiftState(bool on)
{
  if (state == idle  ||  state == doing_shift) state = on ? doing_shift : idle;
}

void BaseGraphWindow::SetControlState(bool on)
{
  if (state == idle  ||  state == doing_control) state = on ? doing_control : idle;
}

bool BaseGraphWindow::IsIdleState()
{
  return bool(state == idle);
}

bool BaseGraphWindow::IsZoomState()
{
  return bool(state == doing_zoom);
}

bool BaseGraphWindow::IsRubberState()
{
  return bool(state == doing_rubber);
}

bool BaseGraphWindow::IsShiftState()
{
  return bool(state == doing_shift);
}

bool BaseGraphWindow::IsControlState()
{
  return bool(state == doing_control);
}

//----- BaseDomainWindow ---------------------------------------------------------

BaseDomainWindow::BaseDomainWindow(pDrawHandler pDH)
  : BaseGraphWindow(pDH),
    drawmode(draw_circle), points_drawmode(true),
    paramplot_tfrom(0.0), paramplot_tto(1.0),
    pparamplot_code(NULL), n_elements(0), rubber_obj(NULL)
{
  int i;
  aPoints.Grow(nTypesOfGraphicalObjects);
  aDelta.Grow(3);
  for (i=0; i<aPoints.NoOfElements(); i++) aPoints[i] = 40;
  for (i=0; i<aDelta.NoOfElements();  i++) aDelta[i]  = 0.1;
}

BaseDomainWindow::~BaseDomainWindow()
{
  for (DynaIdx i=0; i<n_elements; i++)
    delete elements[i];
}

BaseDomainWindow::DrawMode BaseDomainWindow::GetDrawMode()
{
  return drawmode;
}

void BaseDomainWindow::SetDrawMode(DrawMode newmode)
{
  drawmode = newmode;
}

void BaseDomainWindow::SetPointsPlotDrawMode(bool draw_as_points)
{
  points_drawmode = draw_as_points;
}

bool BaseDomainWindow::GetPointsPlotDrawMode()
{
  return points_drawmode;
}

int BaseDomainWindow::NoOfPoints(DrawMode type, int newvalue)
{
#ifdef __BORLANDC__
  const int maxpts = 800;
#else
  const int maxpts = 10000;
#endif
  int old = aPoints[type];
  if (newvalue != 0) {    // change no. of points
    newvalue = Abs(newvalue);
    aPoints[type] = (newvalue > maxpts) ? maxpts : newvalue;
  }
  return old;
}

double BaseDomainWindow::DeltaValue(DeltaType type, double newvalue)
{
  double old = aDelta[type];
  if (newvalue != 0.0) {  // change delta value
    aDelta[type] = Abs(newvalue);
  }
  return old;
}

void BaseDomainWindow::RubberInit(DisplayPoint p)
{
  assert(rubber_obj == 0);

  GraphicParamPlot *pparamplot = NULL;

  switch( GetDrawMode() ) {
  case draw_parameterplot:
    SetIdleState();
    pparamplot = SelectParamPlot();
    if (pparamplot != NULL) {
      rubber_obj = pparamplot;
      SetRubberState();
      rubber_obj->RubberInit(p);
      RubberEnd();
    }
    return;
  case draw_freehand:
    {
      rubber_obj = new GraphicFreehand(&drawhandler);
      DETERMINE_POINTS_DRAWMODE;
      ((GraphicFreehand *)rubber_obj)->SetDrawmodeForDraw(drawmode);
    }
    break;
  case draw_iteratepoint:
    rubber_obj = new GraphicIteration(&drawhandler, NoOfPoints(GetDrawMode()));
    break;
  case draw_lines:
    rubber_obj = new GraphicLine(&drawhandler, NoOfPoints(GetDrawMode()));
    break;
  case draw_rectangle:
    rubber_obj = new GraphicRectangle(&drawhandler, NoOfPoints(GetDrawMode()));
    break;
  case draw_grid:
    rubber_obj = new GraphicGrid(&drawhandler, NoOfPoints(GetDrawMode()),
				 DeltaValue(delta_grid_x), DeltaValue(delta_grid_y));
    break;
  case draw_circle:
    rubber_obj = new GraphicCircle(&drawhandler, NoOfPoints(GetDrawMode()));
    break;
  case draw_concentriccircles:
    rubber_obj = new GraphicConcentricCircles(&drawhandler,
					      NoOfPoints( GetDrawMode() ),
					      DeltaValue(delta_concentric_circles));
    break;
  default:
    return;
  }

  drawhandler->SelectArrowstyle( GetDirectedMode() ? DrawHandler::centred : DrawHandler::none );
  rubber_obj->RubberInit(p);
  SetRubberState();
}

void BaseDomainWindow::RubberTo(DisplayPoint p)
{
  assert(rubber_obj != 0);
  assert(state == doing_rubber);
  rubber_obj->RubberTo(p);
  DrawTextInCoordWindow(rubber_obj->RubberInfo());
}

void BaseDomainWindow::RubberEnd()
{
  assert(rubber_obj != 0);
  assert(state == doing_rubber);
  rubber_obj->RubberEnd();
  if (n_elements == elements.NoOfElements())
    elements.Grow(10);
  elements[n_elements++] = rubber_obj;
  RubberCalcPoints();   // Calculate points (map)
  rubber_obj = 0;
  SetIdleState();
}

void BaseDomainWindow::RubberCalcPoints()
{
}

void BaseDomainWindow::Redraw()
{
  assert(drawhandler != 0);
  Busy();
  BaseGraphWindow::Redraw();

  for (DynaIdx i=0; i<n_elements; i++)
    switch (elements[i]->IsA()) {
    case GR_ITERATION:
    case GR_PARAMPLOT:
    case GR_FREEHAND:
      drawhandler->SetDrawmode(DrawHandler::on);
      DETERMINE_POINTS_DRAWMODE;
      ((GraphicPointsBase*)elements[i])->DrawAsPoints(drawmode);
      break;
    default:
      drawhandler->SetDrawmode(DrawHandler::on);
      drawhandler->SelectArrowstyle( GetDirectedMode() ?
				     DrawHandler::centred :
				     DrawHandler::none );
      elements[i]->Draw();
    }

  NotBusy();
}

void BaseDomainWindow::Clear()
{
  assert(drawhandler != 0);
  for (DynaIdx i=0; i<n_elements; i++)
    delete (elements[i]);
  elements.Empty();
  n_elements = 0;
  drawhandler->Clear();
  Redraw();
}

void BaseDomainWindow::AutoScale()
{
  if (n_elements == 0) return;

  Busy();

  Complex ll, ur;
  SuperBox box;
  for (DynaIdx i=0; i<n_elements; i++) {
    elements[i]->GetExtent(ll, ur);
    box.AddPoint(ll).AddPoint(ur);
  }

  box.GetResult(ll, ur);

  Complex delta = ur - ll;
  delta = Complex(Abs(::real(delta))/20, Abs(::imag(delta))/20);

  ll -= delta;
  ur += delta;

  SetVirtualGeometry(ll, ur);

  NotBusy();
}

pGraphicObjectBase BaseDomainWindow::GetFirst()
{
  next = 0;
  return GetNext();
}

pGraphicObjectBase BaseDomainWindow::GetNext()
{
  if (next < n_elements) return elements[next++]; else return NULL;
}

//----- BaseRangeWindow -----------------------------------------------------

BaseRangeWindow::BaseRangeWindow(pDrawHandler pDH)
  : BaseGraphWindow(pDH), old_cursor_valid(cFALSE)
{
  drawmode = draw_lines;
}

BaseRangeWindow::~BaseRangeWindow()
{
  for (DynaIdx i=0; i<pointslist.NoOfElements(); i++)
    delete pointslist[i];
}

BaseRangeWindow::DrawMode BaseRangeWindow::GetDrawMode()
{
  return drawmode;
}

void BaseRangeWindow::SetDrawMode(DrawMode newmode)
{
  drawmode = newmode;
  SetDirectedMode( bool(drawmode == draw_directed_lines) );
}

void BaseRangeWindow::Clear()
{
  RemoveCursor();
  for (DynaIdx i=0; i<pointslist.NoOfElements(); i++)
    delete pointslist[i];
  pointslist.Empty();
  drawhandler->Clear();
  Redraw();
}

void BaseRangeWindow::Redraw()
{
  Busy();
  RemoveCursor();
  BaseGraphWindow::Redraw();
  for (DynaIdx i=0; i<pointslist.NoOfElements(); i++)
    DrawASegmentList(pointslist[i]);
  NotBusy();
}

void BaseRangeWindow::SetCursor(const Complex &coord)
{
  drawhandler->SetDrawmode(DrawHandler::reverse);
  if (old_cursor_valid) 
    drawhandler->DrawCross(old_cursor);
  else
    old_cursor_valid = cTRUE;
  drawhandler -> DrawCross(coord) . SetDrawmode(DrawHandler::on);
  old_cursor = coord;
  DrawTextInCoordWindow( FormatNumber(coord) );
}

void BaseRangeWindow::RemoveCursor()
{
  if (old_cursor_valid) {
    drawhandler->SetDrawmode(DrawHandler::reverse).DrawCross(old_cursor)
    		.SetDrawmode(DrawHandler::on);
    old_cursor_valid = cFALSE;
    DrawTextInCoordWindow( "" );
  }
}

void BaseRangeWindow::AddPoints(pComplexPointsSegmentList newpoints)
{
  assert(newpoints != 0);
  pointslist.Grow(1);
  pointslist[pointslist.NoOfElements()-1] = newpoints;
  Busy();
  RemoveCursor();
  DrawASegmentList(newpoints);
  NotBusy();
}

void BaseRangeWindow::DrawASegmentList(pComplexPointsSegmentList list)
{
  assert(list != 0);

  drawhandler->SelectColor(list->color).SetLinestyle(DrawHandler::solid);

  if (drawmode == draw_points) {
    for (DynaIdx i=0; i<list->segmentlist.NoOfElements(); i++) 		// For each segment:
      for (DynaIdx j=0; j<(*list).segmentlist[i].NoOfElements(); j++) 	// For each point in segment:
	drawhandler->DrawDot( (*list).segmentlist[i][j] );
  }
  else {
    drawhandler->SelectArrowstyle( (drawmode==draw_lines) ? DrawHandler::none : DrawHandler::centred );
    Complex from, to;
    for (DynaIdx i=0; i<list->segmentlist.NoOfElements(); i++) {		// For each segment:
      from = (*list).segmentlist[i][0];
      for (DynaIdx j=1; j<(*list).segmentlist[i].NoOfElements(); j++) {	// For each point in segment:
	to = (*list).segmentlist[i][j];
	drawhandler->DrawLine(from, to);
	from = to;
      }
    }
  }
}

void BaseRangeWindow::AutoScale()
{
  if (pointslist.NoOfElements() == 0) return;

  Busy();
  RemoveCursor();

  Complex ll, ur;
  SuperBox box;

  for (DynaIdx i=0; i<pointslist.NoOfElements(); i++) {
    pComplexPointsSegmentList list = pointslist[i];
    for (DynaIdx j=0; j<list->segmentlist.NoOfElements(); j++) 		// For each segment:
      for (DynaIdx k=0; k<(*list).segmentlist[j].NoOfElements(); k++) 	// For each point in segment:
	box.AddPoint((*list).segmentlist[j][k]);
  }

  box.GetResult(ll, ur);

  Complex delta = ur - ll;
  delta = Complex(Abs(::real(delta))/20, Abs(::imag(delta))/20);

  ll -= delta;
  ur += delta;

  SetVirtualGeometry(ll, ur);

  NotBusy();
}

void BaseRangeWindow::SetScreenGeometry(DisplayPoint dp, int dpi)
{
  RemoveCursor();
  BaseGraphWindow::SetScreenGeometry(dp, dpi);
}

void BaseRangeWindow::SetVirtualGeometry(const Complex lower_left,
					 const Complex upper_right)
{
  RemoveCursor();
  BaseGraphWindow::SetVirtualGeometry(lower_left, upper_right);
}

void BaseRangeWindow::ClearWindow()
{
  RemoveCursor();
  BaseGraphWindow::ClearWindow();
}

// EOF
