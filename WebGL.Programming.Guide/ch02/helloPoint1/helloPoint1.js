//helloPoint1.js
//vertex shader source
var VSHADER_SOURCE =
'void main() {\n' +
'   gl_Position = vec4(0.0, 0.0, 0.0, 1.0);\n' + //coordinate
'   gl_PointSize= 10.0;\n' + //set point size
'}\n';

var FSHADER_SOURCE =
'void main() {\n' +
'   gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);\n' + //set the color
'}\n';

function main() {
    //retrieve canvas element
    var canvas = document.getElementById('webgl');
    //get the render context for webgl
    var gl = getWebGLContext(canvas);
    if (!gl) {
        console.log('Failed to get the rendering context for WebGl');
        return ;
    }

    //Initialize shader
    if (!initShader(gl, VSHADER_SOURCE, FSHADER_SOURCE)) {
        console.log('Failed to initialize shaders');
        return;
    }

    //set the color for clearing canvas
    gl.clearColor(0.0, 0.0, 0.0, 1.0);
    //clear
    gl.clear(gl.COLOR_BUFFER_BIT);
    gl.drawArrays(gl.POINTS, 0, 1);
}