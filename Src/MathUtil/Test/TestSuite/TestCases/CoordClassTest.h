/************************************************************** ggt-head beg
 *
 * GGT: Generic Graphics Toolkit
 *
 * Original Authors:
 *   Allen Bierbaum
 *
 * -----------------------------------------------------------------
 * File:          CoordClassTest.h,v
 * Date modified: 2003/02/05 22:02:49
 * Version:       1.5
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
#ifndef _GMTL_COORD_CLASS_TEST_H_
#define _GMTL_COORD_CLASS_TEST_H_

#include <cppunit/extensions/HelperMacros.h>

namespace gmtlTest
{
   /**
    * Functionality tests
    */
   class CoordClassTest : public CppUnit::TestFixture
   {
      CPPUNIT_TEST_SUITE(CoordClassTest);

      CPPUNIT_TEST(testCoordClassTestCreation);
      CPPUNIT_TEST(testMultiArgConstructors);

      CPPUNIT_TEST_SUITE_END();

   public:
      // Correctness tests
      void testCoordClassTestCreation();
      void testMultiArgConstructors();
   };

   /**
    * Metric tests.
    */
   class CoordClassMetricTest : public CppUnit::TestFixture
   {
      CPPUNIT_TEST_SUITE(CoordClassMetricTest);

      CPPUNIT_TEST(testCoordTimingDefaultConstructor);
      CPPUNIT_TEST(testCoordTimingElementConstructor);
      CPPUNIT_TEST(testCoordTimingCopyConstructor);
      CPPUNIT_TEST(testCoordTimingGet);
      CPPUNIT_TEST(testCoordTimingOpEqual);

      CPPUNIT_TEST_SUITE_END();

   public:
      // Performance tests
      void testCoordTimingDefaultConstructor();
      void testCoordTimingElementConstructor();
      void testCoordTimingCopyConstructor();
      void testCoordTimingGet();
      void testCoordTimingOpEqual();

   };
}

#endif
