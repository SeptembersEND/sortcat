<div align="center">
<h1 align="center"> Sort[a] Cat </h1>
</div>

<p align="center">
<img src="https://septembersend.github.io/img/sortcat/cat.jpg" />
</p>

> [!WARNING]
> Currently does not free any memory used!
> Proceed at your own risk!

## Compiling

Use the C compiler of your choice, and compile `main.c`.
For example:

```bash
cc main.c
```


## How to use

After you compile the program, name a file `in.ppm`, and create the directory
`out`, where the exported images will be saved.

To create a ppm image, you can use
[ImageMagick](https://imagemagick.org/script/download.php) to convert an image
to the `.ppm` file format used for this program.


## How make mp4

```bash
ffmpeg -r <fps> -pattern_type glob -i 'test*.ppm' -c:v libx265 <outfile>.mp4
```

you can slow it down by using:

```bash
-vf "setpts=0.5*PTS"
```


## How to make a gif?

Use the video you made and run the following:

```bash
ffmpeg -i <inputfile>.mp4 <outputfile>.gif
```

Can shrink it via this:

```bash
ffmpeg -i <inputfile>.mp4 -filter_complex "[0:v]palettegen=max_colors=64[pal];[0:v][pal]paletteuse=dither=bayer" <output>.gif
```

Uses dithering.

Have gif loop once:

```bash
-loop -1
```


## Examples

### `quicksort()`

```C
#include "src/lib.h"
int main(void) {
    Image img = import("in.ppm");
    quicksort(&img);
    ende = 1;
    snapshot(&img);
}
```

Can move `ende = 1` to above `quicksort()` to get a series of images of it
going through the sorting process.

<p align="center">
    <img src="https://septembersend.github.io/img/sortcat/cyber.jpg" />
    <img src="https://septembersend.github.io/img/sortcat/cybersorted.jpg" />
</p>


### `sobelOperator()`

```C
#include "src/lib.h"
int main(void) {
    Image img = import("in.ppm");
    Image buf = sobelOperator(&img);
    ende = 1;
    snapshot(&buf);
}
```

Can move `ende = 1` to above `sobelOperator()` to get a series of images of it
going through line by line.

<p align="center">
    <img src="https://septembersend.github.io/img/sortcat/cybersobel.jpg" />
</p>


## 3D Rendering (`example/3d_image.c`)

```C
#include "../src/3d.h"
#include "../src/lib.h"
int main(void) {
  Image img = init(512, 512, 256);
  model obj = import_objF(model_file);
  put_3dmodel(obj, img, NULL, NULL);
  ende = 1;
  snapshot(img);
}
```

Can move `ende = 1` to above `put_3dmodel()` to get a series of images of it
going through line by line.

<p align="center">
    <img src="https://septembersend.github.io/img/sortcat/cat.jpg" />
    <img src="https://septembersend.github.io/img/sortcat/catsorted.jpg" />
</p>


## Why Sort Cat?

Because the first thing I did with this project, is sort the pixels in an image
of a cat.

<p align="center">
    <img src="https://septembersend.github.io/img/sortcat/cat.jpg" />
    <img src="https://septembersend.github.io/img/sortcat/catsorted.jpg" />
</p>
