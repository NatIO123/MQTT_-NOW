idf_component_register(SRCS "az_json_token.c"
                    INCLUDE_DIRS "include"
                    REQUIRES "az_precondition_internal"
                    REQUIRES "az_result_internal"
                    REQUIRES "az_span_internal"
                    REQUIRES "az_json_private"
                    REQUIRES "az_span_private"
                    REQUIRES "_az_cfg")
