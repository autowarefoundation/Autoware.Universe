# autoware_dynamic_lanelet_provider

## Purpose

This package aims to provide a dynamic Lanelet2 map to the other Autoware nodes.
The dynamic Lanelet2 map is a Lanelet2 map that is updated in real-time based
on the current odometry position of the vehicle.

To use this package, you need to provide a divided Lanelet2 maps and a metadata file.
You should check
the [lanelet2_map_loader documentation](https://autowarefoundation.github.io/autoware.universe/main/map/map_loader/#lanelet2_map_loader)
for more information about the divided lanelet map and the metadata file.

## Inputs / Outputs

### Input

| Name                         | Type                                       | Description                     |
| ---------------------------- | ------------------------------------------ | ------------------------------- |
| `~/input/odometry`           | `nav_msgs::msg::Odometry`                  | ego vehicle odometry            |
| `/map/lanelet_map_meta_data` | autoware_map_msgs::msg::LaneletMapMetaData | metadata info for lanelet2 maps |

### Output

| Name                  | Type                                    | Description                |
| --------------------- | --------------------------------------- | -------------------------- |
| `output/lanelet2_map` | `autoware_map_msgs::msg::LaneletMapBin` | dynamic Lanelet2 map topic |

### Client

| Name                                   | Type                                             | Description                                |
| -------------------------------------- | ------------------------------------------------ | ------------------------------------------ |
| `service/get_differential_lanelet_map` | `autoware_map_msgs::srv::GetSelectedLanelet2Map` | service to load differential Lanelet2 maps |

## Parameters

{{ json_to_markdown("map/autoware_dynamic_lanelet_provider/schema/dynamic_lanelet_provider.schema.json") }}
