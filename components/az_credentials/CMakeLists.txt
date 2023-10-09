idf_component_register(SRCS "az_credentials.c"
                    INCLUDE_DIRS "include"
                    REQUIRES "az_http_transport"
                    REQUIRES "az_result"
                    REQUIRES "az_span"
                    REQUIRES "_az_cfg_prefix")
