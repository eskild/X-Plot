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
//----- RangeWindow ---------------------------------------------------------------------------
//

static void ac_RangeWindow(Widget w, XEvent *event, String args[], 
			   unsigned int *num_args)
{
  XButtonEvent *button_event = (XButtonEvent *) event;

  if (*num_args != 1)
    XtError("ac_RangeWindow: Wrong number of arguments.");

  range_win->SetShiftKeyState( bool(button_event->state & ShiftMask) );
  range_win->SetControlKeyState( bool(button_event->state & ControlMask) );

  DisplayPoint location(button_event->x, button_event->y);

  if (strcmp(args[0], "move")       == 0) {
    range_win -> PointerTo_Range(location);
  } else
  if (strcmp(args[0], "startpoint") == 0) {
    range_win -> PointerInit(location);
  } else
  if (strcmp(args[0], "movepoint")  == 0) {
    range_win -> PointerTo(location);
  } else
  if (strcmp(args[0], "endpoint")   == 0) {
    range_win -> PointerEnd();
  } else
  if (strcmp(args[0], "menu")       == 0) {
    if (range_win->IsIdleState()) 
      RangeDialog();
    else
      range_win->PointerRightInit(location);
  } else
    XtError("ac_RangeWindow: Bad argument.");
}

static void cb_RangeWindow_resize(Widget w, XtPointer client_data, XtPointer call_data)
{
  range_win->Resize();
}

static void cb_RangeWindow_expose(Widget w, XtPointer client_data, XtPointer call_data)
{
  XmDrawingAreaCallbackStruct *cbs = (XmDrawingAreaCallbackStruct *) call_data;
  if (cbs->reason == XmCR_EXPOSE) {
    XExposeEvent *event = (XExposeEvent *) cbs->event;
    range_win->Redraw(event->count);
  }
}

RangeWindow::RangeWindow(Widget parent)
  : BaseRangeWindow( new XDrawHandler(new LinearDisplayMapper) )
{
  SetZoomFactor(::zoom_factor);
  shell = XtVaCreatePopupShell("domain_shell",
			       xmDialogShellWidgetClass,  XtParent(parent),
			       XmNtitle,                  "Range",
			       XmNdeleteResponse,         XmDO_NOTHING,
			       XmNx,                      300,
			       XmNy,                       50,
			       NULL);
  
  // Install actions and translations and create drawing area:
  XtActionsRec actions;
  actions.string = "ac_RangeWindow";
  actions.proc   = ac_RangeWindow;
  XtAppAddActions(app_context, &actions, 1);

  String translations = "<Btn1Motion>: ac_RangeWindow(movepoint)  ManagerGadgetButtonMotion() \n\
                         <Btn1Down>:   ac_RangeWindow(startpoint) ManagerGadgetArm()          \n\
                         <Btn1Up>:     ac_RangeWindow(endpoint)   ManagerGadgetActivate()     \n\
                         <Btn3Up>:     ac_RangeWindow(menu)       ManagerGadgetActivate()     \n\
                         <Motion>:     ac_RangeWindow(move)";
  area = XtVaCreateWidget("domain_area",
			  xmDrawingAreaWidgetClass, shell,
			  XmNtranslations,          XtParseTranslationTable(translations),
			  XmNwidth,                 200,
			  XmNheight,                200,
			  NULL);

  XtAddCallback(area, XmNresizeCallback, cb_RangeWindow_resize, NULL);
  XtAddCallback(area, XmNexposeCallback, cb_RangeWindow_expose, NULL);
  XtManageChild(area);

  winsysinfo = new XSysInfo(area);

  drawhandler->AttachToWindow(this).SelectBackgroundColor(DrawHandler::black);

  XtPopup(shell, XtGrabNone);
}

void RangeWindow::Resize()
{
  Dimension width, height;
  XtVaGetValues(area, XmNwidth, &width, XmNheight, &height, NULL);
  SetScreenGeometry( DisplayPoint(width-1, height-1) );
}

void RangeWindow::Redraw(int n)
{
  if (n == 0) {
    ClearWindow();
    BaseRangeWindow::Redraw();
  }
}

void RangeWindow::SetDrawMode(DrawMode m)
{
  BaseRangeWindow::SetDrawMode(m);
  Redraw(0);
}

void RangeWindow::PointerTo_Range(DisplayPoint dp)
{
  SetCursor(drawhandler->Map(dp));
}

void RangeWindow::SetShiftKeyState(bool down)
{
  SetShiftState(down);
}

void RangeWindow::SetControlKeyState(bool down)
{
  SetControlState(down);
}

void RangeWindow::DrawTextInCoordWindow(const CppStr &str)
{
  coord_win->RangeText(str);
}

