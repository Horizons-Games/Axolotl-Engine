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

/** @file GeomType.h
	@author Jukka Jyl�nki
	@brief Specifies a type identifier enum for distinguishing between different geometry object types. */
#pragma once

MATH_BEGIN_NAMESPACE

enum GeomType
{
	GTPoint,
	GTAABB,
	GTCapsule,
	GTCircle,
	GTFrustum,
	GTLine,
	GTLineSegment,
	GTOBB,
	GTPlane,
	GTPolygon,
	GTPolyhedron,
	GTRay,
	GTSphere,
	GTTriangle
};

MATH_END_NAMESPACE
