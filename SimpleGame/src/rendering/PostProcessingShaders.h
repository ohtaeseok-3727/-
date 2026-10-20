#pragma once

namespace PostShaders
{
    const char* const Vertex = R"GLSL(#version 330 core
out vec2 uv;
void main()
{
    vec2 corner = vec2((gl_VertexID << 1) & 2, gl_VertexID & 2);
    uv = corner;
    gl_Position = vec4(corner * 2.0 - 1.0, 0.0, 1.0);
}
)GLSL";

    const char* const Fragment = R"GLSL(#version 330 core
in vec2 uv;
layout(location=0) out vec4 outScene;
layout(location=1) out vec4 outBloom;
uniform sampler2D sourceScene;
uniform sampler2D sourceBlur;
uniform sampler2D sourceBloom;
uniform int mode;
uniform vec2 direction;
uniform float threshold;
uniform bool enabled;
uniform float exposure;
uniform float bloomStrength;
uniform float bloomRadius;
uniform float vignetteStrength;
uniform float edgeStrength;
uniform vec4 cutLine;
uniform vec2 cutMotion;
uniform vec4 arcaneShock; // center in pixels, explosion age, displacement strength

vec3 extractBright(vec3 color)
{
    float peak = max(max(color.r, color.g), color.b);
    return color * max(peak - threshold, 0.0) / max(peak, 0.0001);
}
vec3 toDisplay(vec3 color)
{
    color = max(color, vec3(0.0));
    return mix(12.92 * color, 1.055 * pow(color, vec3(1.0 / 2.4)) - 0.055,
        step(vec3(0.0031308), color));
}
void main()
{
    outBloom = vec4(0.0);
    if (mode == 0) {
        // Extract before averaging so subpixel highlights can seed bloom.
        vec2 texel = 1.0 / vec2(textureSize(sourceScene, 0));
        vec3 color = vec3(0.0), bright = vec3(0.0);
        for (int y = 0; y < 2; ++y) for (int x = 0; x < 2; ++x) {
            vec3 sampleColor = texture(sourceScene, uv + (vec2(x,y)-0.5)*texel).rgb;
            color += sampleColor * 0.25;
            bright += extractBright(sampleColor) * 0.25;
        }
        outScene = vec4(color, 1.0);
        outBloom = vec4(bright, 1.0);
    } else if (mode == 1) {
        // Same Gaussian weights, independent spread for bloom and scene edge blur.
        float weights[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);
        vec2 texel = direction / vec2(textureSize(sourceScene, 0));
        vec3 color = texture(sourceScene, uv).rgb * weights[0];
        vec3 bright = texture(sourceBloom, uv).rgb * weights[0];
        for (int i = 1; i < 5; ++i) {
            color += (texture(sourceScene, uv+texel*i).rgb + texture(sourceScene, uv-texel*i).rgb)*weights[i];
            vec2 bloomOffset = texel * float(i) * bloomRadius;
            bright += (texture(sourceBloom, uv+bloomOffset).rgb + texture(sourceBloom, uv-bloomOffset).rgb)*weights[i];
        }
        outScene = vec4(color, 1.0);
        outBloom = vec4(bright, 1.0);
    } else {
        vec2 sceneUV = uv;
        if (cutMotion.x > 0.001) {
            vec2 size = vec2(textureSize(sourceScene, 0));
            vec2 pixel = vec2(uv.x, 1.0 - uv.y) * size;
            vec2 axis = cutLine.zw - cutLine.xy;
            float len = max(length(axis), 1.0);
            vec2 tangent = axis / len;
            vec2 normal = vec2(-tangent.y, tangent.x);
            vec2 delta = pixel - cutLine.xy;
            float along = dot(delta, tangent) / len;
            float edge = 3.0 * (0.65*sin(along*97.0) + 0.35*sin(along*173.0+1.3));
            float side = dot(delta, normal) - edge;
            float spread = 1.0 - smoothstep(cutMotion.y, cutMotion.y + 0.09, along);
            vec2 shift = (normal + tangent*0.22) * sign(side) * cutMotion.x * spread;
            sceneUV -= vec2(shift.x, -shift.y) / size;
            sceneUV = clamp(sceneUV, 0.5/size, 1.0-0.5/size);
        }
        if(arcaneShock.w>0.001) {
            vec2 size=vec2(textureSize(sourceScene,0));
            vec2 pixel=vec2(uv.x,1.0-uv.y)*size;
            vec2 delta=(pixel-arcaneShock.xy)/vec2(1.0,.58);
            float radius=length(delta);
            float front=300.0*(.06+(1.0-exp(-arcaneShock.z*7.0))*1.32);
            float envelope=exp(-pow((radius-front)/16.0,2.0));
            vec2 displacement=delta/max(radius,1.0)*vec2(1.0,.58)*envelope*arcaneShock.w;
            sceneUV-=vec2(displacement.x,-displacement.y)/size;
            sceneUV=clamp(sceneUV,.5/size,1.0-.5/size);
        }
        vec3 color = texture(sourceScene, sceneUV).rgb;
        if (enabled) {
            vec2 centered = abs(uv*2.0-1.0);
            float edge = smoothstep(0.62, 0.98, max(centered.x, centered.y));
            color = mix(color, texture(sourceBlur, sceneUV).rgb, edge*edgeStrength);
            color += texture(sourceBloom, sceneUV).rgb * bloomStrength;
            // Exposure followed by exponential tone mapping; floating highlights compress smoothly.
            color = vec3(1.0) - exp(-max(color, vec3(0.0))*exposure);
            float vignette = smoothstep(0.30, 1.0, length(centered)*0.70710678);
            color *= 1.0 - vignette*vignetteStrength;
        }
        // HDR working buffer -> ordinary SDR window; not HDR10 monitor output.
        outScene = vec4(toDisplay(clamp(color, 0.0, 1.0)), 1.0);
    }
}
)GLSL";
}
