/**
 * Crate a program object and make current
 * @param gl OpenGL contex
 * @param vshader a vertex shader program source code
 * @param fshader a fragment shader program source code
 * @return return true if the program is create
 */
function initShader(gl, vshader, fshader) {
    var program = createProgram(gl, vshader, fshader);
    if (!program) {
        console.log('Failed to create program');
        return false;
    }

    gl.useProgram(program);
    gl.program = program;
    return true;
}

/**
 * create the shader program and compile link it
 * @param gl OpenGL contex
 * @param vshader a vertex shader source code
 * @param fshader a fragment shader source coded
 * @return return a shader program if successful, otherwise return null
 */
function createProgram(gl, vshader, fshader) {
    var vertexShader = loadShader(gl, gl.VERTEX_SHADER, vshader);
    var fragmentShader = loadShader(gl, gl.FRAGMENT_SHADER, fshader);
    if (!vertexShader || !fragmentShader) {
        console.log('load shader source failed');
        return null;
    }

    //create a program object
    var program = gl.createProgram();
    if (!program) {
        console.log('create the shader program failed');
        return null;
    }

    //attach the shaders to program
    gl.attachShader(program, vertexShader);
    gl.attachShader(program, fragmentShader);
    //link the program object
    gl.linkProgram(program);

    //check the link status
    var linked = gl.getProgramParameter(program, gl.LINK_STATUS);
    if (!linked) {
        var error = gl.getProgramInfoLog(program);
        console.log('Failed to link program: ' + error);
        gl.deleteProgram(program);
        gl.deleteShader(vertexShader);
        gl.deleteShader(fragmentShader);
        return null;
    }
    return program;
}

/**
 * Create a shader object
 * @param gl OpenGL context
 * @param type the type of the shader object to be created
 * @param source shader program source code(string)
 * @return return the created shader object
 */
function loadShader(gl, type, source) {
    var shader = gl.createShader(type);
    if (shader == null) {
        console.log('Unable to create shader');
        return null;
    }

    //set the shader program
    gl.shaderSource(shader, source);
    //compile the shader
    gl.compileShader(shader);
    var compiled = gl.getShaderParameter(shader, gl.COMPILE_STATUS);
    if (!compiled) {
        var error = gl.getShaderInfoLog(shader);
        console.log('Failed to compile shader: ' + error);
        gl.deleteShader(shader);
        return null;
    }
    return shader;
}

/**
 * Initialize and get the WebGL rendering context
 * @param canvas <canvas> element in HTML
 * @param isDebug flag to initialze the context for debug
 * @return return the WebGL context
 */
function getWebGLContext(canvas, isDebug) {
    var gl = WebGLUtils.setupWebGL(canvas);
    if (!gl) {
        return null;
    }

    //if force open the DEBUG flag, create a debug contex for GL
    if (arguments.length < 2 || isDebug) {
        gl = WebGLDebugUtils.makeDebugContext(gl);
    }
    return gl;
}