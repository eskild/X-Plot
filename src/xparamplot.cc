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

static bool eval(CppStr s, double &d)
{
  ErrorObj   err;
  pParser    parser = new ExpressionParser(scanner, cFALSE);
  pParseTree tree = NULL;
  pCodeSegment pcode = NULL;
  bool ok = false;

  scanner->Init();
  scanner->the_input_line = s;
  tree = parser->Parse(err);

  if (err==true) {
    Tell("Compilation Error in Range Expression.");
    CppStr t;
    while ( (t=err()) != "@" ) cout << t << endl;
  }
  else {
    pcode = tree->GenerateCode(err);
    if (err==true)
      Tell("Couldn't Generate Code for Range Expression.");
    else {
      Complex c;  // not used for anything
      double val = ::real(pcode->Run(c, err));
      if (err==true)
	Tell("Couldn't Evaluate Range Expression.");
      else {
	d = val;
	ok = true;
      }
    }
  }
  
  delete pcode;
  delete tree;
  delete parser;

  return ok;
}

static enum { RUNNING, OK, CANCEL }  Looping = RUNNING;

static void cb_ParamPlotDialog_ok(Widget w, XtPointer client_data, XtPointer call_data)
{
  Looping = OK;
}

static void cb_ParamPlotDialog_cancel(Widget w, XtPointer client_data, XtPointer call_data)
{
  Looping = CANCEL;
}

bool ParamPlotDialog(CppStr &functext, double &t_from, double &t_to, int &pts)
{
  static Widget dialog;
  static Widget texts[4];

  if (! dialog) {
    dialog = XtVaCreatePopupShell("dialog",
				  xmDialogShellWidgetClass,  domain_win->ShellWindow(),
				  XmNtitle,                  "Parameter Plot Values",
				  XmNnoResize,               False,
				  XmNdeleteResponse,         XmUNMAP,
				  XmNdialogStyle,            XmDIALOG_PRIMARY_APPLICATION_MODAL,
				NULL);

    Widget pane = XtVaCreateWidget("pane", xmPanedWindowWidgetClass, dialog,
				   XmNsashWidth,  1,
				   XmNsashHeight, 1,
				   NULL);

    char *labeltexts[4] =
    {
      "f(t)=",
      "From t=",
      "to t=",
      "Points:"
    };


    Widget rowcol = XtVaCreateWidget("rowcolumn",
				     xmRowColumnWidgetClass, pane,
				     XmNpacking, XmPACK_COLUMN,
				     XmNnumColumns, XtNumber(labeltexts),
				     XmNorientation, XmHORIZONTAL,
				     XmNisAligned, True,
				     XmNentryAlignment, XmALIGNMENT_END,
				     NULL);

    char buf[20];
    for (int i=0; i<4; i++) {
      XtVaCreateManagedWidget(labeltexts[i],
			      xmLabelGadgetClass, rowcol,
			      NULL);
      sprintf(buf, "text_%d", i);
      texts[i] = XtVaCreateManagedWidget(buf,
					 xmTextWidgetClass, rowcol,
					 NULL);
    }

    XtManageChild(rowcol);

    // Set up action area:
    static ActionAreaItem action_items[] = {
      { "   Ok   ", cb_ParamPlotDialog_ok,     NULL },
      { " Cancel ", cb_ParamPlotDialog_cancel, NULL }
    };
    CreateActionArea(pane, action_items, XtNumber(action_items));
    
    XtManageChild(pane);
  }

  XtManageChild(dialog);
  XtPopup(dialog, XtGrabNone);

  Looping = RUNNING;
  while (Looping == RUNNING)
    XtAppProcessEvent(app_context, XtIMAll);

  XtPopdown(dialog);

  if (Looping == OK) {
    // Retrieve values:
    functext = XmTextGetString(texts[0]);
    char *tfromtext  = XmTextGetString(texts[1]);
    char *ttotext    = XmTextGetString(texts[2]);
    char *pointstext = XmTextGetString(texts[3]);
    
    if (!eval(tfromtext, t_from)  ||  !eval(ttotext, t_to)  ||
	sscanf(pointstext, " %d", &pts) != 1)
      return false;
    
    return true;
  }

  return false;  // CANCEL
}

// EOF
