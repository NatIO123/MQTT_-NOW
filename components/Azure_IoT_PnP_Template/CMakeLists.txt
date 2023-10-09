idf_component_register(SRCS "Azure_IoT_PnP_Template.c"
                    INCLUDE_DIRS "include"
                    REQUIRES "Azure_IoT"
                    REQUIRES "az_core"
                    REQUIRES "az_iot"
                    REQUIRES "az_precondition_internal"
                    REQUIRES "az_span")
