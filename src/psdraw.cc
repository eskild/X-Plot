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

#include "psdraw.h"
#include "xdraw.h"
#include <stdio.h>

//****************************************************************************
// PSOptions:
//
// Features:
//	Various options for the PostScript printout settings.
//	Automatically loads setup from file "xplot.print".
//
//
//
//
//****************************************************************************

static void err(CppStr msg, CppStr line)
{
  cout << "xplot.print: " << msg << endl << line << endl;
}

static void err(CppStr msg)
{
  cout << "xplot.print: " << msg << endl;
}

static void Decode(CppStr s, double &d)
{
  float t;   // Can't be double!
  if (sscanf(s.Ptr(), "%f", &t) == 1) {
    d = Abs(t);
    if (s[s.len()-1] != '"') d /= 2.54;
  }
  else
    err("This isn't a real number:", s);
}

static void DecodePure(CppStr s, double &d)
{
  float t;   // Can't be double!
  if (sscanf(s.Ptr(), "%f", &t) == 1)
    d = Abs(t);
  else
    err("This isn't a real number:", s);
}

PSOptions::PSOptions()
: dpi(72), paper_width(8.27), paper_height(10.94), margin_top(1),
  margin_bottom(1/3), margin_left(1/3), margin_right(1/3)
{
#ifdef __BORLANDC__
  ifstream f("xplot.prt");
#else
  ifstream f("xplot.print");
#endif

  if (!f.good()) {
    err("Could not open file! Using default values.");
    return;
  }

  CppStr s;
  while (f.good()  &&  !f.eof()) {
    f >> s;
    s.SubstituteChar('\t', ' ').KillInitiatingSpaces();
    if (s.len() == 0  ||  s[0] == '#') continue;

    int equ_pos = s.CharPos('=');
    if (equ_pos > 0) {
      CppStr label   = s.SubString(0, equ_pos-1);
      CppStr setting = s.SubString(equ_pos+1, s.len()-1);
      label.KillInitiatingSpaces().KillTerminatingSpaces().UpCase();
      setting.KillInitiatingSpaces().KillTerminatingSpaces();

      if (label=="PRINT_COMMAND_PREFIX") command_prefix = setting; else
      if (label=="PRINT_OUTPUT_FILE")    outputfilename = setting; else
      if (label=="PRINT_PS_COORDSETUP")  ps_coord_setup = setting; else
      if (label=="PRINT_DPI")            DecodePure(setting, dpi); else
      if (label=="PRINT_PAPER_WIDTH")    Decode(setting, paper_width); else
      if (label=="PRINT_PAPER_HEIGHT")   Decode(setting, paper_height); else
      if (label=="PRINT_MARGIN_TOP")     Decode(setting, margin_top); else
      if (label=="PRINT_MARGIN_BOTTOM")  Decode(setting, margin_bottom); else
      if (label=="PRINT_MARGIN_LEFT")    Decode(setting, margin_left); else
      if (label=="PRINT_MARGIN_RIGHT")   Decode(setting, margin_right); else
      if (label=="PRINT_MARGIN_WINDOWS") Decode(setting, margin_windows);
      else
	err("Unknown label:", s);
    }
    else err("Bad line:", s);
  }

  if (paper_width < paper_height) {
    err("Paper Width should be greater han Paper Height (values have been swapped).");
    double t = paper_width; paper_width = paper_height; paper_height = t;
  }

  work_width  = paper_width - margin_left - margin_right - margin_windows;
  work_height = paper_height - margin_top - margin_bottom;

  if (work_width  < 5.0) err("Resulting width available for display < 5 inches. Reduce margins.");
  if (work_height < 5.0) err("Resulting height available for display < 5 inches. Reduce margins.");

  work_width  = Max(work_width,  5.0);
  work_height = Max(work_height, 5.0);

  // Note: The requirement that width>=height can sometimes be overruled by setting large margins
  // That is ok.

  range_side_length  = Min(work_height, work_width*2/3);
  domain_side_length = work_width*1/3;

  f.close();
}

