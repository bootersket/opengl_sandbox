# OpenGL Learning Goals

### Mental Model of the Graphics Pipeline

* Explain the states from CPU-side vertex data->vertex shader->fragment shader->framebuffer

* Understand what transforms are handled where (e.g., model/view/projection in vertex shader, lighting in fragment shader, etc.)

* Know the distinction between client-side data and GPU-side resources (VBOs, VAOs, textures, etc.)

✅ Should not need to look this up once understood.


### Buffer and Object Management
* Confidently use VAOs, VBOs, EBOs/IBOs.

* Understand buffer binding, buffer data uploads, draw calls (glDrawElements, glDrawArrays, etc.)

* Know what glEnableVertexAttribArray, glVertexAttribPointer, etc. do and why.

🔍 Might occasionally check exact function signatures, but should know the purpose and flow.


### Shader Workflow

* Write GLSL shaders from scratch: vertex and fragment shaders at minimum.

* Understand how to pass uniforms, attributes, varyings.

* Use shader compilation/linking properly and debug errors.

✅ Writing simple shaders and debugging should be doable without hand-holding.
🔍 Complex lighting models or advanced GLSL syntax may require reference.

### Coordinate Systems and Transformations

* Know the difference between model, world, view, clip, NDC, screen space.

* Understand perspective vs. orthographic projection.

* Apply glm or equivalent for transform matrices.

✅ Should be second nature to convert object coords to screen space.

### Texture Handling

* Load and bind textures correctly.

* Understand texture units, filtering, wrapping, mipmaps.

* Use textures in shaders (sampler2D, texture()).

🔍 You may occasionally check GL enum values or advanced formats.


6. State Machine Awareness

* Understand the OpenGL state machine: enabling depth testing, blending, culling, etc.

* Know how state leaks can cause hard-to-track bugs.

✅ Should have good mental hygiene about setting/resetting state.
