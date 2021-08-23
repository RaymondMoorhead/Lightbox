// SHADER(Name, VertexShader, FragmentShader, GeometryShader)
// Creates a new shader program of the given name with the given shader code files
// Name is plainly typed, Shader file names must be quotes
// empty quotes will not generate that type of shader for a given program

// SHADER_UNIFORM1F(Name, Value)
// SHADER_UNIFORM2F(Name, Value1, Value2)
// SHADER_UNIFORM3F(Name, Value1, Value2, Value3)
// SHADER_UNIFORM4F(Name, Value1, Value2, Value3, Value4)
// sets the initial uniform values on GraphicsController::Initialize for
// the last SHADER created in this file
// Name is plainly typed as in the shader, values are typed as in C

SHADER(Basic, "basic.vert", "basic.frag", "basic.geom")
SHADER_UNIFORM4F(ambient, 0.2f, 0.2f, 0.2f, 1.0f)

SHADER(DepthCube, "shadow_map_cube.vert", "shadow_map_cube.frag", "shadow_map_cube.geom")

SHADER(DepthFlat, "depth_only.vert", "empty.frag", "")

SHADER(Framebuffer, "framebuffer.vert", "framebuffer.frag", "")