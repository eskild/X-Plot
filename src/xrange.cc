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
//----- RangeDialog ---------------------------------------------------------------------------
//

static void cb_RangeDialog_ok(Widget w, XtPointer client_data, XtPointer call_data)
{
  Widget shell = (Widget) client_data;

  XtPopdown(shell);
}

static void cb_RangeDialog_button(Widget w, XtPointer client_data, XtPointer call_data)
{
  pRadioInfo i = (pRadioInfo) client_data;

  XtPopdown(i->shell);

  switch(i->button_no) {
  case  0: // Points mode
    range_win->SetDrawMode(BaseRangeWindow::draw_points);
    break;
  case  1: // Lines mode
    range_win->SetDrawMode(BaseRangeWindow::draw_lines);
    break;
  case  2: // Directed lines mode
    range_win->SetDrawMode(BaseRangeWindow::draw_directed_lines);
    break;

  case 10: // Coordinates
    range_win->SetCoordsMode( bool( ! range_win->GetCoordsMode() ) );
    range_win->Redraw(0);
    break;
  case 11: // Axis
    range_win->SetAxisMode( bool( ! range_win->GetAxisMode() ) );
    range_win->Redraw(0);
    break;

  case 20: // Zoom In
    range_win->ZoomIn();
    break;
  case 21: // Zoom Out
    range_win->ZoomOut();
    break;
  case 22: // Zoom Area
    range_win->SetZoomState();
    break;
  case 23: // Autoscale
    range_win->AutoScale();
    break;

  default: cout << "cb_RangeDialog_button_toggle: Bad index: " << i->button_no << endl;
  }
}

void RangeDialog()
{
  static Widget dialog, pane, rowcol, actionarea,
                dt_frame, dt_box, opt_frame, opt_box,
                zoom_frame, zoom_box;

  if (! dialog) {

    dialog = XtVaCreatePopupShell("dialog",
				  xmDialogShellWidgetClass,  range_win->ShellWindow(),
				  XmNtitle,                  "Range Options",
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
    
    // now we try to create the Drawing Type area: A frame with some radio buttons
    
    dt_frame = XtVaCreateManagedWidget("radio_area",
				       xmFrameWidgetClass, rowcol,
				       XmNshadowType,      XmSHADOW_IN,
				       NULL);
    
    XtVaCreateManagedWidget("Drawing Type",
			    xmLabelGadgetClass,        dt_frame,
			    XmNchildType,              XmFRAME_TITLE_CHILD,
			    XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
			    NULL);
    
    dt_box = XmCreateRadioBox(dt_frame, "radio_box", NULL, 0);
    
    static char *dt_labels[3] = { "Points", "Lines", "Directed lines" };
    static RadioInfo dt_info[3];
    
    static Widget b;   // generic button widget

    // the buttons:
    int i;
    for (i=0; i<3; i++) {
      b = XtVaCreateManagedWidget(dt_labels[i], xmToggleButtonGadgetClass, dt_box, NULL);
      dt_info[i] = RadioInfo(dialog, i);
      XtAddCallback(b, XmNvalueChangedCallback, cb_RangeDialog_button, (XtPointer*) &dt_info[i]);
      XmToggleButtonGadgetSetState(b,
				   range_win->GetDrawMode() == (BaseRangeWindow::DrawMode)i,
				   False);
    }

    XtManageChild(dt_box);

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
    
    static char *opt_labels[2] = { "Coordinates", "Axis" };
    static RadioInfo opt_info[2];
    
    for (i=0; i<2; i++) {
      opt_info[i] = RadioInfo(dialog, 10 + i);
      b = XtVaCreateManagedWidget(opt_labels[i], xmToggleButtonGadgetClass, opt_box, NULL);
      XtAddCallback(b, XmNvalueChangedCallback, cb_RangeDialog_button, (XtPointer*) &opt_info[i]);
      switch(i) {
      case 0: XmToggleButtonGadgetSetState(b, (Boolean)range_win->GetCoordsMode(), False);
	      break;
      case 1: XmToggleButtonGadgetSetState(b, (Boolean)range_win->GetAxisMode(), False);
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
      XtAddCallback(b, XmNactivateCallback, cb_RangeDialog_button, (XtPointer*) &zoom_info[i]);
    }
    
    XtManageChild(zoom_box);

    
    // more items should be filled in here.
    
    XtManageChild(rowcol);


    // Set up action area:
    static ActionAreaItem action_items[] = {
      { "  Ok  ", cb_RangeDialog_ok, NULL }
    };
    action_items[0].data = (XtPointer) dialog;   // so that cb_ok() can close dialog
    
    actionarea = CreateActionArea(pane, action_items, XtNumber(action_items));
    
    XtManageChild(pane);
  }
    
  XtPopup(dialog, XtGrabNone);
}

