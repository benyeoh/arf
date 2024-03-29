/************************************************************** ggt-head beg
 *
 * GGT: Generic Graphics Toolkit
 *
 * Original Authors:
 *   Allen Bierbaum
 *
 * -----------------------------------------------------------------
 * File:          gmtl-AxisAngleOps.h,v
 * Date modified: 2004/05/25 16:59:41
 * Version:       1.2
 * -----------------------------------------------------------------
 *
 *********************************************************** ggt-head end */
/*************************************************************** ggt-cpr beg
*
* GGT: The Generic Graphics Toolkit
* Copyright (C) 2001,2002 Allen Bierbaum
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
 ************************************************************ ggt-cpr end */

#ifndef _PYGMTL_AXIS_ANGLE_OPS_H_
#define _PYGMTL_AXIS_ANGLE_OPS_H_

// This file makes declarations of the templated function instantiations
// needed for Boost.Python to do its thing.

#include <gmtl/AxisAngle.h>
#include <gmtl/AxisAngleOps.h>


namespace gmtl
{
   template bool isEqual(const AxisAnglef&, const AxisAnglef&, const float);
   template bool isEqual(const AxisAngled&, const AxisAngled&, const double);
}


#endif /* _PYGMTL_AXIS_ANGLE_OPS_H_ */
