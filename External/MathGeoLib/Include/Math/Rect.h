/* Copyright Jukka Jyl�nki

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

/** @file Rect.h
	@author Jukka Jyl�nki
	@brief 2D integral axis-aligned rectangle, equivalent to RECT in Windows API. */
#pragma once

// Define this when building this project to make Rects be transformable to Win32 RECT() structs.
// #define MATH_WIN32_INTEROP

#if defined(WIN32) && defined(MATH_WIN32_INTEROP)
#include "../Math/InclWindows.h"
#endif

MATH_BEGIN_NAMESPACE

/// A 2D integral (x,y),(w,h) -rectangle.
class Rect
{
public:
	Rect() { left = top = right = bottom = 0; }
	Rect(int left_, int top_, int width, int height)
		:left(left_), top(top_), right(left + width), bottom(top + height) {}
//	~Rect() {}

	int Width() { return right - left; }
	int Height() { return bottom - top; }

	int left;
	int top;
	int right;
	int bottom;

#if defined(WIN32) && defined(MATH_WIN32_INTEROP)
	operator RECT()
	{
		RECT r;
		r.top = top;
		r.left = left;
		r.right = right;
		r.bottom = bottom;
		return r;
	}
	Rect(const RECT &r)
	{
		top = r.top;
		left = r.left;
		bottom = r.bottom;
		right = r.right;
	}
#endif
};

MATH_END_NAMESPACE
