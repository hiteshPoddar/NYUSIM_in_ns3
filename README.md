# NYUSIM_in_ns3
Release v1.0 for NYUSIM in ns-3

# Usage
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


# References

<p class = "justified-text"> The following describes in detail the implementation of NYUSIM in n3:</p>
<ul>
<li><a href="https://arxiv.org/pdf/2305.01828.pdf">ns-3 Implementation of Sub-Terahertz and Millimeter Wave Drop-based NYU Channel Model (NYUSIM)</a> provides a detailed description of the implementation of NYUSIM channel model in ns-3. We encourage the readers to read this paper to get an overview of the implementation of NYUSIM in ns-3.</li>
<li><a href="https://www.proquest.com/openview/3e8cd42260d8e3fe502e0706256608af/1?pq-origsite=gscholar&cbl=18750&diss=y">NYUSIM Wireless Channel Simulator Extension above 100
GHz and Implementation in ns-3</a> provides a detailed description of the implementation of NYUSIM channel model above 100 GHz in MATLAB and NYUSIM channel model for the frequency range of 0.5 GHz - 150 GHz in ns-3.</li>
<li><a href="https://arxiv.org/pdf/2302.12385.pdf">Full-Stack End-To-End mmWave Simulations Using
3GPP and NYUSIM Channel Model in ns-3</a> provides insights on using NYUSIM with <a href="https://apps.nsnam.org/app/mmwave/"> ns-3 mmWave</a> module and a compares the end-to-end throuhgput, latency and packet-drop observed using the  NYUSIM and 3GPP channel model at mmWave frequency.</li>
</ul>

<p class = "justified-text"> To encourage fellow researchers to leverage the NYUSIM module within the ns-3 framework for their own studies, we kindly request that when incorporating this module into your research, you consider acknowledging its use by citing:</p>
<p class = "justified-text"> Poddar, H., Yoshimura, T., Pagin, M., Rappaport, T., Ishii, A., & Zorzi, M. (2023, June). ns-3 Implementation of Sub-Terahertz and Millimeter Wave Drop-based NYU Channel Model (NYUSIM). In Proceedings of the 2023 Workshop on ns-3 (pp. 19-27)
</p>


# Future Work

<p class = "justified-text"> We plan to implement NYUSIM-based blockage models for all 3GPP listed scenarios and create LOS probability models
for InH, RMa, and InF scenarios. In addition, we will also add spatial
consistency-based models for NYUSIM in ns-3 and extend NYUSIM models above 150 GHz for 3GPP-listed scenarios.
</p>
<span class="green-large-text">About</span>
<p class = "justified-text"> This module is being developed by <a href="https://wireless.engineering.nyu.edu/">NYU WIRELESS </a>
and <a href="https://www.sharplabs.com/">Sharp Laboratories of America</a>. This work is supported and funded by the NYU WIRELESS Industrial Affiliates Program, and the commissioned research (No.04201) from the National Institute of Information and Communications Technology (NICT), Japan. </p>

# Authors
<p class = "justified-text"> 
NYUSIM in ns-3 is the result of the development effort carried out by <ul>
    <li>Hitesh Poddar, NYU WIRELESS</li>
    <li>Tomoki Yoshimura, Sharp Laboratories of America</li>
</ul>

# License

<p class = "justified-text"> Copyright (c) 2023 New York University and NYU WIRELESS
<p class = "justified-text"> Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:</p>
<p class = "justified-text">
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software. Users are encouraged to cite NYU WIRELESS publications regarding this work.</p> 
<p class = "justified-text">THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.</p>
