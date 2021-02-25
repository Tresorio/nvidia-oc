# nvidia-oc

This is a CLI tool to set the core and memory clock offsets of your Nvidia GPUs on Linux.
It uses the same API as nvidia-settings but doesn't need any graphical libraries dependencies.

## Install

You can download a pre-built package of nvidia-oc in the [Releases](https://github.com/elassyo/nvidia-oc/releases) section or you can [build](#Build) the tool yourself.

### Requirements

To use this tool, you will need to install the following dependencies:

1. libX11 (`libx11-6` on Ubuntu)
2. libXNVCtrl (`libxnvctrl0` on Ubuntu)

## Usage

> ### :warning: Disclaimer
>
> Overclocking can permanently damage your hardware.
>
> **USE THIS TOOL AT YOUR OWN RISK.**

To use this tool, you first need to enable overclocking in the Nvidia driver configuration by setting the `Coolbits` option in your X server configuration file. You can do this easily with the nvidia-xconfig tool:

```shell
nvidia-xconfig --enable--all-gpus --cool-bits=31
```

While this is a CLI tool, the Nvidia control API it uses to set overclocking values need to go through an X server. You will need have to have one running as well as having the `DISPLAY` (and maybe `XAUTHORITY`) set properly for nvidia-oc to work.

You can use this tool to set the GPU core and memory clock offsets of all the GPUs on a system by running:

```shell
nvidia-oc CORE_CLOCK MEM_CLOCK
```

where `CORE_CLOCK` and `MEM_CLOCK` are your desired clock offsets in MHz. 0 means no offset = default clock.

These overclock values are not saved and will be reset when you reboot your machine. If you need the settings to persist, you can use an init script or systemd service to run nvidia-oc after boot.

## Build

### Requirements

To build this project, you need to install the following dependencies:

1. CMake
2. g++ or clang++
3. libX11 and libXNVCtrl headers (`libx11-dev` and `libxnvctrl-dev` on Ubuntu)

### Instructions

1. Download / extract this tool's source code
2. Create a build directory:

    ```shell
    mkdir build
    cd build
    ```

3. Configure the project with CMake:

    ```shell
    cmake ..
    ```

4. Build the project with CMake:

    ```shell
    cmake --build .
    ```
