## Description

Demonstrates how to render an object in a scene with a glowing outline.

The class vtkOutlineGlowPass is designed to highlight parts of a scene by applying the render pass to a layered renderer on top of the main scene. For optimal results, actors that form the outline should be brightly colored with lighting disabled. The outline will have the color of the actors. There is only one outline around all objects rendered by the delegate.

When combined with layered renderers, this creates a very visible highlight without altering the highlighted object.
