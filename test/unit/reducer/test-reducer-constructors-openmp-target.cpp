//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
// Copyright (c) 2016-20, Lawrence Livermore National Security, LLC
// and RAJA project contributors. See the RAJA/COPYRIGHT file for details.
//
// SPDX-License-Identifier: (BSD-3-Clause)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

///
/// Source file containing tests for RAJA reducer constructors and initialization.
///

#include "tests/test-reducer-constructors.hpp"

#include "test-reducer-utils.hpp"

#if defined(RAJA_ENABLE_TARGET_OPENMP)
using OpenMPTargetInitReducerConstructorTypes = Test< camp::cartesian_product<
                                                        OpenMPTargetReducerPolicyList,
                                                        DataTypeList,
                                                        OpenMPTargetResourceList
                                                     >
                            >::Types;

INSTANTIATE_TYPED_TEST_CASE_P(OpenMPTargetInitTest,
                              ReducerInitConstructorUnitTest,
                              OpenMPTargetInitReducerConstructorTypes);
#endif

