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

static void cb_unmanagechild(Widget w, XtPointer client_data, XtPointer call_data)
{
  XtUnmanageChild(w);
}

static void cb_TheRootWindow_doLoad(Widget w, XtPointer client_data, XtPointer call_data)
{
  XtUnmanageChild(w);

  char *filename;
  XmFileSelectionBoxCallbackStruct *cbs = (XmFileSelectionBoxCallbackStruct *) call_data;
  
  if (XmStringGetLtoR(cbs->value, XmFONTLIST_DEFAULT_TAG, &filename)) {

    CppStr name(filename);
    XtFree(filename);		// clean-up
  
    LoadLibrary(filename);
  }
}

#if 0

// CURRENTLY UNUSED!!!

static void cb_TheRootWindow_doSave(Widget w, XtPointer client_data, XtPointer call_data)
{
  char *filename;
  XmFileSelectionBoxCallbackStruct *cbs = (XmFileSelectionBoxCallbackStruct *) call_data;
  
  if (XmStringGetLtoR(cbs->value, XmFONTLIST_DEFAULT_TAG, &filename)) {
  
    CppStr name(filename);
    XtFree(filename);		// clean-up
  
    if (name != "") {
    
      // do actual file handling.
    
    }
  }
   
  XtUnmanageChild(w);
}

#endif


static void cb_TheRootWindow_New(Widget w, XtPointer client_data, XtPointer call_data)
{
  if (Confirm("Are you sure you want to clear the workspace?")) {
    domain_win->Clear();
    range_win->Clear();
  }
}

static void cb_TheRootWindow_Load(Widget w, XtPointer client_data, XtPointer call_data)
{
  static Widget dialog;		// so that we don't need to initialize it every time around.
  
  if (!dialog) {
    dialog = XmCreateFileSelectionDialog(w, (char*)"file_sb", NULL, 0);
    XtAddCallback(dialog, XmNokCallback, cb_TheRootWindow_doLoad, NULL);
    XtAddCallback(dialog, XmNcancelCallback, cb_unmanagechild, NULL);
    
    XmString title = XmStringCreateLocalized((char*)"Load Workspace");
    XmString dir   = XmStringCreateLocalized((char*)UserDir);
      XtVaSetValues(dialog,
		    XmNdialogTitle, title,
		    XmNdirectory,   dir,
		    NULL);
    XmStringFree(dir);
    XmStringFree(title);

  }
  XtManageChild(dialog);
  XtPopup(XtParent(dialog), XtGrabNone);
  XMapRaised(XtDisplay(dialog), XtWindow(XtParent(dialog)));
}

#if 0

// CURRENTLY UNUSED!!!

static void cb_TheRootWindow_Save(Widget w, XtPointer client_data, XtPointer call_data)
{
  static Widget dialog;		// so that we don't need to initialize it every time around.
  
  if (!dialog) {
    dialog = XmCreateFileSelectionDialog(w, "file_sb", NULL, 0);
    XtAddCallback(dialog, XmNokCallback, cb_TheRootWindow_doSave, NULL);
    XtAddCallback(dialog, XmNcancelCallback, cb_unmanagechild, NULL);

    XmString title = XmStringCreateLocalized("Save Workspace");
      XtVaSetValues(dialog, XmNdialogTitle, title, NULL);
    XmStringFree(title);
  }
  XtManageChild(dialog);
  XtPopup(XtParent(dialog), XtGrabNone);
  XMapRaised(XtDisplay(dialog), XtWindow(XtParent(dialog)));
}

#endif

static void cb_TheRootWindow_Quit(Widget w, XtPointer client_data, XtPointer call_data)
{
  if (Confirm("Are You Sure You Want To Quit?")) {
    cout << "X-Plot is ending." << endl;

    delete scanner;

    delete domain_win;
    delete range_win;
    delete coord_win;
    delete root_win;

    delete userdefs;
    delete stdconsts;
    delete stdfuncs;

    delete opts;

    cout << "Goodbye." << endl;

    exit(0);
  }
}

static void cb_TheRootWindow_Print(Widget w, XtPointer client_data, XtPointer call_data)
{
  PrintDialog();
}

static void cb_TheRootWindow_PrintSetup(Widget w, XtPointer client_data, XtPointer call_data)
{
  PrintSetupDialog();
}

#if 0

// CURRENTLY UNUSED!!!

static void cb_TheRootWindow_RefreshDisplay(Widget w, XtPointer client_data, XtPointer call_data)
{
}

#endif

//
//----- TheRootWindow ---------------------------------------------------------------------------
//

// The root window consists of a menu bar and a drawing area below.

static void ac_TheRootWindow(Widget w, XEvent *event, String args[], unsigned int *num_args)
{
  if (*num_args != 1) XtError("ac_RootWindow: Wrong number of arguments.");
  if (strcmp(args[0], "btn3") == 0) FormulaDialog();
}

