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

XtAppContext app_context;    // The Application Context

pTheScanner        scanner;
pTheRootWindow     root_win;
pCoordWindow       coord_win;
pDomainWindow      domain_win;
pRangeWindow       range_win;

pPSOptions opts = NULL;

const char *VERSION = "X-Plot version 1.34 -- the Lets-Get-The-Code-Out release";

const char *UserDir = ".";          // defaults to current dir.

//===============================================================================================
//===== UTILITY-FUNCTION DEFINITIONS ============================================================
//===============================================================================================

//
//----- LoadLibrary -----------------------------------------------------------------------------
//

void LoadLibrary(CppStr filename)
{
  if (filename == "") return;

  ifstream f(filename.Ptr());
  if (! f.good() ) {
    cout << "Could not load library '" + filename + "'." << endl;
    return;
  }

  cout << "Loading from library " << filename << endl;

  CppStr     s;
  ErrorObj   err;
  pParser    parser = new AssignmentParser(scanner, cFALSE);
  pParseTree tree = NULL;

  while (! f.eof()  &&  f.good() ) {
    f >> s;
    s.KillInitiatingSpaces().KillTerminatingSpaces();
    if (s[0] == '#') continue;    // Comment line: Skip
    if (s == "")     continue;    // Empty line:   Skip

    cout << "  " << s << endl;

    scanner->Init();
    scanner->the_input_line = s;
    tree = parser->Parse(err);
    delete tree;

    if (err==cTRUE) {
      Tell("Compilation Error while loading Library!");
      CppStr t;
      while ( (t=err()) != "@" ) cout << t << endl;
      err = cFALSE;
    }
  }
  delete parser;
  cout << "Library loaded." << endl;
}

//===============================================================================================
//===== CLASS DEFINITIONS =======================================================================
//===============================================================================================

//
//----- TheScanner ------------------------------------------------------------------------------
//

CppStr TheScanner::GetScanLine()
{
  CppStr s = the_input_line;
  the_input_line = ";";
  return s;
}

//
//
//----- Main ----------------------------------------------------------------------------------
//
//

int main(int argc, char *argv[])
{
  Widget topLevel;

  XtSetLanguageProc(NULL, NULL, NULL);

  topLevel = XtVaAppInitialize(&app_context, "XMainWindow", NULL, 0, 
			       &argc, argv, NULL, NULL);




  root_win = new TheRootWindow(topLevel);

  Widget window = XtVaCreateManagedWidget("TheRootWindow",
				   xmMainWindowWidgetClass, topLevel,
				   XmNtitle,                VERSION,
				   NULL);

  Widget menu_bar = XmCreateMenuBar(window, (char*)"menu_bar", NULL, 0);
  XtManageChild(menu_bar);
  Widget frame = XtVaCreateManagedWidget("frame", xmFrameWidgetClass, window, NULL);
  XmMainWindowSetAreas (window, menu_bar, NULL, NULL, NULL, frame);

  // Add elements to menu bar:
  //  SetupMenu();

  String translations = (char*)"<Btn3Up>:     ac_TheRootWindow(btn3)       ManagerGadgetActivate()";

  Widget area = XtVaCreateWidget((char*)"domain_area",
				 xmDrawingAreaWidgetClass, frame,
				 XmNtranslations,          XtParseTranslationTable(translations),
				 XmNwidth,                 200,
				 XmNheight,                 50,
				 NULL);

  XtManageChild(area);

  cout << VERSION << " [built " << __DATE__ << "] is starting." << endl
       << "Main programming by Martin Eskildsen, (then) Technical University of Denmark." << endl
       << "Copyright (c) 1994-2011 Martin Eskildsen" << endl
       << "X-Plot comes with ABSOLUTELY NO WARRANTY; for details read the file" << endl
       << "COPYING. This is free software, and you are welcome to redistribute it" << endl
       << "under certain conditions; again, read COPYING." << endl
       << "Comments and suggestions are welcome, e-mail to eskild.opensource@gmail.com." << endl
       << "Available command-line options: -fat         (thicker lines)," << endl
       << "                                -veryfat     (even thicker lines)," << endl
       << "                                -userdir xxx (user's directory)," << endl
       << "                                -colorps     (PS output in color)." << endl
       << endl;

  scanner       = new TheScanner;

  userdefs	= new UserDefs;
  stdconsts	= new StdConsts;
  stdfuncs	= new StdFuncs;
  InstallCalcErrorHandlers();

  for (int i=1; i<argc; i++) {
    if (strcmp(argv[i], "-fat") == 0) XDrawHandler::LineWidth = 2;          else
    if (strcmp(argv[i], "-veryfat") == 0) XDrawHandler::LineWidth = 3;      else
    if (strcmp(argv[i], "-colorps") == 0) PSDrawHandler::use_color = true;  else
    if (strcmp(argv[i], "-userdir") == 0) {
      i++;
      if (argv[i] != NULL) UserDir = argv[i];
    }
    else
      cout << "Unrecognized command line option: " << argv[i] << endl;
  }


  LoadLibrary("xplot.default.lib");  // can't be done until root_win is created

  coord_win = new CoordWindow(root_win->Window());

  domain_win = new DomainWindow(root_win->Window());

  range_win = new RangeWindow(root_win->Window());

  opts = new PSOptions();

  XtRealizeWidget(topLevel);

  cout << "Ready!" << endl;

  XtAppMainLoop(app_context);
}

// EOF
