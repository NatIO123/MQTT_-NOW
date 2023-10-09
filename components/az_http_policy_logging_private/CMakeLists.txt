idf_component_register(SRCS "az_http_policy_logging_private.c"
                    INCLUDE_DIRS "include"
                    REQUIRES "az_http"
                    REQUIRES "az_http_transport"
                    REQUIRES "_az_cfg_prefix")
