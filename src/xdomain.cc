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
//----- DomainDialog --------------------------------------------------------------------------
//

static Widget aDrawingObject_points[nTypesOfGraphicalObjects];
static Widget aDrawingObject_deltas[3];

static void DomainDialog_decode()
{
  int i;
  for (i=0; i<nTypesOfGraphicalObjects; i++) {
    if (i > 1) {  // ParamPlot and Freehand points are not implemented (here)
      char *text = XmTextGetString(aDrawingObject_points[i]);
      int n;
      if (sscanf(text, "%d", &n) == 1) 
	domain_win->NoOfPoints( (BaseDomainWindow::DrawMode) i, n);
      else
	Tell("Bad number of points given - ignored.");
      XtFree(text);
    }
  }
  for (i=0; i<3; i++) {
    char *text = XmTextGetString(aDrawingObject_deltas[i]);
    double d;
    if (sscanf(text, "%lf", &d) == 1) 
      domain_win->DeltaValue( (BaseDomainWindow::DeltaType) i, d);
    else
      Tell("Bad delta value given - ignored.");
    XtFree(text);
  }
}

static void cb_DomainDialog_button(Widget w, XtPointer client_data, XtPointer call_data)
{
  pRadioInfo i = (pRadioInfo) client_data;

  XtPopdown(i->shell);

  DomainDialog_decode();

  switch(i->button_no) {
  case  0: // Parameter plot
    domain_win->SetDrawMode(BaseDomainWindow::draw_parameterplot);
    break;
  case  1: // Freehand
    domain_win->SetDrawMode(BaseDomainWindow::draw_freehand);
    break;
  case  2: // Iterate point
    domain_win->SetDrawMode(BaseDomainWindow::draw_iteratepoint);
    break;
  case  3: // Line
    domain_win->SetDrawMode(BaseDomainWindow::draw_lines);
    break;
  case  4: // Rectangle
    domain_win->SetDrawMode(BaseDomainWindow::draw_rectangle);
    break;
  case  5: // Grid
    domain_win->SetDrawMode(BaseDomainWindow::draw_grid);
    break;
  case  6: // Circle
    domain_win->SetDrawMode(BaseDomainWindow::draw_circle);
    break;
  case  7: // Concentric Circles
    domain_win->SetDrawMode(BaseDomainWindow::draw_concentriccircles);
    break;

  case 10: // Coordinates
    domain_win->SetCoordsMode( bool( ! domain_win->GetCoordsMode() ) );
    domain_win->Redraw(0);
    break;
  case 11: // Axis
    domain_win->SetAxisMode( bool( ! domain_win->GetAxisMode() ) );
    domain_win->Redraw(0);
    break;
  case 12: // Directed objects
    domain_win->SetDirectedMode( bool( ! domain_win->GetDirectedMode() ) );
    domain_win->Redraw(0);
    break;
  case 13: // Parameter plot as points
    domain_win->SetPointsPlotDrawMode( ! domain_win->GetPointsPlotDrawMode());
    domain_win->Redraw(0);
    break;

  case 20: // Zoom In
    domain_win->ZoomIn();
    break;
  case 21: // Zoom Out
    domain_win->ZoomOut();
    break;
  case 22: // Zoom Area
    domain_win->SetZoomState();
    break;
  case 23: // Autoscale
    domain_win->AutoScale();
    break;

  default: cout << "cb_DomainDialog_button_toggle: Bad index: " << i->button_no << endl;
  }
}

static void cb_DomainDialog_ok(Widget w, XtPointer client_data, XtPointer call_data)
{
  Widget shell = (Widget) client_data;

  XtPopdown(shell);

  DomainDialog_decode();
}

