idf_component_register(SRCS "az_http_policy.c"
                    INCLUDE_DIRS "include"
                    REQUIRES "az_http_private"
                    REQUIRES "az_credentials"
                    REQUIRES "az_http"
                    REQUIRES "az_precondition"
                    REQUIRES "az_span"
                    REQUIRES "az_version"
                    REQUIRES "az_http_internal"
                    REQUIRES "az_result_internal"
                    REQUIRES "az_span_internal"
                    REQUIRES "_az_cfg")
