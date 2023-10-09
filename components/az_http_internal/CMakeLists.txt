idf_component_register(SRCS "az_http_internal.c"
                    INCLUDE_DIRS "include"
                    REQUIRES "az_context"
                    REQUIRES "az_http"
                    REQUIRES "az_http_transport"
                    REQUIRES "az_result"
                    REQUIRES "az_precondition_internal"
                    REQUIRES "_az_cfg_prefix")
