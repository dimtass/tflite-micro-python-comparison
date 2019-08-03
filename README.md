Benchmarking TensorFlow Lite for microcontrollers on amd64
----

In this repo I do simple benchmarking of the tflite-micro build on amd64
with the python API. This is a naive benchmark. Both the c++ and python
code used the same tflite model. The python script loads the `mnist.tflite`
model and then runs the inference on the same digit 1000 times. The c++ is
also using the same tflite model, which is first converted using the tflite
to this `source/src/inc/model_data.h` c header.

The reason I've used pre-defined data is because I wanted also to visually
verify that the model was making the correct prediction. The digit is not
from the dataset, but it's a hand-drawn digit. I've used
[this](https://bitbucket.org/dimtass/stm32f746-tflite-micro-mnist/src/master/jupyter_notebook/MNIST-TensorFlow.ipynb?viewer=nbviewer)
jupyter notebook to create the digit and export it to use it for the
benchmarks. This notebook belongs to [this repo](https://bitbucket.org/dimtass/stm32f746-tflite-micro-mnist).

For the python script the digit is located in `python/digit.txt`.
For the c++ code in `source/src/inc/digit.h`.

## Build the code
To build the code:
```sh
./build.sh
```

In case you need to clean and re-build:
```sh
CLEANBUILD=true ./build.sh
```

## My testing hardware
My workstation doesn't have a GPU that provides any acceleration for tflite.

```sh
sudo lshw -C display
  *-display                 
       description: VGA compatible controller
       product: GK208B [GeForce GT 710]
       vendor: NVIDIA Corporation
       physical id: 0
       bus info: pci@0000:26:00.0
       version: a1
       width: 64 bits
       clock: 33MHz
       capabilities: pm msi pciexpress vga_controller bus_master cap_list rom
       configuration: driver=nvidia latency=0
       resources: irq:85 memory:f6000000-f6ffffff memory:e8000000-efffffff memory:f0000000-f1ffffff ioport:e000(size=128) memory:c0000-dffff
```

Only the CPU is used in the benchmarks
```sh
sudo lshw -C cpu
  *-cpu                     
       description: CPU
       product: AMD Ryzen 7 2700X Eight-Core Processor
       vendor: Advanced Micro Devices [AMD]
       physical id: 15
       bus info: cpu@0
       version: AMD Ryzen 7 2700X Eight-Core Processor
       serial: Unknown
       slot: AM4
       size: 2195MHz
       capacity: 3700MHz
       width: 64 bits
       clock: 100MHz
       capabilities: x86-64 fpu fpu_exception wp vme de pse tsc msr pae mce cx8 apic sep mtrr pge mca cmov pat pse36 clflush mmx fxsr sse sse2 ht syscall nx mmxext fxsr_opt pdpe1gb rdtscp constant_tsc rep_good nopl nonstop_tsc cpuid extd_apicid aperfmperf pni pclmulqdq monitor ssse3 fma cx16 sse4_1 sse4_2 movbe popcnt aes xsave avx f16c rdrand lahf_lm cmp_legacy svm extapic cr8_legacy abm sse4a misalignsse 3dnowprefetch osvw skinit wdt tce topoext perfctr_core perfctr_nb bpext perfctr_llc mwaitx cpb hw_pstate sme ssbd sev ibpb vmmcall fsgsbase bmi1 avx2 smep bmi2 rdseed adx smap clflushopt sha_ni xsaveopt xsavec xgetbv1 xsaves clzero irperf xsaveerptr arat npt lbrv svm_lock nrip_save tsc_scale vmcb_clean flushbyasid decodeassists pausefilter pfthreshold avic v_vmsave_vmload vgif overflow_recov succor smca cpufreq
       configuration: cores=8 enabledcores=8 threads=16
```

## Run the c++ benchmark
The executable is created in `build-amd64/src/`; therefore, from the top directory
run this command:

```sh
./build-amd64/src/amd64-mnist-tflite-micro
```

## Run the python benchmark
I'm using conda and I'm having a virtual environment where I've installed
tensorflow. To run the test:

```sh
cd python
python3 python_benchmark.py
```

> Note: if you're testing on a workstation with GPU acceleration, then it's probably
best to force using the CPU in the benchmarks, as tflite-micro is able only to use
the cpu.

## Results
These are the results on my system for 1000 runs.
```
n_runs = 1000
```

tflite-micro | python
-|-
2.223457 ms | 0.199918 ms

These are the console outputs:

#### cpp
```sh
./build-amd64/src/amd64-mnist-tflite-micro
Program started...
Running inference...
Out[0]: 0.000000
Out[1]: 0.000000
Out[2]: 1.000000
Out[3]: 0.000000
Out[4]: 0.000000
Out[5]: 0.000000
Out[6]: 0.000000
Out[7]: 0.000000
Out[8]: 0.000000
Out[9]: 0.000000

----------------------
Total elapsed time: 2.144370 ms
```

#### python
```sh
python3 python_benchmark.py 
TensorFlow version 1.14.0
INFO: Initialized TensorFlow Lite runtime.
Prediction results: [[1.5875762e-10 2.0665674e-08 1.0000000e+00 9.0883438e-12 3.8147090e-13
  3.2608371e-15 1.3031773e-14 2.2391200e-09 7.7385898e-10 1.3262178e-12]]
Predicted value: 2
----------------------
Total elapsed time: 0.199918 ms
```

**Note**: in case that you need to restrict the execution in a specific
cpu then use this command:
```sh
taskset --cpu-list 1 python3 python_benchmark.py
```

This might needed in order to prevent parallel execution on the python
benchmark, as the tflite-micro code, definitely doesn't support multi-threading.
It's not clear to me if this also prevent spawned threads to run on other cpu
cores, too, though.

## Results
On my system, using only CPU, it seems that the python implementation
is 11.2x times faster.

## Author
Dimitris Tassopoulos <dimtass@gmail.com>