## Classes & Structs

`Shape`

This class should be renamed to `VDShape3D` and should inherit from Godot's `VisualInstance3D`. It should have the one abstract function: `func GetVDSamples3D(float fidelity) -> Array[Array[Vector4]]`

Additionally, a new `VDShape2D` should be added. This object type doesn't exist in the original Vector Engine.

`Sample3D`

This struct should be implemented in Godot as a struct... but structs are not yet implemented. Instead, it should be a `Vector4` and the `w` component will hold the brightness. When structs are implemented in Godot, this `Vector4` could be reimplemented as a `VDSample3D` with the brightness can be a separate bool property.

Additionally, a `Vector3` can be used for a 2D sample with brightness in Godot 2D space. This object type doesn't exist in the original Vector Engine.

`Sample`

This struct should be implemented in Godot as a struct... but structs are not yet implemented. It should be a `Vector3` and have the same properties. It is based in Vector display screen space where (0, 0) is the center of the screen.

Helper functions should be implemented in a static class.

`SamplerSystem`

This sampler system should be recreated as a server, similar to the rendering server(?)

`Post Processing`

Post processing should be implemented the same way as it was in the original Vector Engine:
- Post processing on individual `VDShape3D` and `VDShape2D` nodes (in object space)
- Post processing in 3D or 2D world space before camera transformations
- Post processing in 2D screen space on each camera's sample stream
- Post processing in 2D screen space on final sample stream (Is this really necessary? Is the previous one good enough on its own?)

`Camera`

This entire camera class, including the concept of `Filter` and `Priority` can be discarded in favour if Godot camera classes. The unique properites of this class were only used to do things that the Godot camera systems are capable of.

`FrameOutput`

(no comments)

## Other Notes
Maybe untyped arrays or `Vector4`/`Vector3` could be implemented with a static helper class that extracts properties? This would give some `struct`-like functionality.
