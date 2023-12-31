#include <stdio.h>
#include "az_http_pipeline.h"

// Copyright (c) Microsoft Corporation. All rights reserved.
// SPDX-License-Identifier: MIT

#include <az_http.h>
#include <az_http_internal.h>
#include <az_precondition_internal.h>

#include <_az_cfg.h>

AZ_NODISCARD az_result az_http_pipeline_process(
    _az_http_pipeline* ref_pipeline,
    az_http_request* ref_request,
    az_http_response* ref_response)
{
  _az_PRECONDITION_NOT_NULL(ref_request);
  _az_PRECONDITION_NOT_NULL(ref_response);
  _az_PRECONDITION_NOT_NULL(ref_pipeline);

  return ref_pipeline->_internal.policies[0]._internal.process(
      &(ref_pipeline->_internal.policies[1]),
      ref_pipeline->_internal.policies[0]._internal.options,
      ref_request,
      ref_response);
}
