/************************************************************** ggt-head beg
 *
 * GGT: Generic Graphics Toolkit
 *
 * Original Authors:
 *   Allen Bierbaum
 *
 * -----------------------------------------------------------------
 * File:          Vec.h,v
 * Date modified: 2004/11/12 01:34:49
 * Version:       1.21
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
#ifndef _GMTL_VEC_H_
#define _GMTL_VEC_H_

#include <MathUtil/gmtl/Defines.h>
#include <MathUtil/gmtl/Config.h>
#include <MathUtil/gmtl/VecBase.h>
#include <MathUtil/gmtl/Util/StaticAssert.h>

namespace gmtl
{

/**
 * A representation of a vector with SIZE components using DATA_TYPE as the data
 * type for each component.
 *
 * @param DATA_TYPE     the datatype to use for the components
 * @param SIZE          the number of components this VecBase has
 * @see Vec3f
 * @see Vec4f
 * @see Vec3d
 * @see Vec4f
 * @ingroup Types
 */
template<class DATA_TYPE, unsigned SIZE, bool isAligned=false>
#ifdef GMTL_NO_METAPROG
class Vec : public VecBase<DATA_TYPE, SIZE>
#else
class Vec : public VecBase<DATA_TYPE, SIZE, meta::DefaultVecTag, isAligned>
#endif
{
public:
   /// The datatype used for the components of this Vec.
   typedef DATA_TYPE DataType;

   /// The number of components this Vec has.
   enum Params { Size = SIZE };

   /// The superclass type.
   typedef VecBase<DATA_TYPE, SIZE, meta::DefaultVecTag, isAligned> BaseType;
   typedef Vec<DATA_TYPE, SIZE, isAligned> VecType;

public:
   /**
    * Default constructor. All components are initialized to zero.
    */
   Vec()
   {
      //for (unsigned i = 0; i < SIZE; ++i)
      //   this->mData[i] = (DATA_TYPE)0;
   }

   /// @name Value constructors
   //@{
   /**
    * Make an exact copy of the given Vec object.
    * @pre  Vector should be the same size and type as the one copied
    * @param rVec    the Vec object to copy
    */
   /*
   Vec( const Vec<DATA_TYPE, SIZE>& rVec )
      : BaseType( static_cast<BaseType>( rVec ) )
   {;}
   */

#ifdef GMTL_NO_METAPROG
   Vec( const VecBase<DATA_TYPE, SIZE, isAligned>& rVec )
      : BaseType( rVec )
   {
   }
#else
   template<typename REP2>
   Vec( const VecBase<DATA_TYPE, SIZE, REP2, isAligned>& rVec )
      : BaseType( rVec )
   {
   }
#endif

   /**
    * Creates a new Vec initialized to the given values.
    */
   Vec(const DATA_TYPE& val0,const DATA_TYPE& val1)
   : BaseType(val0, val1)
   {
      GMTL_STATIC_ASSERT( SIZE == 2, Out_Of_Bounds_Element_Access_In_Vec );
   }

   Vec(const DATA_TYPE& val0,const DATA_TYPE& val1,const DATA_TYPE& val2)
   : BaseType(val0, val1, val2)
   {
      GMTL_STATIC_ASSERT( SIZE == 3, Out_Of_Bounds_Element_Access_In_Vec );
   }

   Vec(const DATA_TYPE& val0,const DATA_TYPE& val1,const DATA_TYPE& val2,const DATA_TYPE& val3)
   : BaseType(val0, val1, val2, val3)
   {
      GMTL_STATIC_ASSERT( SIZE == 4, Out_Of_Bounds_Element_Access_In_Vec );
   }
   //@}

   /** Assign from different rep. */
#ifdef GMTL_NO_METAPROG
   inline VecType& operator=(const VecBase<DATA_TYPE,SIZE,isAligned>& rhs)
   {
      BaseType::operator=(rhs);
      return *this;
   }
#else
   template<typename REP2>
   inline VecType& operator=(const VecBase<DATA_TYPE,SIZE,REP2,isAligned>& rhs)
   {
      BaseType::operator=(rhs);
      return *this;
   }
#endif
};

// --- helper types --- //
typedef Vec<int, 2> Vec2i;
typedef Vec<float,2> Vec2f;
typedef Vec<double,2> Vec2d;
typedef Vec<int, 3> Vec3i;
typedef Vec<float,3> Vec3f;
typedef Vec<double,3> Vec3d;
typedef Vec<int, 4> Vec4i;
typedef Vec<float,4> Vec4f;
typedef Vec<double,4> Vec4d;

typedef Vec<float, 4, true> VecA4f;
typedef Vec<float, 3, true> VecA3f;
typedef Vec<int, 4, true> VecA4i;
typedef Vec<int, 3, true> VecA3i;

#define _CAST_VEC2(vec) *((gmtl::Vec2f*)&vec)
#define _CAST_VEC3(vec) *((gmtl::Vec3f*)&vec)
#define _CAST_VEC4(vec) *((gmtl::Vec4f*)&vec)
#define _CAST_VECA3(vec) *((gmtl::VecA3f*)&vec)
#define _CAST_VECA4(vec) *((gmtl::VecA4f*)&vec)

#define _CAST_VEC3I(vec) *((gmtl::Vec3i*)&vec)
#define _CAST_VEC4I(vec) *((gmtl::Vec4i*)&vec)
#define _CAST_VECA3I(vec) *((gmtl::VecA3i*)&vec)
#define _CAST_VECA4I(vec) *((gmtl::VecA4i*)&vec)

}

#endif
