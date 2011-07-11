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
//----- FormulaDialog ---------------------------------------------------------------------------
//

static Widget formula_text_entry_widget;

static void cb_FormulaDialog_ok(Widget w, XtPointer client_data, XtPointer call_data)
{
  Widget shell = (Widget) client_data;

  char *text = XmTextGetString(formula_text_entry_widget);

  scanner->the_input_line  = text;
  root_win->map_expression = text;

  root_win->ParseMapExpression(cFALSE);

  XtFree(text);

  XtPopdown(shell);
}

static void cb_FormulaDialog_recalculate(Widget w, XtPointer client_data, XtPointer call_data)
{
  Widget shell = (Widget) client_data;

  char *text = XmTextGetString(formula_text_entry_widget);

  scanner->the_input_line  = text;
  root_win->map_expression = text;

  root_win->ParseMapExpression(cTRUE);

  XtFree(text);

  XtPopdown(shell);
}

static void cb_FormulaDialog_cancel(Widget w, XtPointer client_data, XtPointer call_data)
{
  Widget shell = (Widget) client_data;

  XtPopdown(shell);
}

void FormulaDialog()
{
  static Widget dialog, pane, rowcol, actionarea;

  if (!dialog) {
    dialog = XtVaCreatePopupShell("dialog",
				  xmDialogShellWidgetClass,  root_win->Window(),
				  XmNtitle,                  "X-Plot Mapping Function",
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

    XtVaCreateManagedWidget("map(z)=", xmLabelGadgetClass, rowcol, NULL);

    formula_text_entry_widget = XtVaCreateManagedWidget("text_formula",
							xmTextFieldWidgetClass, rowcol,
							NULL);

    // more items should be filled in here.

    XtManageChild(rowcol);

    // Set up action area:
    static ActionAreaItem action_items[] = {
      { "  Ok  ",      cb_FormulaDialog_ok,         NULL },
      { "Recalculate", cb_FormulaDialog_recalculate, NULL },
      { "Cancel",      cb_FormulaDialog_cancel,     NULL }
    };
    action_items[0].data = (XtPointer) dialog;   // so that ok() can close dialog
    action_items[1].data = (XtPointer) dialog;   // so that recalculate() can close dialog
    action_items[2].data = (XtPointer) dialog;   // so that cancel() can close dialog

    actionarea = CreateActionArea(pane, action_items, XtNumber(action_items));

    XtManageChild(pane);
  }
  XmTextSetString(formula_text_entry_widget, & root_win->map_expression[0]);

  XtPopup(dialog, XtGrabNone);
}