double PSOptions::Origo_X(bool domain)
{
  return domain ? margin_left : (margin_left + domain_side_length + margin_windows);
}

double PSOptions::Origo_Y()
{
  return margin_bottom;
}

double PSOptions::WindowSideLength(bool domain)
{
  return domain ? domain_side_length : range_side_length;
}

//****************************************************************************
// PSDrawHandler
//
// Features:
//	Handles the generation of drawing commands, and writes them to a
//	file.
//
//
//
//
//****************************************************************************

static const DisplayPoint UNLIKELY(32767, 32767);

bool PSDrawHandler::use_color = false;

PSDrawHandler& PSDrawHandler::ps_moveto_lineto(const DisplayPoint &p1, const DisplayPoint &p2)
{
  if (last_coord == p1) 
    ps_lineto(p2);
  else
    if (last_coord == p2) 
      ps_lineto(p1);
    else
      ps_moveto(p1).ps_lineto(p2);

  return *this;
}

PSDrawHandler& PSDrawHandler::ps_moveto(const DisplayPoint &p)
{
  if (last_coord != p) {
    last_coord = p;
    *psfile << ' ' << p.x << ' ' << p.y << " M ";
  }
  return *this;
}

PSDrawHandler& PSDrawHandler::ps_lineto(const DisplayPoint &p)
{
  last_coord = p;
  *psfile << p.x << ' ' << p.y << " L" << endl;
  return *this;
}

PSDrawHandler& PSDrawHandler::ps_rlineto(const DisplayPoint &p)
{
  last_coord += p;
  *psfile << ' ' << p.x << ' ' << p.y << " R ";
  return *this;
}

PSDrawHandler& PSDrawHandler::ps_stroke()
{
  *psfile << " S N" << endl;
  last_coord = UNLIKELY;
  return *this;
}

PSDrawHandler& PSDrawHandler::ps_newpath()
{
  *psfile << " N ";
  return *this;
}

PSDrawHandler& PSDrawHandler::ps_closepath()
{
  *psfile << " C ";
  return *this;
}

// Actual Drawhandler routines:

PSDrawHandler::PSDrawHandler(bool d, rPSOptions o, const CppStr &cre, const CppStr &txt)
: DrawHandler(new LinearDisplayMapper2), psfile(0), domain(d), opts(o),
  text(txt), creator(cre)
{
  last_coord = UNLIKELY;
  int side_len = int(opts.In_to_Dots(opts.WindowSideLength(domain)));
  mapper->SetScreenGeometry( DisplayPoint( side_len, side_len ), (int)opts.dpi );

  rgb_info[ 0] = RGB_Info(0.0, 0.0, 0.0);
  rgb_info[ 1] = RGB_Info(0.5, 0.0, 0.0);
  rgb_info[ 2] = RGB_Info(0.0, 0.5, 0.0);
  rgb_info[ 3] = RGB_Info(0.0, 0.0, 0.5);
  rgb_info[ 4] = RGB_Info(0.5, 0.5, 0.0);
  rgb_info[ 5] = RGB_Info(0.0, 0.5, 0.5);
  rgb_info[ 6] = RGB_Info(0.5, 0.0, 0.5);
  rgb_info[ 7] = RGB_Info(1.0, 1.0, 0.0);
  rgb_info[ 8] = RGB_Info(0.0, 1.0, 1.0);
  rgb_info[ 9] = RGB_Info(1.0, 0.0, 1.0);
}

PSDrawHandler::~PSDrawHandler()
{
}

DrawHandler& PSDrawHandler::BeginDrawing()
{
  *psfile << " N ";
  return *this;
}

DrawHandler& PSDrawHandler::EndDrawing()
{
  return ps_stroke();
}

