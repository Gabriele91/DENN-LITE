# DENN-LITE Doc

*Under development...*

## Build the framework

To build the framework you don't need particular requirements. Under linux OS you have to install `zlib-dev` to a successful compilation. From the command line, if you have installed a build environment like `gcc` or `clang` you can simply use the following commands from the main folder:

```bash
# For the release binaries
make release

# For the debug binaries
make debug
```

You can check the options available with the executable with *--help* command:

```bash
./Release/DENN-float --help
```

## Simple example

To use the framework you need a dataset compatible with it. You can download some prebuild datasets with this command:

```bash
git clone https://github.com/MircoT/DENN-dataset-samples.git
```

You can put them at the same folder level of *DENN-LITE*.

An example of how to use the framework is the following:

```bash
./Release/DENN-float -t 2000 -s 5 -b 10 -bo 10 -np 123 -f 0.5 -cr 0.9 -cmax 150 -cmin -150 -rmax 150 -rmin -150 -rc -1 -rd 0.001 -tp 16 -i ../DENN-dataset-samples/DATASETS/d_06_01_2018/qsar.gz -em JDE -jf 0.1 -jcr 0.1 -m rand/1 -co bin -ro bench -o example.json
```

> **NOTE**: check the path of the dataset, it depends on which folder you clone the dataset samples.

## Further readings

* [how to make a test batch job](./tests.md)