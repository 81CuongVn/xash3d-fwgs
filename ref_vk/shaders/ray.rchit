#version 460 core
#extension GL_EXT_ray_tracing: require
#extension GL_EXT_shader_16bit_storage : require

struct Kusok {
	uint index_offset;
	uint vertex_offset;
	uint triangles;

	// Material
	uint texture;
	float roughness;
};

struct Vertex {
	vec3 pos;
	vec3 normal;
	vec2 gl_tc;
	vec2 lm_tc;
};

layout(std430, binding = 3, set = 0) readonly buffer Kusochki { Kusok kusochki[]; };
layout(std430, binding = 4, set = 0) readonly buffer Indices { uint16_t indices[]; };
layout(std430, binding = 5, set = 0) readonly buffer Vertices { Vertex vertices[]; };

layout (constant_id = 6) const uint MAX_TEXTURES = 4096;
layout (set = 0, binding = 6) uniform sampler2D textures[MAX_TEXTURES];

layout(location = 0) rayPayloadInEXT vec4 ray_result;

hitAttributeEXT vec2 bary;

// float hash(float f) { return fract(sin(f)*53478.4327); }
// vec3 hashUintToVec3(uint i) { return vec3(hash(float(i)), hash(float(i)+15.43), hash(float(i)+34.)); }

void main() {
    const float l = gl_HitTEXT;
    ray_result = vec4(fract(l / 8.));
    //return;

    //ray_result = vec4(.5);

    //vec3 hit_pos = gl_WorldRayOriginEXT + gl_WorldRayDirectionEXT * gl_HitTEXT;
	//ray_result = vec4(fract(hit_pos), 1.);
    //ray_result = vec4(bary, 0., 1.);
    //return;

    //ray_result.rgb = hashUintToVec3(gl_GeometryIndexEXT);

    // const int instance_kusochki_offset = gl_InstanceCustomIndexEXT;
    // const int kusok_index = instance_kusochki_offset + gl_GeometryIndexEXT;

    // const uint first_index_offset = kusochki[kusok_index].index_offset + gl_PrimitiveID * 3;
    // const uint vi1 = uint(indices[first_index_offset+0]) + kusochki[kusok_index].vertex_offset;
    // const uint vi2 = uint(indices[first_index_offset+1]) + kusochki[kusok_index].vertex_offset;
    // const uint vi3 = uint(indices[first_index_offset+2]) + kusochki[kusok_index].vertex_offset;
    // const vec3 n1 = vertices[vi1].normal;
    // const vec3 n2 = vertices[vi2].normal;
    // const vec3 n3 = vertices[vi3].normal;

    // // TODO use already inverse gl_WorldToObject ?
    // const vec3 normal = normalize(transpose(inverse(mat3(gl_ObjectToWorldEXT))) * (n1 * (1. - bary.x - bary.y) + n2 * bary.x + n3 * bary.y));
    // //hit_pos += normal * normal_offset_fudge;

    // //C = normal * .5 + .5; break;

    // const vec2 texture_uv = vertices[vi1].gl_tc * (1. - bary.x - bary.y) + vertices[vi2].gl_tc * bary.x + vertices[vi3].gl_tc * bary.y;
    // const uint tex_index = kusochki[kusok_index].texture;
    // //C = vec3(hash(float(tex_index)), hash(float(tex_index)+15.43), hash(float(tex_index)+34.)); break; // * vec3(texture_uv, 1.); break;
    // const vec3 base_color = pow(texture(textures[tex_index], texture_uv).rgb, vec3(2.));

	// //ray_result = vec4(base_color, 1.);
	// //ray_result = vec4(mix(base_color, abs(fract(hit_pos)), .5), 1.);
	// ray_result = vec4(abs(fract(hit_pos * .01)), 1.);
}