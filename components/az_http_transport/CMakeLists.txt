idf_component_register(SRCS "az_http_transport.c"
                    INCLUDE_DIRS "include"
                    REQUIRES "az_http"
                    REQUIRES "az_span"
                    REQUIRES "_az_cfg_prefix")
