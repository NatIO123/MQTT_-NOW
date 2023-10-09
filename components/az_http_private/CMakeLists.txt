idf_component_register(SRCS "az_http_private.c"
                    INCLUDE_DIRS "include"
                    REQUIRES "az_http"
                    REQUIRES "az_http_transport"
                    REQUIRES "az_precondition"
                    REQUIRES "az_span"
                    REQUIRES "az_precondition_internal"
                    REQUIRES "_az_cfg_prefix")
