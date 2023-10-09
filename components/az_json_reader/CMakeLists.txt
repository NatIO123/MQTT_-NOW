idf_component_register(SRCS "az_json_reader.c"
                    INCLUDE_DIRS "include"
                    REQUIRES "az_json_private"
                    REQUIRES "az_span_private"
                    REQUIRES "az_precondition"
                    REQUIRES "az_result_internal"
                    REQUIRES "_az_cfg")
