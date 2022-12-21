/* Copyright Jukka Jylänki

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

/** @file BitFuncs.h
	@author Jukka Jylänki
	@brief */
#pragma once

static inline int CountTrailingZeroes32(uint32_t x)
{
#ifdef _MSC_VER
	unsigned long ret;
	_BitScanForward(&ret, x);
	return (int)ret;
#else // TODO: be more precise here
	return __builtin_ctz(x);
#endif
}

// TODO: Other bit twiddling related functions.
