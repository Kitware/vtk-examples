### Description

Makes solid marble texture with strong veins. The "veincolor" parameter controls the color of the veins. The background color is given by the surface's DiffuseColor.

Explore the parameter space with the vtkSliderWidget.

#### Parameters

* _veinfreq_ - controls the lowest frequency of the color veins e.g. 10
* _veinlevels_ - how many "levels" of vein tendrils it has e.g. 2
* _warpfreq_ - lowest frequency of the turbulent warping in the marble e.g. 10
* _ warping_ - controls how much turbulent warping there will be e.g. 0.5
* _veincolor_ - the color of the veins e.g. white: 1 1 1
* _sharpness_ - controls how sharp or fuzzy the veins are (higher = sharper) e.g. 8

!!! cite
    This shader is inspired by Larry Gritz's veined marble shader.
