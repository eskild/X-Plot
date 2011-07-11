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
//----- CoordWindow ---------------------------------------------------------------------------
//

static void cb_CoordWindow_resize(Widget w, XtPointer client_data, XtPointer call_data)
{
  coord_win->Resize();
}

static void cb_CoordWindow_expose(Widget w, XtPointer client_data, XtPointer call_data)
{
  XmDrawingAreaCallbackStruct *cbs = (XmDrawingAreaCallbackStruct *) call_data;
  if (cbs->reason == XmCR_EXPOSE) {
    XExposeEvent *event = (XExposeEvent *) cbs->event;
    coord_win->Redraw(event->count);
  }
}

CoordWindow::CoordWindow(Widget parent)
  : BaseCoordWindow( new XDrawHandler(new LinearDisplayMapper) )
{
  shell = XtVaCreatePopupShell("domain_shell",
			       xmDialogShellWidgetClass,  XtParent(parent),
			       XmNtitle,                  "Coordinates",
			       XmNdeleteResponse,         XmDO_NOTHING,
			       XmNx,                      300,
			       XmNy,                       50,
			       NULL);
  
  area = XtVaCreateWidget("domain_area",
			  xmDrawingAreaWidgetClass, shell,
			  XmNwidth,                 300,
			  XmNheight,                 75,
			  NULL);

  XtAddCallback(area, XmNresizeCallback, cb_CoordWindow_resize, NULL);
  XtAddCallback(area, XmNexposeCallback, cb_CoordWindow_expose, NULL);
  XtManageChild(area);

  winsysinfo = new XSysInfo(area);

  drawhandler->AttachToWindow(this).SelectBackgroundColor(DrawHandler::white);

  XtPopup(shell, XtGrabNone);
}

void CoordWindow::Resize()
{
  Dimension width, height;
  XtVaGetValues(area, XmNwidth, &width, XmNheight, &height, NULL);
  SetScreenGeometry( DisplayPoint(width-1, height-1) );
}

void CoordWindow::Redraw(int n)
{
  if (n == 0) {
    ClearWindow();
    BaseCoordWindow::Redraw();
  }
}

