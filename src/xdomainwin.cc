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

#include "xdefs.h"

//
//----- DomainWindow --------------------------------------------------------------------------
//

static void ac_DomainWindow(Widget w, XEvent *event, String args[],
			    unsigned int *num_args)
{
  XButtonEvent *button_event = (XButtonEvent *) event;

  if (*num_args != 1)
    XtError("ac_DomainWindow: Wrong number of arguments.");

  domain_win->SetShiftKeyState( bool(button_event->state & ShiftMask) );
  domain_win->SetControlKeyState( bool(button_event->state & ControlMask) );

  DisplayPoint location(button_event->x, button_event->y);

  if (strcmp(args[0], "move")       == 0) {
    domain_win -> PointerTo_Range(location);
  } else
  if (strcmp(args[0], "startpoint") == 0) {
    domain_win -> PointerInit(location);
  } else
  if (strcmp(args[0], "movepoint")  == 0) {
    domain_win -> PointerTo(location);
  } else
  if (strcmp(args[0], "endpoint")   == 0) {
    domain_win -> PointerEnd();
  } else
  if (strcmp(args[0], "menu")       == 0) {
    if (domain_win->IsIdleState())
      DomainDialog();
    else
      domain_win->PointerRightInit(location);
  } else
    XtError("ac_DomainWindow: Bad argument.");
}

static void cb_DomainWindow_resize(Widget w, XtPointer client_data, XtPointer call_data)
{
  domain_win->Resize();
}

static void cb_DomainWindow_expose(Widget w, XtPointer client_data, XtPointer call_data)
{
  XmDrawingAreaCallbackStruct *cbs = (XmDrawingAreaCallbackStruct *) call_data;
  if (cbs->reason == XmCR_EXPOSE) {
    XExposeEvent *event = (XExposeEvent *) cbs->event;
    domain_win->Redraw(event->count);
  }
}

DomainWindow::DomainWindow(Widget parent)
  : BaseDomainWindow( new XDrawHandler(new LinearDisplayMapper) )
{
  SetZoomFactor(::zoom_factor);
  shell = XtVaCreatePopupShell("domain_shell",
			       xmDialogShellWidgetClass,  XtParent(parent),
			       XmNtitle,                  "Domain",
			       XmNdeleteResponse,         XmDO_NOTHING,
			       NULL);
  
  // Install actions and translations and create drawing area:
  XtActionsRec actions;
  actions.string = "ac_DomainWindow";
  actions.proc   = ac_DomainWindow;
  XtAppAddActions(app_context, &actions, 1);

  String translations = "<Btn1Motion>: ac_DomainWindow(movepoint)  ManagerGadgetButtonMotion() \n\
                         <Btn1Down>:   ac_DomainWindow(startpoint) ManagerGadgetArm()          \n\
                         <Btn1Up>:     ac_DomainWindow(endpoint)   ManagerGadgetActivate()     \n\
                         <Btn3Up>:     ac_DomainWindow(menu)       ManagerGadgetActivate()     \n\
                         <Motion>:     ac_DomainWindow(move)";
  area = XtVaCreateWidget("domain_area",
			  xmDrawingAreaWidgetClass, shell,
			  XmNtranslations,          XtParseTranslationTable(translations),
			  XmNwidth,                 200,
			  XmNheight,                200,
			  NULL);

  XtAddCallback(area, XmNresizeCallback, cb_DomainWindow_resize, NULL);
  XtAddCallback(area, XmNexposeCallback, cb_DomainWindow_expose, NULL);
  XtManageChild(area);

  winsysinfo = new XSysInfo(area);

  drawhandler->AttachToWindow(this).SelectBackgroundColor(DrawHandler::black);

  XtPopup(shell, XtGrabNone);
}

void DomainWindow::Resize()
{
  Dimension width, height;
  XtVaGetValues(area, XmNwidth, &width, XmNheight, &height, NULL);
  SetScreenGeometry( DisplayPoint(width-1, height-1) );
}

void DomainWindow::Redraw(int n)
{
  if (n == 0) {
    ClearWindow();
    BaseDomainWindow::Redraw();
  }
}

