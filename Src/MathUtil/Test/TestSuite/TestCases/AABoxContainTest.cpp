/************************************************************** ggt-head beg
 *
 * GGT: Generic Graphics Toolkit
 *
 * Original Authors:
 *   Allen Bierbaum
 *
 * -----------------------------------------------------------------
 * File:          AABoxContainTest.cpp,v
 * Date modified: 2003/02/05 02:21:17
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
#include "AABoxContainTest.h"
#include "../Suites.h"
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/MetricRegistry.h>

#include <gmtl/Containment.h>

namespace gmtlTest
{
   CPPUNIT_TEST_SUITE_REGISTRATION(AABoxContainTest);
   CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(AABoxContainMetricTest, Suites::metric());

   void AABoxContainTest::testIsInVolumePt()
   {
      // Test empty box
      gmtl::AABoxf box;
      gmtl::Point3f origin;
      CPPUNIT_ASSERT(! gmtl::isInVolume(box, origin));

      // Test valid box with pt outside
      gmtl::AABoxf box2(gmtl::Point3f(-1,-1,-1), gmtl::Point3f(1,1,1));
      gmtl::Point3f pt1(2,2,2);
      CPPUNIT_ASSERT(! gmtl::isInVolume(box2, pt1));

      //Test valid vox with pt inside
      CPPUNIT_ASSERT(gmtl::isInVolume(box2, origin));
   }

   void AABoxContainMetricTest::testTimingIsInVolumePt()
   {
      gmtl::AABoxf box(gmtl::Point3f(-1,-1,-1), gmtl::Point3f(1,1,1));
      gmtl::Point3f origin;
      const long iters(400000);
      float use_value(0);
      CPPUNIT_METRIC_START_TIMING();

      for(long iter=0;iter<iters; ++iter)
      {
         gmtl::isInVolume(box, origin);
         use_value = use_value + box.mMin[0] + 2.0f;
      }

      CPPUNIT_METRIC_STOP_TIMING();
      CPPUNIT_ASSERT_METRIC_TIMING_LE("AABoxContainTest/IsInVolumePt", iters, 0.075f, 0.1f);  // warn at 7.5%, error at 10%

      CPPUNIT_ASSERT(use_value > 0.0f);
   }

   void AABoxContainTest::testIsInVolumeAABox()
   {
      // Test valid box against empty box
      gmtl::AABoxf empty;
      gmtl::AABoxf box(gmtl::Point3f(-1,-1,-1), gmtl::Point3f(1,1,1));
      CPPUNIT_ASSERT(! gmtl::isInVolume(empty, box));
      CPPUNIT_ASSERT(! gmtl::isInVolume(box, empty));

      // Test non-overlapping valid boxes
      gmtl::AABoxf box2(gmtl::Point3f(30,30,30), gmtl::Point3f(40,40,40));
      CPPUNIT_ASSERT(! gmtl::isInVolume(box, box2));
      CPPUNIT_ASSERT(! gmtl::isInVolume(box2, box));

      // Test overlapping valid boxes
      gmtl::AABoxf box3(gmtl::Point3f(35,35,35), gmtl::Point3f(37,37,37));
      CPPUNIT_ASSERT(gmtl::isInVolume(box2, box3));
      CPPUNIT_ASSERT(gmtl::isInVolume(box3, box2));

      // Test valid box against itself
      CPPUNIT_ASSERT(gmtl::isInVolume(box, box));
   }

   void AABoxContainMetricTest::testTimingIsInVolumeAABox()
   {
      gmtl::AABoxf box(gmtl::Point3f(-1,-1,-1), gmtl::Point3f(1,1,1));
      gmtl::AABoxf box2(gmtl::Point3f(0,0,0), gmtl::Point3f(-0.5f, -0.5f, -0.5f));

      const long iters(400000);
      float use_value(0);
      CPPUNIT_METRIC_START_TIMING();

      for(long iter=0;iter<iters; ++iter)
      {
         gmtl::isInVolume(box, box2);
         use_value = use_value + box.mMin[0] + 2.0f;
      }

      CPPUNIT_METRIC_STOP_TIMING();
      CPPUNIT_ASSERT_METRIC_TIMING_LE("AABoxContainTest/IsInVolumeAABox", iters, 0.075f, 0.1f);  // warn at 7.5%, error at 10%

      CPPUNIT_ASSERT(use_value > 0.0f);
   }

   void AABoxContainTest::testExtendVolumePt()
   {
      // Test empty box and pt
      gmtl::AABoxf empty, result;
      gmtl::Point3f origin;
      result = empty;
      gmtl::extendVolume(result, origin);
      CPPUNIT_ASSERT(! result.isEmpty());
      CPPUNIT_ASSERT(result.getMin() == origin);
      CPPUNIT_ASSERT(result.getMax() == origin);

      // Valid box against pt inside
      gmtl::AABoxf box2(gmtl::Point3f(-1,-1,-1), gmtl::Point3f(1,1,1));
      result = box2;
      gmtl::extendVolume(result, origin);
      CPPUNIT_ASSERT(! result.isEmpty());
      CPPUNIT_ASSERT(result.getMin() == box2.getMin());
      CPPUNIT_ASSERT(result.getMax() == box2.getMax());

      // Valid box with pt outside
      gmtl::Point3f pt(30, 30, -30);
      gmtl::Point3f expMin(-1,-1,-30);
      gmtl::Point3f expMax(30,30,1);
      result = box2;
      gmtl::extendVolume(result, pt);
      CPPUNIT_ASSERT(! result.isEmpty());
      CPPUNIT_ASSERT(result.getMin() == expMin);
      CPPUNIT_ASSERT(result.getMax() == expMax);
   }

   void AABoxContainMetricTest::testTimingExtendVolumePt()
   {
      gmtl::AABoxf box(gmtl::Point3f(-1,-1,-1), gmtl::Point3f(1,1,1));
      gmtl::Point3f origin;

      const long iters(400000);
      float use_value(0);
      CPPUNIT_METRIC_START_TIMING();

      for(long iter=0;iter<iters; ++iter)
      {
         gmtl::extendVolume(box, origin);
         use_value = use_value + box.mMin[0] + 2.0f;
      }

      CPPUNIT_METRIC_STOP_TIMING();
      CPPUNIT_ASSERT_METRIC_TIMING_LE("AABoxContainTest/ExtendVolumePt", iters, 0.075f, 0.1f);  // warn at 7.5%, error at 10%

      CPPUNIT_ASSERT(use_value > 0.0f);
   }

   void AABoxContainTest::testExtendVolumeAABox()
   {
      gmtl::AABoxf empty, result;
      gmtl::AABoxf box(gmtl::Point3f(-1,-1,-1), gmtl::Point3f(1,1,1));

      // Both boxes empty
      {
         gmtl::extendVolume(result, empty);
         CPPUNIT_ASSERT(result.isEmpty());
      }

      // Empty box with valid box
      {
         result = empty;
         gmtl::extendVolume(result, box);
         CPPUNIT_ASSERT(! result.isEmpty());
         CPPUNIT_ASSERT(result.getMin() == box.getMin());
         CPPUNIT_ASSERT(result.getMax() == box.getMax());
      }

      // Overlapping valid boxes
      {
         gmtl::AABoxf box2(gmtl::Point3f(0,0,0), gmtl::Point3f(2,2,2));
         gmtl::Point3f expMin(-1,-1,-1);
         gmtl::Point3f expMax(2,2,2);
         result = box;
         gmtl::extendVolume(result, box2);
         CPPUNIT_ASSERT(! result.isEmpty());
         CPPUNIT_ASSERT(result.getMin() == expMin);
         CPPUNIT_ASSERT(result.getMax() == expMax);
      }

      // Non-overlapping valid boxes
      {
         gmtl::AABoxf box2(gmtl::Point3f(2,2,2), gmtl::Point3f(4,4,4));
         gmtl::Point3f expMin(-1,-1,-1);
         gmtl::Point3f expMax(4,4,4);
         result = box;
         gmtl::extendVolume(result, box2);
         CPPUNIT_ASSERT(! result.isEmpty());
         CPPUNIT_ASSERT(result.getMin() == expMin);
         CPPUNIT_ASSERT(result.getMax() == expMax);
      }
   }

   void AABoxContainMetricTest::testTimingExtendVolumeAABox()
   {
      gmtl::AABoxf box(gmtl::Point3f(-1,-1,-1), gmtl::Point3f(1,1,1));
      gmtl::AABoxf box2(gmtl::Point3f(0,0,0), gmtl::Point3f(2,2,2));

      const long iters(400000);
      float use_value(0);
      CPPUNIT_METRIC_START_TIMING();

      for(long iter=0;iter<iters; ++iter)
      {
         gmtl::extendVolume(box, box2);
         use_value = use_value + box.mMin[0] + 2.0f;
      }

      CPPUNIT_METRIC_STOP_TIMING();
      CPPUNIT_ASSERT_METRIC_TIMING_LE("AABoxContainTest/ExtendVolumeAABox", iters, 0.075f, 0.1f);  // warn at 7.5%, error at 10%

      CPPUNIT_ASSERT(use_value > 0.0f);
   }

   void AABoxContainTest::testMakeVolumeSphere()
   {
      gmtl::Spheref sph(gmtl::Point3f(1,1,1), 2);
      gmtl::AABoxf box;

      gmtl::Point3f expected_min(-1,-1,-1);
      gmtl::Point3f expected_max( 3, 3, 3);

      gmtl::makeVolume(box, sph);
      CPPUNIT_ASSERT(box.getMin() == expected_min);
      CPPUNIT_ASSERT(box.getMax() == expected_max);
      CPPUNIT_ASSERT(! box.isEmpty());
   }
}
