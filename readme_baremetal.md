# Bare metal installation

please refer to "getting-started-with-pico.pdf" in the ./docs folder

##### Install a pico arm compiler 

your can get it here:
https://developer.arm.com/downloads/-/gnu-rm

##### install the pico sdk and it's submodules

```sh
git clone https://github.com/raspberrypi/pico-sdk.git pico-sdk
cd pico-sdk
git submodule update --init
```

##### install usis library (in the "test" folder)

```sh
mkdir test
cd test
git clone https://gitlab.shelyak.online/root/sw0017-usis-library.git usis
```

##### copy pico example

```sh
cp examples/bare_metal/test2.cpp
```

### compilation

##### export the `pico sdk` path

```sh
export PICO_SDK_PATH=<path to pico-sdk>
```

##### export the `gcc` path

```sh
export PATH=<path to pico gcc>:$PATH
```

##### compile

```sh
cd test/build-rp2040
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug .
make
```

### upload & debug 

in order to debug with VSCode please check the .vscode folder

##### tasks.json

replace `PATH TO YOUR GCC` with your gcc path

```json
{
	"version": "2.0.0",
	"tasks": [
		{
			"type": "shell",
			"label": "build RP2040",
			"options": {
				"env": {
					"PATH": "<PATH TO YOUR GCC>/bin:${env:PATH}",
				},
				"cwd": "./build-rp2040"
			},
			"command": "make.exe ",
			"group": "build"
		}
	]
}
```

##### launch.json

you must use jlink 

```json
{
	"version": "0.2.0",
	"configurations": [
		{
			"name": "Debug baremetal RP2040",
			"cwd": "${workspaceRoot}",
			"executable": "${workspaceRoot}/build/test2.elf",
			"gdbPath": "arm-none-eabi-gdb.exe",
			"request": "launch",
			"type": "cortex-debug",
			"servertype": "jlink",
			"device": "rp2040_M0_0"
		}
	]
}

```

##### c_cpp_properties.json

replace `PATH TO YOUR GCC` with your gcc path

```json
{
    "configurations": [
        {
            "name": "gcc",
            "includePath": [
                "${workspaceFolder}/**",
                "<PATH TO YOUR GCC>/lib/gcc/arm-none-eabi/10.3.1/include",
                "${workspaceFolder}/src"
            ],
            "defines": [
                "_DEBUG",
                "UNICODE",
                "_UNICODE",
				"RP2040_BAREMETAL",
				"RP2040"
            ],
            "windowsSdkVersion": "10.0.18362.0",
            "compilerPath": "<PATH TO YOUR GCC>/bin/arm-none-eabi-g++.exe",
            "cStandard": "c99",
            "cppStandard": "c++17"
        }
    ],
    "version": 4
}

```








