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

//===============================================================================================
//===== DIALOG BOXES ============================================================================
//===============================================================================================

//
//----- Tell() and Confirm() -------------------------------------------------------------------
//

// Constants for cb_getbutton()
enum { button_none, button_1, button_2 };

static void cb_getbutton(Widget w, XtPointer client_data, XtPointer call_data)
{
  int *answer = (int *) client_data;
  XmAnyCallbackStruct *cbs = (XmAnyCallbackStruct *) call_data;

  switch(cbs->reason) {
    case XmCR_OK:     *answer = button_1;    break;
    case XmCR_CANCEL: *answer = button_2;    break;
    default:          *answer = button_none;
  }
}

bool Confirm(const char *s, bool isQuestion)
{
  Widget dialog;
  int    answer = button_none;

  assert(root_win != NULL);

  if (isQuestion) {
    dialog       = XmCreateQuestionDialog(root_win->Window(), "dialog", NULL, 0);
    XmString yes = XmStringCreateLocalized("    Yes    ");
    XmString no  = XmStringCreateLocalized("    No    ");

    XtVaSetValues(dialog,
		  XmNdialogStyle,       XmDIALOG_PRIMARY_APPLICATION_MODAL,
		  XmNokLabelString,     yes,
		  XmNcancelLabelString, no,
		  NULL);
    XtSetSensitive(XmMessageBoxGetChild(dialog, XmDIALOG_HELP_BUTTON), False);
    XtAddCallback(dialog, XmNokCallback,     cb_getbutton, &answer);
    XtAddCallback(dialog, XmNcancelCallback, cb_getbutton, &answer);
    XmStringFree(yes);
    XmStringFree(no);
  }
  else {
    dialog = XmCreateInformationDialog(root_win->Window(), "dialog", NULL, 0);

    XtVaSetValues(dialog,
		  XmNdialogStyle,       XmDIALOG_PRIMARY_APPLICATION_MODAL,
		  NULL);
    XtSetSensitive(XmMessageBoxGetChild(dialog, XmDIALOG_CANCEL_BUTTON), False);
    XtSetSensitive(XmMessageBoxGetChild(dialog, XmDIALOG_HELP_BUTTON), False);
    XtAddCallback(dialog, XmNokCallback, cb_getbutton, &answer);
  }

  XmString title = XmStringCreateLocalized((char*)VERSION);
  XtVaSetValues(dialog, XmNdialogTitle, title, NULL);
  XmStringFree(title);

  XmString text = XmStringCreateLocalized( (char*) s);  // cast needed because of const char *s
  XtVaSetValues(dialog, XmNmessageString, text, NULL);
  XmStringFree(text);

  XtManageChild(dialog);
  XtPopup(XtParent(dialog), XtGrabNone);

  while (answer == button_none) XtAppProcessEvent(app_context, XtIMAll);

  XtPopdown(XtParent(dialog));

  return (answer == button_1) ? cTRUE : cFALSE;
}

void Tell(const CppStr s)
{
  cout << s << endl;
  (void) Confirm(s.Ptr(), cFALSE);
}

//----- TextInputDialog -----------------------------------------------------------------------

static Widget text_entry_widget;
static CppStr *ptext_entry = NULL;
static bool   TextInputDialog_answer;

static void cb_TextInputDialog_ok(Widget w, XtPointer client_data, XtPointer call_data)
{
  char *text = XmTextGetString(text_entry_widget);

  assert(text != NULL);
  assert(ptext_entry != NULL);

  *ptext_entry = text;
  XtFree(text);
  TextInputDialog_answer = cTRUE;    // notify TextInputDialog() that we're done.
}

void TextInputDialog(char *titletext, char *infotext, CppStr *presult)
{
  assert(presult != NULL);
  assert(titletext != NULL  &&  infotext != NULL);

  ptext_entry = presult;  // set up pointer to destination object

  Widget dialog, pane, rowcol, actionarea;

  dialog = XtVaCreatePopupShell("dialog",
				xmDialogShellWidgetClass,  root_win->Window(),
				XmNtitle,                  titletext,
				XmNnoResize,               True,
				XmNdeleteResponse,         XmDESTROY,  // FIXME: Check! p138
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

  XtVaCreateManagedWidget(infotext, xmLabelGadgetClass, rowcol, NULL);

  text_entry_widget = XtVaCreateManagedWidget("text_input",
					      xmTextFieldWidgetClass, rowcol,
					      NULL);

  // more items should be filled in here.

  XtManageChild(rowcol);

  // Set up action area:
  static ActionAreaItem action_items[] = {
    { "  Ok  ",      cb_TextInputDialog_ok,         NULL },
  };
  action_items[0].data = (XtPointer) dialog;   // so that ok() can close dialog

  actionarea = CreateActionArea(pane, action_items, XtNumber(action_items));

  XtManageChild(pane);
  
  XmTextSetString(text_entry_widget, &((*presult)[0]));

  XtManageChild(dialog);
  XtPopup(dialog, XtGrabNone);

  TextInputDialog_answer = cFALSE;
  while (TextInputDialog_answer == cFALSE) 
    XtAppProcessEvent(app_context, XtIMAll);

  XtPopdown(dialog);
}

// EOF
