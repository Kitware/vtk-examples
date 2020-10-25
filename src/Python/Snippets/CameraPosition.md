### Description

A callback that gives you the camera position and focal point.

To use the snippet, click the *Copy to clipboard* at the upper left of the code blocks.

### Implementation

``` Python

def CameraModifiedCallback(caller, event):
    """
     Used to estimate positions similar to the book illustrations.
    :param caller:
    :param event:
    :return:
    """
    print(caller.GetClassName(), "modified")
    # Print the interesting stuff.
    res = f'\tcamera = renderer->GetActiveCamera()\n'
    res += f'\tcamera.SetPosition({", ".join(map("{0:5.6f}".format, caller.GetPosition()))})\n'
    res += f'\tcamera.SetFocalPoint({", ".join(map("{0:5.6f}".format, caller.GetFocalPoint()))})\n'
    res += f'\tcamera.SetViewUp({", ".join(map("{0:5.6f}".format, caller.GetViewUp()))})\n'
    res += f'\tcamera.SetDistance({"{0:5.6f}".format(caller.GetDistance())})\n'
    res += f'\tcamera.SetClippingRange({", ".join(map("{0:5.6f}".format, caller.GetClippingRange()))})\n'
    print(res)

```

### Usage

```python
    renWin.Render()
    ren.GetActiveCamera().AddObserver('ModifiedEvent', CameraModifiedCallback)
```

Once you have the output, replace the `ren.GetActiveCamera().AddObserver...` line with the output data.