PSDrawHandler& PSDrawHandler::ReformatText()
{
  CppStr s = text;
  int i;

  for (i=0; i<s.len(); i++)             // Fix paranthesis
    if (s[i]=='('  ||  s[i]==')') {
      s.Insert("\\", i);
      i += 1;
    }

  // Fix newlines and operators:
  for (i=0; i<s.len(); i++)
    if (s[i] == '#') {
      s.Delete(i,1);
      if (i < s.len()) s.Insert(" ) w {u} h\n(", i);
    }
    else if (s[i]=='+'  ||  s[i]=='-'  ||  s[i]=='*'  ||  s[i]=='/') {
      char str[4] = " x ";  str[1] = s[i];
      s.Delete(i,1);
      s.Insert(str, i);
      i += 3;
    }

  *psfile << "(" << s << " ) w {u} h" << endl;

  return *this;
}

bool PSDrawHandler::Good()
{
  return bool(psfile  &&  psfile->good());
}

ofstream* PSDrawHandler::CreateFile()
{
  psfile = new ofstream(opts.outputfilename.Ptr());
  return psfile;
}

PSDrawHandler& PSDrawHandler::CloseFile()
{
  psfile->close();
  delete psfile;
  return *this;
}

PSDrawHandler& PSDrawHandler::AttachToFile(ofstream *f)
{
  psfile = f;
  return *this;
}

PSDrawHandler& PSDrawHandler::CreatePrologue()
{
  double square_len   = opts.Cm_to_Dots(0.1);
  double FntHeight    = opts.Cm_to_Dots(0.25);
  double BigFntHeight = opts.Cm_to_Dots(0.4);

  *psfile
  	 << "%!PS-Adobe-2.0" << endl
	 << "%%Creator: X-Plot " << VERSION << " Copyright (C) 1996-2011 Martin Eskildsen" << endl
	 << "gsave"		<< endl
	 << "newpath"		<< endl
	 << "/PAPERWIDTH " << opts.In_to_Dots(opts.paper_width) << " def" << endl
	 << "/PAPERHEIGHT " << opts.In_to_Dots(opts.paper_height) << " def" << endl
	 << "/PAPERMARGINTOP " << opts.In_to_Dots(opts.margin_top) << " def" << endl
	 << "/PAPERMARGINBOTTOM " << opts.In_to_Dots(opts.margin_bottom) << " def" << endl
	 << "/PAPERMARGINLEFT " << opts.In_to_Dots(opts.margin_left) << " def" << endl
	 << "/PAPERMARGINRIGHT " << opts.In_to_Dots(opts.margin_right) << " def" << endl
	 << "/PAPERMARGINWINDOWS " << opts.In_to_Dots(opts.margin_windows) << " def" << endl
         << "/DPI " << opts.dpi << " def" << endl
	 << "/SIDELENGTHDOMAIN " << opts.In_to_Dots(opts.WindowSideLength(cTRUE)) << " def" << endl
	 << "/SIDELENGTHRANGE  " << opts.In_to_Dots(opts.WindowSideLength(cFALSE)) << " def" << endl
	 << "/M { moveto }    def" 	<< endl
	 << "/L { lineto }    def" 	<< endl
	 << "/R { rlineto }   def"	<< endl
	 << "/S { stroke }    def" 	<< endl
	 << "/N { newpath }   def"	<< endl
	 << "/C { closepath } def"	<< endl
	 << "/LW { " << opts.Cm_to_Dots(0.005) << " setlinewidth } def" << endl
	 << "/FNT     { /Times-Roman findfont " << FntHeight << " scalefont } def" << endl
	 << "/BIGFNT  { /Times-Roman findfont " << BigFntHeight << " scalefont } def" << endl
	 << "/BX { newpath moveto " << square_len << " 0 rlineto 0 " << square_len
         << " rlineto " << -square_len << " 0 rlineto 0 " << -square_len << " rlineto" << endl
	 << "      C LW S } def" << endl
	 << "/SETBORDERPATH { newpath 0 0 moveto dup 0 lineto dup dup lineto 0 exch lineto closepath } def" << endl

         << "%" << endl
         << "% Fixed right-margin text output routing. From the book \"Postscript" << endl
         << "% programmering\", pg. 106, by Erwin Neutzsky-Wull, Borgens Forlag 1990." << endl
         << "%" << endl
         << "/h {/i 0 def /j exch def /k exch def /l exch def /m ( )" << endl
         << "stringwidth pop def /n 0 def /o 0 def /p 0 def /q l def {q ( )" << endl
         << "search {/r exch def pop /q exch def /s r stringwidth pop def n s" << endl
         << "add k gt {l p o p sub getinterval j /p o def /n s m add def } {/n" << endl
         << "n s add m add def} ifelse /o o r length add 1 add def} {/i 1 def" << endl
         << "pop exit} ifelse} loop /t l length def l p t p sub getinterval j}" << endl
         << "def /u {0 v -1 mul rmoveto gsave i 0 eq {w n div 1 scale} if show" << endl
         << "grestore} def" << endl << endl

	 << "% Coord. setup:" << endl
	 << (72.0/opts.dpi) << " dup scale" << endl
	 << opts.ps_coord_setup << endl

	 << "0 setlinecap 0 setlinejoin 5 setmiterlimit" << endl

	 << "/w SIDELENGTHDOMAIN def /v " << BigFntHeight << " def" << endl
	 << "PAPERMARGINLEFT PAPERHEIGHT PAPERMARGINTOP sub " << BigFntHeight
         << " 4 mul sub M BIGFNT setfont" << endl;

  ReformatText();

  *psfile
         << "PAPERMARGINLEFT PAPERHEIGHT PAPERMARGINTOP sub " << FntHeight << " sub M "
         << "FNT setfont (Generated by " << VERSION << ") show" << endl
	 << "PAPERMARGINLEFT PAPERHEIGHT PAPERMARGINTOP sub " << FntHeight << " 2 mul sub M "
	 << "FNT setfont (Copyright (C) 1996-2011 Martin Eskildsen) show" << endl
	 << "PAPERMARGINLEFT PAPERHEIGHT PAPERMARGINTOP sub " << FntHeight << " 4 mul sub M "
         << "FNT setfont (User " << creator << ") show" << endl

	 << endl;

  *psfile << "% End of prologue" << endl;
  return *this;
}

