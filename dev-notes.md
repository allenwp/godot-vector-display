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

`SamplerSystem` and `GameLoop`

This sampler system should be named `VDRenderer`. Additionaly there should be a new Godot `Node` class called `VectorDisplay`. The `VectorDisplay` node will give a warning when it is not the last node in the main scene tree because it will call on the VDRenderer in its `_process` func. It will produce a number of debug output options that render to the desktop game viewport as children of its node, as well as host configuration options for ASIO ouptut and vector display configuration, etc.

This should likely be written in C++ with theading similar to the original `SamplerSystem`.

`Post Processing`

Post processing should be implemented the same way as it was in the original Vector Engine, with a few additions:
- Post processing on individual `VDShape3D` and `VDShape2D` nodes (in object space and world space)
- Post processing in 3D or 2D world space before camera transformations
- Post processing in 2D screen space on each camera's sample stream
- Post processing in 2D screen space on final sample stream

Because the Godot engine is generally designed with inheritance, post processors will be added as an array of post processor resources to a `VDShape3D` or `VDShape2D`. These post processors can run in object space or world space.

Post processors can also be added to a new `VDCamera3D` and `VDCamera2D` class, which inherit from `Camera3D` and `Camera2D`. They will be added as an array of post processor resources, similar to the `VDShape` classes, and can be run in world space or screen space.

Post processing on the final sample stream can be done in screen space via an array of post processors attatched to the `VDRenderer` object.

`Camera`

This entire camera class, including the concept of `Filter` and `Priority` can be discarded in favour if Godot camera classes. The unique properites of this class were only used to do things that the Godot camera systems are capable of.

`FrameOutput`

(no comments)

## Other Notes
Maybe untyped arrays or `Vector4`/`Vector3` could be implemented with a static helper class that extracts properties? This would give some `struct`-like functionality.