void DomainWindow::RubberCalcPoints()
{
  pCodeSegment map_code = root_win->GetMapCodeSegment();
  if (map_code == 0) Tell("\
No map function given; you should supply one!\n\
Do this by clicking the right mouse button\n\
in the root window (the window with the menu.)");
  else {
    ErrorObj err;
    pComplexPointsSegmentList list = new ComplexPointsSegmentList;
    assert(list != 0);
    drawhandler->Busy(cTRUE);
    rubber_obj->DrawToMap(*list, map_code, err);
    drawhandler->Busy(cFALSE);

    if (rubber_obj->IsA() == GR_ITERATION  ||
	rubber_obj->IsA() == GR_PARAMPLOT) {
//	rubber_obj->IsA() == GR_FREEHAND) {
      GraphicPointsBase::PointsDrawmode drawmode;
      if (GetPointsPlotDrawMode())
	drawmode = GraphicPointsBase::edm_points;
      else
	if (GetDirectedMode())
	  drawmode = GraphicPointsBase::edm_directedlines;
	else
	  drawmode = GraphicPointsBase::edm_lines;
      ((GraphicPointsBase*)rubber_obj)->DrawAsPoints(drawmode);
    }
    
    if (err == cTRUE) {
      Tell("Couldn't calculate all points.");
      SetIdleState();
      return;
    }
    range_win->AddPoints(list);
  }
}

GraphicParamPlot* DomainWindow::SelectParamPlot()
{
  CppStr f;
  double tf, tt;
  int np;

  if (ParamPlotDialog(f, tf, tt, np)) {

    if (tf >= tt) {
      Tell("Value Error: From >= To.");
      return NULL;
    }

    if (np < 1) {
      Tell("Value Error: Points < 1.");
      return NULL;
    }

    scanner->Init();
    scanner->the_input_line = CppStr("PaRaMpLoT_MaP_FuNc(t)=") + f;

    ErrorObj   err;
    pParser    parser = new AssignmentParser(scanner, cFALSE);
    pParseTree tree   = parser->Parse(err);
    delete tree;
    delete parser;

    int i = userdefs->LinearFind("PaRaMpLoT_MaP_FuNc");

    if (err==true  ||  i == -1) {
      Tell("Compilation Error!");
      CppStr t;
      while ( (t=err()) != "@" ) cout << t << endl;
      return NULL;
    }

    // Everything ok; we accept the dialog

    paramplot_tfrom = tf;
    paramplot_tto   = tt;
    pparamplot_code = (*userdefs)[i].value.pCode;
    NoOfPoints(draw_parameterplot, np);

    err = false;

    GraphicParamPlot *pgp = new GraphicParamPlot(&drawhandler, np,
						pparamplot_code,
						tf,
						tt,
						err);

    if (err == true) {
      Tell("Couldn't Calculate All Domain Points in Param. Plot.");
      delete pgp;
      return NULL;
    }

    return pgp;
  }

  return NULL;
}

void DomainWindow::PointerInit(DisplayPoint dp)
{
  if (IsIdleState()) SetRubberState();  // FIXME '!IsZoomState()'
  BaseDomainWindow::PointerInit(dp);
}

void DomainWindow::PointerTo_Range(DisplayPoint dp)
{
  Complex coord = drawhandler->Map(dp);
  if (!IsRubberState()  &&  !IsZoomState()) DrawTextInCoordWindow( FormatNumber(coord) );
  pCodeSegment map_code = root_win->GetMapCodeSegment();
  if (map_code != NULL) {
    static ErrorObj err;
    err           = cFALSE;
    coord         = map_code->Run(coord, err);
    if (err == cFALSE) range_win->SetCursor(coord);
  }
}

void DomainWindow::SetShiftKeyState(bool down)
{
  SetShiftState(down);
}

void DomainWindow::SetControlKeyState(bool down)
{
  SetControlState(down);
}

void DomainWindow::Busy(bool b)
{
  drawhandler->Busy(b);
}

void DomainWindow::DrawTextInCoordWindow(const CppStr &str)
{
  coord_win->DomainText(str);
}

