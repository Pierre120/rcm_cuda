# Robinson's Compass Mask CUDA

CUDA implementation of Robinson's Compass Mask.

## Adding image input

Place the images under the **`/inputs`** directory.

## Output images

Output images can be seen under the **`/outputs`** directory.

## Compiling C

```
# Windows
gcc -std=c11 -Wall main.c -o rcm.exe

# Linux/MacOS
gcc -std=c11 -Wall main.c -o rcm.out
```

## Running program

```
# Windows
rcm

# Linux/MacOS
./rcm.out
```

## Entering filename for image input

The program will prompt for filename input as seen below:

```
Enter the image filename: |
```

Just enter the filename of the image itself without prefixing the `./inputs/` directory.

Sample directory:

```
images
    | - sample.jpg
```

In the program:

```
Enter the image filename: sample.jpg
```
