idf_component_register(SRCS "az_iot.c"
                    INCLUDE_DIRS "include"
                    REQUIRES "az_iot_adu_client"
                    REQUIRES "az_iot_common"
                    REQUIRES "az_iot_hub_client"
                    REQUIRES "az_iot_hub_client_properties"
                    REQUIRES "az_iot_provisioning_client")
