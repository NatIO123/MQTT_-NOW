idf_component_register(SRCS "az_iot_hub_client.c"
                    INCLUDE_DIRS "include"
                    REQUIRES "az_result"
                    REQUIRES "az_span"
                    REQUIRES "az_version"
                    REQUIRES "az_precondition_internal"
                    REQUIRES "az_result_internal"
                    REQUIRES "az_span_internal"
                    REQUIRES "az_iot_hub_client"
                    REQUIRES "az_iot_common_internal"
                    REQUIRES "az_iot_common"
                    REQUIRES "_az_cfg"
                    REQUIRES "_az_cfg_prefix")
