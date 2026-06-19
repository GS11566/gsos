
# GSOS

A simple OS written from scratch, to help myself understand how modern computers work.


> [!NOTE]
> Only supports Legacy BIOS

## To build

Clone the project

```bash
  git clone https://github.com/GS11566/gsos.git
```

Go to the project directory

```bash
  cd gsos
```

Compile!

```bash
  make floppy_image
```

The main_floppy.img will be in the build/ directory!


## To run in QEMU

Clone the project (don't do this if you've cloned it already)

```bash
  git clone https://github.com/GS11566/gsos.git
```

Go to the project directory

```bash
  cd gsos
```

Compile and run QEMU

```bash
  make run
```

## Authors

- [@GS11566](https://www.github.com/GS11566)

