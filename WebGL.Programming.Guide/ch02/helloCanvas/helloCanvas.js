function main() {
    //retrieve canvas element
    var canvas = document.getElementById('webgl');

    //get the rendering context for WebGL
    var gl = getWebGLContext(canvas);
    if (!gl) {
        console.log('Failed to get the rendering context');
        return ;
    }

    //clear color
    gl.clearColor(0.0, 0.0, 1.0, 1.0);
    //clear canvas
    gl.clear(gl.COLOR_BUFFER_BIT);
}