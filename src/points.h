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

#ifndef __POINTS_H__
#define __POINTS_H__

#include "dynaarra.h"
#include "code.h"
#include <Complex.h>

typedef DynaArray<Complex>     ComplexPointsSegment;
typedef ComplexPointsSegment* pComplexPointsSegment;
typedef ComplexPointsSegment& rComplexPointsSegment;

CLASSDEF(ComplexPointsSegmentList);
class ComplexPointsSegmentList {
public:
	ComplexPointsSegmentList() : color(0), segmentlist(0, cFALSE)
	{ }

	long				color;
	DynaArray<ComplexPointsSegment>	segmentlist;
};

// Every object that can be plotted consists of a list of segments, each segment
// containing a list of Complex values (points in the Complex space). As an
// example, consider the grid: # . Each line in the grid is a ComplexPointsSegment,
// and the entire grid is represented by four of these segments contained in one
// ComplexPointsSegmentList. It is very IMPORTANT to note that the ComplexPoints-
// SegmentsList should be constructed with the 'delete_when_destroyed' flag set to
// cFALSE. This allows the List to grow without problems. But, care should be taken
// to 'Empty()' the List before destroying it  -  otherwise, memory will be lost.

#endif

// EOF
