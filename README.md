# NYUSIM_in_ns3
Release v1.0 for NYUSIM in ns-3

Steps to use NYUSIM in ns-3 on ns-3 mainline: (Successfully Tested on ns-3 version 3.39)

1. Copy the all the files in repository present in the directory propagation/model to ns-3 mainline src/propagation/model
2. Copy the all the files in repository present in the directory propagation/example to ns-3 mainline src/propagation/examples
3. On ns-3 mainline in the directory src/propagation add the following lines to the CMakeLists.txt file under
   SOURCE_FILES
    model/nyu-channel-condition-model.cc
    model/nyu-propagation-loss-model.cc
   HEADER_FILES
    model/nyu-channel-condition-model.h
    model/nyu-propagation-loss-model.h
4. Copy the all the files in repository present in the directory spectrum/model to ns-3 mainline src/spectrum/model
5. Copy the all the files in repository present in the directory spectrum/example to ns-3 mainline src/spectrum/examples
6. On ns-3 mainline in the directory src/spectrum add the following lines to the CMakeLists.txt file under
   SOURCE_FILES
    model/nyu-channel-model.cc
    model/nyu-spectrum-propagation-loss-model.cc
   HEADER_FILES
    model/nyu-channel-model.cc
    model/nyu-spectrum-propagation-loss-model.cc
7. You can run the example files from Step 2 or Step 5 to see the usage of NYUSIM channel model.
