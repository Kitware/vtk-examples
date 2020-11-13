### Description

This example introduces 3D widgets. 3D widgets take advantage of the
event/observer design pattern introduced previously. They typically
have a particular representation in the scene which can be interactively
selected and manipulated using the mouse and keyboard. As the widgets
are manipulated, they in turn invoke events such as StartInteractionEvent,
InteractionEvent, and EndInteractionEvent which can be used to manipulate
the scene that the widget is embedded in. 3D widgets work in the context
of the event loop which was set up in the previous example.