PSDrawHandler& PSDrawHandler::CreateEpilogue()
{
  *psfile << "% Epilogue begins here:" << endl
	  << "S showpage" 	<< endl

	  << "grestore"          << endl
	  << endl;

  return *this;
}

PSDrawHandler& PSDrawHandler::SetLocation()
{
  *psfile << "% Creating " << (domain ? "domain" : "range") << " work-area:" << endl
          << (domain ? "gsave " : "")
          << opts.In_to_Dots(opts.Origo_X(domain)) << ' '
          << opts.In_to_Dots(opts.Origo_Y()) << " translate "
          << opts.In_to_Dots(opts.WindowSideLength(domain))
          << " dup SETBORDERPATH " << opts.Cm_to_Dots(0.1)
          << " setlinewidth S SETBORDERPATH clip newpath LW" << endl;
  return *this;
}

PSDrawHandler& PSDrawHandler::ResetLocation()
{
  *psfile << "% Release work-area:" << endl
          << -opts.In_to_Dots(opts.Origo_X(domain)) << ' '
	  << -opts.In_to_Dots(opts.Origo_Y()) << " translate " << (domain ? "grestore" : "")
          << endl;
  return *this;
}

DrawHandler& PSDrawHandler::AttachToWindow(pBaseWindow)
{
  return *this;
}

DrawHandler& PSDrawHandler::Clear()
{
  return *this;
}

