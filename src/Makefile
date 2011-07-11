#    X-Plot  --  a program for visualizing complex functions
#
#    Copyright (C) 1996-2011 Martin Eskildsen.
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
#    Comments, suggestions, questions to:
#            Martin Eskildsen
#            eskild.opensource@gmail.com
#
#
#    Programming by Martin Eskildsen, (then) student at the
#    Technical University of Denmark.

.PHONY:		all install clean distclean documentation

all:		xplot

CXX		= g++
TEX		= latex

CXXLIBS		= -L. -lXm -lXt -lX11 -lm
CXXFLAGS	= -g -I. -Wall -Wno-write-strings

# -Wno-write-strings to make G++ silent on the matter of X11 function prototypes with
# (char*) params being passed string constants. The X11 funcs usually won't be modifying
# the strings anyway, so we'll just make the compiler stop complaining.

SRCS		= \
		x.cc \
		actionarea.cc \
		xdraw.cc \
		xdomain.cc \
		xdomainwin.cc \
		xrange.cc \
		xrangewin.cc \
		xformuladialog.cc \
		xcoordwin.cc \
		xprint.cc \
		xrootwin.cc \
		xdialogs.cc \
		xparamplot.cc \
		calcenv.cc \
		userdefs.cc \
		stdfuncs.cc \
		stdconst.cc \
		scanner.cc \
		parsetre.cc \
		parser.cc \
		mappers.cc \
		graphobj.cc \
		error.cc \
		fifolist.cc \
		dbgfile.cc \
		cppstr.cc \
		code.cc \
		basewin.cc \
		psdraw.cc

DEPSDIR		= ../.deps
OBJDIR		= ../.obj
DIRS		= $(DEPSDIR) $(OBJDIR)
DOCDIR		= ../doc

OBJS		= $(patsubst %.cc,$(OBJDIR)/%.o,$(filter %.cc,$(SRCS)))
DEPS		= $(patsubst %.cc,$(DEPSDIR)/%.d,$(filter %.cc,$(SRCS)))

INSTALL_DIR	= ../install

$(DEPSDIR)/%.d:	%.cc $(DEPSDIR)
	$(CXX) -MM $(CPPDEPSFLAGS) $< | sed 's+$(notdir $*)\.o[ :]*+$(OBJDIR)/$*\.o: +g' > $@

$(OBJDIR)/%.o: %.cc
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) -o $@ $<

$(DOCDIR)/%.dvi: $(DOCDIR)/%.tex
	$(TEX) -output-directory=$(DOCDIR) $<

xplot:	$(DIRS) $(OBJS)
	$(CXX) $(CXXFLAGS) $(CXXLINKFLAGS) -o $@ $(OBJS) $(CXXLIBS)

ifeq "$(strip $(filter distclean clean, $(MAKECMDGOALS)))" ""
-include	$(DEPS)
endif

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(DEPSDIR):
	mkdir -p $(DEPSDIR)

documentation:	$(DOCDIR)/howto.dvi

install:
	$(MAKE) xplot
	$(MAKE) documentation
	mkdir -p $(INSTALL_DIR)
	rm -f $(INSTALL_DIR)/*
	cp xplot $(DOCDIR)/howto.dvi ../run/xplot.default.lib ../run/xplot.print $(INSTALL_DIR)

clean:
	rm -rf $(OBJDIR) *~ core xplot\
		$(DOCDIR)/howto.aux \
		$(DOCDIR)/howto.dvi \
		$(DOCDIR)/howto.log

distclean:
	$(MAKE) clean
	rm -rf $(DEPSDIR) $(INSTALL_DIR)

# EOF
