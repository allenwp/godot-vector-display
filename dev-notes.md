`Shape`

This class should be renamed to `VDShape3D` and should inherit from Godot's `VisualInstance3D`. It should have the one abstract function: `func GetVDSamples3D(float fidelity) -> Array[Sample3D]`

Additionally, a new `VDShape2D` should be added. This object type doesn't exist in the original Vector Engine.

`Sample3D`

This struct should be implemented in Godot as a struct... but structs are not yet implemented. It should be named `VDSample3D` and have the same three properties (Position, Brightness, and Disabled).

Additionally, a new `VDSample2D` should be added. This object type doesn't exist in the original Vector Engine.

`Sample`

This struct should be implemented in Godot as a struct... but structs are not yet implemented. It should be named `VDSample` and have the same properties. It is based in Vector display screen space where (0, 0) is the center of the screen.

Helper functions should be implemented in a static class.

## Structs
Maybe structs could be implemented as untyped arrays with a static helper class that extracts properties from array objects?