DrawHandler& PSDrawHandler::SelectColor(long index)
{
  DrawHandler::SelectColor(index);

  if (use_color) {
    index = index % MAX_PS_COLORS;

    *psfile <<
      ' ' << rgb_info[index].red << ' ' <<
      ' ' << rgb_info[index].green << ' ' <<
      ' ' << rgb_info[index].blue << " setrgbcolor" << endl;
  }

  return *this;
}

DrawHandler& PSDrawHandler::SelectColorBlack()
{
  DrawHandler::SelectColorBlack();
  *psfile << "0.5 setgray" << endl;
  return *this;
}

DrawHandler& PSDrawHandler::SelectColorWhite()
{
  DrawHandler::SelectColorWhite();
  *psfile << "0 setgray" << endl;
  return *this;
}

DrawHandler& PSDrawHandler::SetLinestyle(Linestyle s)
{
  DrawHandler::SetLinestyle(s);

  if (s == solid)
    *psfile << "S [] 0 setdash" << endl;
  else
    *psfile << "S [" << Cm_to_Dots(0.1) << ' ' << Cm_to_Dots(0.05) << "] 0 setdash" << endl;

  return *this;
}

DrawHandler& PSDrawHandler::DrawCross(const Complex &)
{
  return *this;
}

DrawHandler& PSDrawHandler::DrawArrow(const Complex &p1, const Complex &p2)
{
  DisplayPoint from, to_centre, to_end;

  CalcArrowPoints(p1, p2, from, to_centre, to_end);

  ps_moveto(from).ps_lineto(to_centre).ps_lineto(to_end).ps_stroke();

  return *this;
}

DrawHandler& PSDrawHandler::DrawLine(const Complex &p1, const Complex &p2)
{
  DisplayPoint from = Map(p1);
  DisplayPoint to   = Map(p2);

  ps_moveto_lineto(from, to);

  if (arrowstyle != none) DrawArrow(p1, p2);

  return *this;
}

DrawHandler& PSDrawHandler::DrawRectangle(const Complex &p1, const Complex &p2)
{
  DisplayPoint d1 = Map(p1);
  DisplayPoint d2 = Map(p2);

  *psfile << "% Rectangle " << p1 << ' ' << p2 << endl;
  ps_stroke();
  ps_moveto(d1).ps_lineto( DisplayPoint(d2.x, d1.y) );
  ps_lineto(d2).ps_lineto( DisplayPoint(d1.x, d2.y) );
  ps_lineto(d1).ps_closepath().ps_stroke();

  if (arrowstyle != none)
    DrawArrow(p1, Complex( ::real(p2), ::imag(p1) ) );

  return *this;
}

DrawHandler& PSDrawHandler::DrawCircle(const Complex &c, const double &radius)
{
  DisplayPoint d_c = Map(c);
  DisplayPoint d_r = Map(c+Complex(radius,0));
  int pixel_radius = Abs(d_r.x - d_c.x);

  *psfile << "N " << d_c.x << ' ' << d_c.y << ' ' << pixel_radius << " 0 360 arc S" << endl;

  if (arrowstyle != none)				// circles are ccw.
    DrawArrow(c+Complex(radius,radius), c+Complex(-radius,radius));

  return *this;
}

DrawHandler& PSDrawHandler::DrawDot(const Complex &p)
{
  DisplayPoint c = Map(p);

  *psfile << c.x << ' ' << c.y << " BX ";

  return *this;
}

DrawHandler& PSDrawHandler::DrawText(int, const CppStr&)
{
  return *this;
}

DrawHandler& PSDrawHandler::DrawText(const Complex &lo_left, const CppStr& s)
{
  DisplayPoint dp = Map(lo_left);
  *psfile << dp.x << ' ' << dp.y << " M FNT setfont (" << s << ") show"
	  << endl;
  return *this;
}

DrawHandler& PSDrawHandler::Centre(const Complex &c)
{
  DrawHandler::Centre(c);
  return *this;
}

// EOF
