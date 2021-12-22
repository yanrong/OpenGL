//vertex shader source
var VERTEX_SOURCE =
'attribute vec4 aPosition;\n' + //attribute variable
'void main() {\n' +
'   gl_Position = aPosition;\n' +
'   gl_PointSize = 10.0;\n' +
'}';

//fragment shader source
var FRAGMENT_SOURCE =
'void main() {\n' +
'   gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);\n' +
'}\n';

function main() {
    var canvas = document.getElementById("webgl");

    //get the render contex
    var gl = getWebGLContext(canvas);
    if (gl == null) {
        console.log("Failed to get the rendering content");
        return;
    }

    //initialize shaders
    if (!initShader(gl, VERTEX_SOURCE, FRAGMENT_SOURCE)) {
        console.log("Failed to initialize shaders.");
        return;
    }

    var aPosition = gl.getAttribLocation(gl.program, "aPosition");
    if (aPosition < 0) {
        console.log("Failed to get the shader variable");
        return ;
    }

    //set the value, pass the value from program to vertex shader
    gl.vertexAttrib3f(aPosition, 0.0, 0.0, 0.0);
    //clear and set background color
    gl.clearColor(0.0, 0.0, 0.0, 1.0);
    //clear the buffer object
    gl.clear(gl.COLOR_BUFFER_BIT);
    //draw a point
    gl.drawArrays(gl.POINTS, 0, 1);
}