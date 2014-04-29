/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 * Version 0.6b
 *
 * Copyright (c) 2013-2014 Inviwo Foundation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Main file authors: Timo Ropinski, Peter Steneteg
 *
 *********************************************************************************/

#include "include/inc_sampler2d.frag"

uniform TEXTURE_TYPE entryColorTex_;
uniform TEXTURE_TYPE entryDepthTex_;
uniform TEXTURE_PARAMETERS entryParameters_;
uniform TEXTURE_TYPE exitColorTex_;
uniform TEXTURE_TYPE exitDepthTex_;
uniform TEXTURE_PARAMETERS exitParameters_;

uniform mat4 NDCToTextureMat_; // Normalized device coordinates to volume texture coordinates
uniform float nearDist_;

in vec3 texCoord_;

void main() {
    float entryDepth = texture(entryDepthTex_, texCoord_.xy).z;
    float exitDepth = texture(exitDepthTex_, texCoord_.xy).z;
    vec4 entryColor;

    if (entryDepth > exitDepth) {
        // entry points are clipped by near plane
        // Convert texture coordinates to normalized device coordinates. (ndc) The z value will always be -1 on the clipping plane
        vec4 cameraCoordinates = vec4(2.0f*texCoord_.xy-1.0f, -1.0f, 1.0f);
        // convert the ndc back to the volume texture coordinates
        entryColor = NDCToTextureMat_ * cameraCoordinates * nearDist_;
        entryDepth = 0.0f;
    } else {
        entryColor = texture(entryColorTex_, texCoord_.xy);
    }

    FragData0 = entryColor;
    gl_FragDepth = entryDepth;
}