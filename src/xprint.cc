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
//----- PrintDialog ---------------------------------------------------------------------------
//

void PrintDialog()
{
  if (!Confirm("Are you sure you want to Print?")) return;

  CppStr result = "f(z)=" + root_win->map_expression;

  TextInputDialog("X-Plot Graph Description", "Enter Text; use `#' for newline: ", 
		  &result);

  CppStr org_opts_outputfilename = opts->outputfilename;

  if (opts->outputfilename == "")         // use temporary file
    opts->outputfilename = mkstemp("xplot-XXXXXX");

  opts->outputfilename = CppStr(UserDir) + "/" + opts->outputfilename;

  pPSDrawHandler dom = new PSDrawHandler(cTRUE, *opts, getlogin(), result);
  pPSDrawHandler ran = new PSDrawHandler(cFALSE, *opts, getlogin(), result);

  pDrawHandler old_dom = domain_win->SetDrawHandler(dom);
  pDrawHandler old_ran = range_win->SetDrawHandler(ran);

  ran->AttachToFile( dom->CreateFile() );
  dom->CreatePrologue();

  dom->SetLocation();
  domain_win->Redraw(0);
  dom->ResetLocation();

  ran->SetLocation();
  range_win->Redraw(0);
  ran->ResetLocation();

  dom->CreateEpilogue().CloseFile();
  domain_win->SetDrawHandler(old_dom);
  range_win->SetDrawHandler(old_ran);

  delete dom;
  delete ran;

  cout << "Created PostScript file " << opts->outputfilename << endl;

  if (opts->command_prefix != "") {
    CppStr cmd = opts->command_prefix + " " + opts->outputfilename;
    cout << "Executing: " << cmd << endl;
    system(cmd.Ptr());
  }

  opts->outputfilename = org_opts_outputfilename;
}

//
//----- PrintSetupDialog ----------------------------------------------------------------------
//

void PrintSetupDialog()
{
  TextInputDialog("X-Plot Printer Setup", "Unix Print Command: ",
		  &opts->command_prefix);
}

