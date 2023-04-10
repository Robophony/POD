 #version 440 core
 in vec3 N;
 in vec3 L;
 in vec3 E;
 in vec3 H;

out vec4 color2;
void main()
{

   vec4 black  = N.r * vec4(0,0,0,1); // we need to do this with one of the existing variables or this code may be optimized out
    color2 = vec4(0,1,0,1)+ black;// make green
}
