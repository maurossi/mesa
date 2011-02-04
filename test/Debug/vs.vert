uniform sampler2D sampler2d;
uniform mat4 uMatrix;
uniform mat4 uRotM;
uniform vec4 uIndex;

attribute vec4 aPosition; 
attribute vec4 aTexCoord;
attribute mat4 aTransM;

varying vec4 vTexCoord;
varying vec4 vNormal;

void main()
{	
	vTexCoord = aTexCoord;// + texture2D(sampler2d, aTexCoord.xy);
	gl_Position = uMatrix * aPosition;// * vec4(0.5,0.5,0.5,1);
	//gl_Position = aPosition;
	//gl_Position = aTransM * gl_Position;
    //gl_Position = aPosition * vec4(0.2, 0.2, 0.2, 1);
	vNormal = aPosition / vec4(vec3(1.732050807568877),1);
	gl_PointSize = aTexCoord.z;
    //vNormal = vec4(vec3(0.01),1);
	//vNormal = (uRotM * vec4(vNormal, 0)).xyz;
}