TheRootWindow::TheRootWindow(Widget& TopLevel)
{
  // Create root window and menu bar:
  window = XtVaCreateManagedWidget("TheRootWindow",
				   xmMainWindowWidgetClass, TopLevel,
				   XmNtitle,                VERSION,
				   NULL);

  menu_bar = XmCreateMenuBar(window, (char*)"menu_bar", NULL, 0);
  XtManageChild(menu_bar);
  Widget frame = XtVaCreateManagedWidget("frame", xmFrameWidgetClass, window, NULL);
  XmMainWindowSetAreas (window, menu_bar, NULL, NULL, NULL, frame);

  // Add elements to menu bar:
  SetupMenu();

  // Install actions and translations and create drawing area:
  XtActionsRec actions;
  actions.string = (char*)"ac_TheRootWindow";
  actions.proc   =  ac_TheRootWindow;
  XtAppAddActions(app_context, &actions, 1);

  String translations = (char*)"<Btn3Up>:     ac_TheRootWindow(btn3)       ManagerGadgetActivate()";

  Widget area = XtVaCreateWidget("domain_area",
				 xmDrawingAreaWidgetClass, frame,
				 XmNtranslations,          XtParseTranslationTable(translations),
				 XmNwidth,                 200,
				 XmNheight,                 50,
				 NULL);

  XtManageChild(area);

  // Create a default function: f(z)=z.
  map_code = new CodeSegment;
  map_code->Emit( CodeElement( cod_loadparameter ) );
  map_code->Emit( CodeElement( cod_return ) );
  map_expression = "z";
}

TheRootWindow::~TheRootWindow()
{
}

void TheRootWindow::SetupMenu()
{
  Widget Menu, BarButton, ItemButton;

  //----- FILE MENU -----

  // First create a menu, then a button for the menu, finally attach button to menu:
  Menu = XmCreatePulldownMenu(menu_bar, (char*)"fileMenu", NULL, 0);
  BarButton = XtVaCreateManagedWidget("File    ", xmCascadeButtonWidgetClass,
            				menu_bar, NULL);
  XtVaSetValues(BarButton, XmNsubMenuId, Menu, NULL);

  // Create the items of the menu: First the item, then the call-back:
  ItemButton = XtVaCreateManagedWidget("New", xmCascadeButtonWidgetClass, Menu, NULL);
  XtAddCallback(ItemButton, XmNactivateCallback, cb_TheRootWindow_New, 0);

  ItemButton = XtVaCreateManagedWidget("Load...", xmCascadeButtonWidgetClass, Menu, NULL);
  XtAddCallback(ItemButton, XmNactivateCallback, cb_TheRootWindow_Load, 0);

  ItemButton = XtVaCreateManagedWidget("Print", xmCascadeButtonWidgetClass, Menu, NULL);
  XtAddCallback(ItemButton, XmNactivateCallback, cb_TheRootWindow_Print, 0);

  ItemButton = XtVaCreateManagedWidget("Change Printer...", 
				       xmCascadeButtonWidgetClass, Menu, NULL);
  XtAddCallback(ItemButton, XmNactivateCallback, cb_TheRootWindow_PrintSetup, 0);

/******** FIXME: CURRENTLY DISABLED!
  ItemButton = XtVaCreateManagedWidget("Save...", xmCascadeButtonWidgetClass, Menu, NULL);
  XtAddCallback(ItemButton, XmNactivateCallback, cb_TheRootWindow_Save, 0);
*********/
  ItemButton = XtVaCreateManagedWidget("Quit X-Plot    ", xmCascadeButtonWidgetClass, Menu, NULL);
  XtAddCallback(ItemButton, XmNactivateCallback, cb_TheRootWindow_Quit, 0);

  //----- VIEW MENU -----

/******** FIXME: CURRENTLY DISABLED!
  // First create a menu, then a button for the menu, finally attach button to menu:
  Menu = XmCreatePulldownMenu(menu_bar, "viewMenu", NULL, 0);
  BarButton = XtVaCreateManagedWidget("View    ", xmCascadeButtonWidgetClass,
            				menu_bar, NULL);
  XtVaSetValues(BarButton, XmNsubMenuId, Menu, NULL);

  // Create the items of the menu: First the item, then the call-back:
  ItemButton = XtVaCreateManagedWidget("Refresh display   ", xmCascadeButtonWidgetClass, Menu, NULL);
  XtAddCallback(ItemButton, XmNactivateCallback, cb_TheRootWindow_RefreshDisplay, 0);
*********/

}

pCodeSegment TheRootWindow::GetMapCodeSegment()
{
  return map_code;
}

Widget TheRootWindow::Window()
{
  return window;
}

void TheRootWindow::ParseMapExpression(bool recalc)
{
  scanner->Init();

  scanner->the_input_line = CppStr("MaP_FuNc(z)=") + map_expression;

  ErrorObj   err;
  pParser    parser = new AssignmentParser(scanner, cFALSE);
  pParseTree tree   = parser->Parse(err);
  delete tree;
  delete parser;

  DynaIdx i = userdefs->LinearFind("MaP_FuNc");

  if (err==cTRUE  ||  i == -1) {
    Tell("Compilation Error!");
    CppStr t;
    while ( (t=err()) != "@" ) cout << t << endl;
    return;
  }
  map_code = (*userdefs)[i].value.pCode;

  if (recalc) {
    range_win -> Clear();
    domain_win->Busy(cTRUE);
    pGraphicObjectBase obj = domain_win->GetFirst();
    while (obj != NULL) {
      pComplexPointsSegmentList list = new ComplexPointsSegmentList;
      assert(list != NULL);
      obj->DrawToMap(*list, map_code, err);
      if (err != cTRUE)
	range_win->AddPoints(list);
      obj = domain_win->GetNext();
    }
    domain_win->Busy(cFALSE);
    if (err == cTRUE) {
      Tell("Couldn't calculate all points.");
      domain_win->SetIdleState();
      return;
    }
  }
}

// EOF
