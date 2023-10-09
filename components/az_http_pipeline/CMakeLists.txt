idf_component_register(SRCS "az_http_pipeline.c"
                    INCLUDE_DIRS "include"
                    REQUIRES "az_http"
                    REQUIRES "az_http_internal"
                    REQUIRES "az_precondition_internal"
                    REQUIRES "_az_cfg")
