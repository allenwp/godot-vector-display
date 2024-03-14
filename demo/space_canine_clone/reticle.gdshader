shader_type spatial;
render_mode unshaded, blend_add, depth_test_disabled, cull_disabled;

uniform sampler2D tex;
uniform vec3 colour : source_color;

void fragment() {
// Texture2D:2
	vec4 n_out2p0 = texture(tex, UV);
// Output:0
	ALBEDO = vec3(n_out2p0.xyz * colour);
}
