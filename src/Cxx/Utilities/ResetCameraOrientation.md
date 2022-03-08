### Description

This example demonstrates how to reset the camera orientation.

In this case, the original orientation is stored, a new orientation calculated, and, finally the original orientation is restored. The original, new and restored orientations of the cone are displayed in one second intervals. After this you can interact with the scene.

To set the camera position, you can use:

- `SetPosition`, `SetFocalPoint`, `SetViewUp`, `SetDistance`, `SetClippingRange`
- `Roll`, `Elevation`, `Azimuth`
- `SetRoll`, `Pitch`, `Yaw`

If you haven't used  `SetDistance` or `SetClippingRange` remember to `ren->ResetCamera();` or `ren.ResetCamera()`.
