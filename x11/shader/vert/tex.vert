#version 450 core

vec2 vbl = vec2 ( -1, -1 );
vec2 vul = vec2 ( -1,  1 );
vec2 vur = vec2 (  1,  1 );
vec2 vbr = vec2 (  1, -1 );

vec2 pos[6] = vec2[](
	vbl,
	vul,
	vur,

	vbl,
	vur,
	vbr
);

vec2 ubl = vec2 ( 0, 0 );
vec2 uul = vec2 ( 0, 1 );
vec2 uur = vec2 ( 1, 1 );
vec2 ubr = vec2 ( 1, 0 );

vec2 uv[6] = vec2[](
    ubl,
    uul,
    uur,

    ubl,
    uur,
    ubr

);

layout(location = 0) out vec2 fragTexCoord;

void main() {
    gl_Position = vec4(pos[gl_VertexIndex], 0.0, 1.0);
    fragTexCoord = vec2(uv[gl_VertexIndex]);
}
