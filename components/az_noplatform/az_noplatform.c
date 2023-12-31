#include <stdio.h>
#include "az_noplatform.h"

// Copyright (c) Microsoft Corporation. All rights reserved.
// SPDX-License-Identifier: MIT

#include <az_platform.h>
#include <az_precondition_internal.h>

#include <_az_cfg.h>

AZ_NODISCARD az_result az_platform_clock_msec(int64_t* out_clock_msec)
{
  _az_PRECONDITION_NOT_NULL(out_clock_msec);
  *out_clock_msec = 0;
  return AZ_ERROR_DEPENDENCY_NOT_PROVIDED;
}

AZ_NODISCARD az_result az_platform_sleep_msec(int32_t milliseconds)
{
  (void)milliseconds;
  return AZ_ERROR_DEPENDENCY_NOT_PROVIDED;
}

