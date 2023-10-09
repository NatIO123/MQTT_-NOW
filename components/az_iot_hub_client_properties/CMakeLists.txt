idf_component_register(SRCS "az_iot_hub_client_properties.c"
                    INCLUDE_DIRS "include"
                    REQUIRES "az_json"
                    REQUIRES "az_result"
                    REQUIRES "az_span"
                    REQUIRES "az_iot_hub_client"
                    REQUIRES "az_precondition_internal"
                
                    REQUIRES "_az_cfg_prefix")
