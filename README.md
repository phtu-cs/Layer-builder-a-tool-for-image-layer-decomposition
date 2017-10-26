# Layer-builder-a-tool-for-image-layer-decomposition
Reproduce LayerBuilder: Layer Decomposition for Interactive Image and Video Color Editing (https://arxiv.org/abs/1701.03754)

Note that color extraction block is not identical to that within Layerbuilder paper.
I use a revised version of K-means to extract the color palette, presented in Palette-based Photo Recoloring (http://www.cs.princeton.edu/~huiwenc/pub/sig2015-palette.pdf)

## Compile

This program depends on opencv 3.0 and Eigen 3.3.4.

## Usage

Please use command line to run this program.

For example

...\Release> layerbuilder tree.ppm 4

Color extraction. Done.

Superpixel segementation. Done.

Layer decomposition. Done.

Which layer do you want to recolor? (from layer 0 to 3)

Color of layer 0:0.137023 0.207921 0.263292 (B G R, [0,1]) layer_0_numsp_4_tree.ppm

Color of layer 1:0.640594 0.341261 0.414382 (B G R, [0,1]) layer_1_numsp_4_tree.ppm

Color of layer 2:0.333945 0.561367  0.71033 (B G R, [0,1]) layer_2_numsp_4_tree.ppm

Color of layer 3:0.472462 0.833339 0.950592 (B G R, [0,1]) layer_3_numsp_4_tree.ppm

Please see output images for visualization

Please input the number of the layer: 1

You want to recolor layer1

Please input the color (BGR) of the new layer (e.g. 0.5 0.5 0.5): 0.5 0.5 0.5



In this example, the input image is tree.ppm. The number of layers is 4.

recol_tree.ppm is the recolored image. 

rec_tree.ppm is the reconstructed image using the decomposed layers, 
which is used to see the effectiveness of image decomposition.


I have not considered the robustness of the primitive interaction program. Please don't input random numbers or 
characters which could make the program crash.

## Result
Please see ![here] (https://github.com/tph9608/Layer-builder-a-tool-for-image-layer-decomposition/results/my_results.pdf)  
