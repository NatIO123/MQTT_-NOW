idf_component_register(SRCS "Azure_IoT.c"
                    INCLUDE_DIRS "include"
                    REQUIRES "az_core"
                    REQUIRES "az_iot"
                    REQUIRES "az_precondition_internal")