void DomainDialog()
{
  static Widget dialog, pane, rowcol, actionarea, do_frame, do_box,
                pa_frame, pa_box, opt_frame, opt_box, zoom_frame, zoom_box;

  if (! dialog) {

    dialog = XtVaCreatePopupShell("dialog",
				  xmDialogShellWidgetClass,  domain_win->ShellWindow(),
				  XmNtitle,                  "Domain Options",
				  XmNnoResize,               True,
				  XmNdeleteResponse,         XmUNMAP,
				  XmNdialogStyle,            XmDIALOG_PRIMARY_APPLICATION_MODAL,
				NULL);

    pane = XtVaCreateWidget("pane", xmPanedWindowWidgetClass, dialog,
			    XmNsashWidth,  1,
			    XmNsashHeight, 1,
			    NULL);

    rowcol = XtVaCreateWidget("control_area",
			      xmRowColumnWidgetClass, pane,
			      XmNorientation,         XmHORIZONTAL,
			      NULL);

    // now we try to create the Drawing Object area: A frame with some radio buttons

    do_frame = XtVaCreateManagedWidget("radio_area",
				       xmFrameWidgetClass, rowcol,
				       XmNshadowType,      XmSHADOW_IN,
				       NULL);

    XtVaCreateManagedWidget("Drawing Objects",
			    xmLabelGadgetClass,        do_frame,
			    XmNchildType,              XmFRAME_TITLE_CHILD,
			    XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
			    NULL);
  
    do_box = XmCreateRadioBox(do_frame, "radio_box", NULL, 0);

    static char *do_labels[nTypesOfGraphicalObjects] = 
    { // Note the order! Must match BaseDomainWindow::DrawMode.
      "Parameter Plot",
      "Freehand",
      "Iterate Point",
      "Line",
      "Rectangle",
      "Grid",
      "Circle",
      "Concentric Circles"
    };
    static RadioInfo do_info[nTypesOfGraphicalObjects];

    Widget b;   // generic button widget

    // the buttons:
    int i;
    for (i=0; i<nTypesOfGraphicalObjects; i++) {
      b = XtVaCreateManagedWidget(do_labels[i],
				  xmToggleButtonGadgetClass, do_box,
				  XmNmarginHeight, 2,
				  NULL);
      do_info[i] = RadioInfo(dialog, i);
      XtAddCallback(b, XmNvalueChangedCallback, cb_DomainDialog_button,
		    (XtPointer*) &do_info[i]);
      XmToggleButtonGadgetSetState(b,
				   domain_win->GetDrawMode() == (BaseDomainWindow::DrawMode)i,
				   False);
    }

    int button_height;
    XtVaGetValues(b, XmNheight, &button_height, NULL);

    XtManageChild(do_box);

    // then the Points area: A frame with some numeric input fields:

    pa_frame = XtVaCreateManagedWidget("input_area",
				       xmFrameWidgetClass, rowcol,
				       XmNshadowType,      XmSHADOW_IN,
				       NULL);

    XtVaCreateManagedWidget("#Points",
			    xmLabelGadgetClass,        pa_frame,
			    XmNchildType,              XmFRAME_TITLE_CHILD,
			    XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
			    NULL);

    pa_box = XtVaCreateWidget("points_box",
			      xmRowColumnWidgetClass, pa_frame,
			      XmNnumColumns,          1,
			      XmNpacking,             XmPACK_COLUMN,
			      NULL);
 
    for (i=0; i<nTypesOfGraphicalObjects; i++) {
      char buf[15];
      sprintf(buf, "text_%d", i);  // to give separate name to each text input field
      aDrawingObject_points[i] = XtVaCreateManagedWidget(buf,
							xmTextFieldWidgetClass, pa_box,
							XmNmarginHeight,  2,
							XmNheight,       button_height,
							XmNwidth,        50,
							NULL);
    }

    XtManageChild(pa_box);

    // then the Delta area: A frame with some numeric input fields:

    static Widget da_frame = XtVaCreateManagedWidget("input_area",
				       xmFrameWidgetClass, rowcol,
				       XmNshadowType,      XmSHADOW_IN,
				       NULL);

    XtVaCreateManagedWidget("Delta values",
			    xmLabelGadgetClass,        da_frame,
			    XmNchildType,              XmFRAME_TITLE_CHILD,
			    XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
			    NULL);

    static Widget da_box = XtVaCreateWidget("delta_box",
					    xmRowColumnWidgetClass, da_frame,
					    XmNnumColumns,          1,
					    XmNpacking,             XmPACK_COLUMN,
					    NULL);

    static char *da_labels[3] = { "Grid X:", "Grid Y:", "Radius:" };
    for (i=0; i<3; i++) {
      XtVaCreateManagedWidget("labels",
			      xmLabelWidgetClass, da_box,
			      XtVaTypedArg, XmNlabelString,  // p366
			      XmRString, da_labels[i], strlen(da_labels[i]) + 1,
			      NULL);
      char buf[15];
      sprintf(buf, "dtext_%d", i);  // to give separate name to each text input field
      aDrawingObject_deltas[i] = XtVaCreateManagedWidget(buf,
							 xmTextFieldWidgetClass, da_box,
							 XmNmarginHeight,  2,
							 XmNheight,       button_height,
							 XmNwidth,        50,
							 NULL);
    }

    XtManageChild(da_box);

    // then the Options area: A frame with some toggle buttons:
    
    opt_frame = XtVaCreateManagedWidget("input_area",
					xmFrameWidgetClass, rowcol,
					XmNshadowType,      XmSHADOW_IN,
					NULL);

    XtVaCreateManagedWidget("Options",
			    xmLabelGadgetClass,        opt_frame,
			    XmNchildType,              XmFRAME_TITLE_CHILD,
			    XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
			    NULL);

    opt_box = XtVaCreateWidget("toggle_box",
			       xmRowColumnWidgetClass, opt_frame,
			       NULL);

    static char *opt_labels[4] =
    {
      "Coordinates",
      "Axis",
      "Directed Objects",
      "Param. as Points"
    };
    static RadioInfo opt_info[4];

    for (i=0; i<4; i++) {
      opt_info[i] = RadioInfo(dialog, 10 + i);
      b = XtVaCreateManagedWidget(opt_labels[i], xmToggleButtonGadgetClass, opt_box, NULL);
      XtAddCallback(b, XmNvalueChangedCallback, cb_DomainDialog_button,
		    (XtPointer*) &opt_info[i]);
      switch(i) {
      case 0: XmToggleButtonGadgetSetState(b, (Boolean)domain_win->GetCoordsMode(), False);
	      break;
      case 1: XmToggleButtonGadgetSetState(b, (Boolean)domain_win->GetAxisMode(), False);
	      break;
      case 2: XmToggleButtonGadgetSetState(b, (Boolean)domain_win->GetDirectedMode(), False);
	      break;
      case 3: XmToggleButtonGadgetSetState(b, (Boolean)domain_win->GetPointsPlotDrawMode(),
					   False);
	      break;
      default: assert(0);
      }
    }

    XtManageChild(opt_box);

    // then the Zoom area: A frame with some buttons:

    zoom_frame = XtVaCreateManagedWidget("input_area",
					 xmFrameWidgetClass, rowcol,
					 XmNshadowType,      XmSHADOW_IN,
					 NULL);

    XtVaCreateManagedWidget("Zoom",
			    xmLabelGadgetClass,        zoom_frame,
			    XmNchildType,              XmFRAME_TITLE_CHILD,
			    XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
			    NULL);

    zoom_box = XtVaCreateWidget("zoom_box",
				xmRowColumnWidgetClass, zoom_frame,
				NULL);

    static char *zoom_labels[4] = { "  Zoom In  ", "  Zoom Out  ",
				    "  Zoom Area  ", "  Autoscale  " };
    static RadioInfo zoom_info[4];

    for (i=0; i<4; i++) {
      if (i == 3) XtVaCreateManagedWidget("sep", xmSeparatorGadgetClass, zoom_box, NULL);
      zoom_info[i] = RadioInfo(dialog, 20 + i);
      b = XtVaCreateManagedWidget(zoom_labels[i], xmPushButtonGadgetClass, zoom_box, NULL);
      XtAddCallback(b, XmNactivateCallback, cb_DomainDialog_button, (XtPointer*) &zoom_info[i]);
    }

    XtManageChild(zoom_box);

    // more items should be filled in here.

    XtManageChild(rowcol);

    // Set up action area:
    static ActionAreaItem action_items[] = {
      { "  Ok  ", cb_DomainDialog_ok, NULL }
    };
    action_items[0].data = (XtPointer) dialog;   // so that cb_ok() can close dialog

    actionarea = CreateActionArea(pane, action_items, XtNumber(action_items));

    XtManageChild(pane);
  }

  int i;
  for (i=0; i<nTypesOfGraphicalObjects; i++) {
    char s[10];
    sprintf(s, "%d", domain_win->NoOfPoints( (BaseDomainWindow::DrawMode) i ) );
    if (i > 1)  // Freehand doesn't use points; ParamPlot doesn't show it here
      XmTextSetString(aDrawingObject_points[i], s);
    else
      XmTextSetString(aDrawingObject_points[i], "");
  }

  for (i=0; i<3; i++) {
    char s[256];
    sprintf(s, "%0.8f", domain_win->DeltaValue( (BaseDomainWindow::DeltaType) i) );
    XmTextSetString(aDrawingObject_deltas[i], s);
  }

  XtPopup(dialog, XtGrabNone);
}

