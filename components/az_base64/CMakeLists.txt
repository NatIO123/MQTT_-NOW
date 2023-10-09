idf_component_register(SRCS "az_base64.c"
                    INCLUDE_DIRS "include"
                    REQUIRES "az_result"
                    REQUIRES "az_span"
                    REQUIRES "az_precondition_internal"
                    REQUIRES "_az_cfg_prefix"
                    REQUIRES "_az_cfg_suffix")
