uniform sampler2D samp2DArray[9];
uniform samplerCube sampCube;
uniform sampler2D samp2D;
uniform sampler2D samp2DA;
uniform vec4 uFragmentColor;
uniform mat4 uRotM;

varying vec4 vTexCoord; 
varying vec4 vNormal;

void main(void)
{
    //gl_FragColor.xy = gl_FragCoord.xy / vec2(480, 800);
    //gl_FragColor.z = !gl_FrontFacing ? 1 : 0;
    
    //gl_FragColor += vec4(0,0,0,0);
    //gl_FragColor = vTexCoord.xyzw;
    //gl_FragColor = texture2D(samp2DArray[int(vTexCoord.z)], vTexCoord.xy);
    gl_FragColor = texture2D(samp2D, vTexCoord.xy);
    //gl_FragColor = textureCube(sampCube, vTexCoord.xyz);
    //gl_FragColor *= vNormal;
    //gl_FragColor = textureCube(samplercube, vNormal.xyz);
    //gl_FragColor.rgb = vec3(sin(gl_FragColor.r));
    //gl_FragColor.a = 1;
    //gl_FragColor *= (vTexCoord.wwww - vTexCoord * vTexCoord);

    //*
    gl_FragColor -= vec4(0.5);
    gl_FragColor.a = 0.0;
    gl_FragColor = uRotM * gl_FragColor;
    gl_FragColor += vec4(0.5);
    //*/
    
    //*
    float dot = dot(vNormal.xyz, vec3(-1.414213562373095, 0, -1.414213562373095));
    gl_FragColor.rgb *= vec3(dot);
	//*/
	
	//gl_FragColor.rgb += vNormal.xyz;
    //gl_FragColor = (gl_FragCoord / vec4(480, 800, 1, 1));
    //gl_FragColor.r = gl_FrontFacing == true ? 1.0 : 0.0;
    //gl_FragColor.gb = gl_PointCoord;
    //gl_FragColor = vTexCoord;

    gl_FragColor.a = 1.0;
}
