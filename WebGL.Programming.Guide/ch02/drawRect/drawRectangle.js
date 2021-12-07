//Draw Rectangle
function main() {
    //retrive the canvas element
    var canvas = document.getElementById('example');
    if (!canvas) {
        console.log('Failed to retrieve the canvas');
        return false;
    }
    //get the rendering context for 2D
    var contex = canvas.getContext('2d');

    //draw a blue rectangle
    contex.fillStyle = 'rgba(0, 0, 255, 1.0)'; //set color and alpha
    contex.fillRect(10, 10, 300, 150); // fill the Rectangle with postion + width/height
}