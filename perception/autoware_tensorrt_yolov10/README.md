# autoware_tensorrt_yolov10

## Purpose

This package detects target objects e.g., cars, trucks, bicycles, pedestrians,etc on a image based on [YOLOV10](https://github.com/THU-MIG/yolov10) model.

## Inputs / Outputs

### Input

| Name       | Type                | Description     |
| ---------- | ------------------- | --------------- |
| `in/image` | `sensor_msgs/Image` | The input image |

### Output

| Name          | Type                                               | Description                                        |
| ------------- | -------------------------------------------------- | -------------------------------------------------- |
| `out/objects` | `tier4_perception_msgs/DetectedObjectsWithFeature` | The detected objects with 2D bounding boxes        |
| `out/image`   | `sensor_msgs/Image`                                | The image with 2D bounding boxes for visualization |

## Assumptions / Known limits

The label contained in detected 2D bounding boxes (i.e., `out/objects`) will be either one of the followings:

- CAR
- PEDESTRIAN ("PERSON" will also be categorized as "PEDESTRIAN")
- BUS
- TRUCK
- BICYCLE
- MOTORCYCLE

If other labels (case insensitive) are contained in the file specified via the `label_file` parameter,
those are labeled as `UNKNOWN`, while detected rectangles are drawn in the visualization result (`out/image`).

## Onnx model

you can download yolov10m.onnx from [releases](https://github.com/THU-MIG/yolov10/releases)

## Label file

This file represents the correspondence between class index (integer outputted from YOLOV10 network) and
class label (strings making understanding easier). This package maps class IDs (incremented from 0)
with labels according to the order in this file.

currently, this file is actually a coco label which contains the following labels:

```text
person
bicycle
car
motorcycle
airplane
bus
train
truck
boat
traffic light
fire hydrant
stop sign
parking meter
bench
bird
cat
dog
horse
sheep
cow
elephant
bear
zebra
giraffe
backpack
umbrella
handbag
tie
suitcase
frisbee
skis
snowboard
sports ball
kite
baseball bat
baseball glove
skateboard
surfboard
tennis racket
bottle
wine glass
cup
fork
knife
spoon
bowl
banana
apple
sandwich
orange
broccoli
carrot
hot dog
pizza
donut
cake
chair
couch
potted plant
bed
dining table
toilet
tv
laptop
mouse
remote
keyboard
cell phone
microwave
oven
toaster
sink
refrigerator
book
clock
vase
scissors
teddy bear
hair drier
```

## Reference repositories

- <https://github.com/THU-MIG/yolov10>

## Legal Notice

The inference code is licensed under Apache 2.0. The model and training code are licensed under AGPL-3.0. you can check details from <https://github.com/THU-MIG/yolov10?tab=AGPL-3.0-1-ov-file>. To inquire about a commercial license when using trained model weights please contact yolov10 author.
