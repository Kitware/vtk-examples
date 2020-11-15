### Description

This example shows how to add user functions to a VTK shader. The example's fragment shader uses an implementation of [Ken Perlin's noise function](https://www.semanticscholar.org/paper/An-image-synthesizer-Perlin/e04d7772b91a83a901408eb0876bbb7814b1d4b5). Perlin received a Technical Achievement Academy Award for his work procedural texture.

The file for the shader code is `src/Testing/Data/Shaders/PerlinNoise.glsl`.

#### Parameters

* _veinfreq_ - controls the lowest frequency of the color veins e.g. 10
* _veinlevels_ - how many "levels" of vein tendrils it has e.g. 2
* _warpfreq_ - lowest frequency of the turbulent warping in the marble e.g. 10
* _ warping_ - controls how much turbulent warping there will be e.g. 0.5
* _veincolor_ - the color of the veins e.g. white: 1 1 1
* _sharpness_ - controls how sharp or fuzzy the veins are (higher = sharper) e.g. 8

!!! cite
    Perlin's original Siggraph Paper: [Perlin, K. 1985. "An Image Synthesizer." Computer Graphics 19(3)](https://dl.acm.org/doi/10.1145/325165.325247).

!!! cite
    This shader is inspired by Larry Gritz's veined marble shader.
