/*!
 * Image that the user want to work on
 * \class ProcessingImage
 */
function ProcessingImage(src) {
    this.img = new Image();
    this.img.src = src;
}

/*!
 * Draw the image on the context
 * \memberof ProcessingImage
 * \param ctx the context to draw on
 */
ProcessingImage.prototype.draw = function(ctx) {
    var locx = this.x;
    var locy = this.y;
    ctx.drawImage(this.img,locx,locy);
}

/*!
 * Check if a point is in the image
 * \memberof ProcessingImage
 * \param mx x coordonate
 * \param my y coordonate
 * \return bool if the point is in the image or not
 */
ProcessingImage.prototype.contains = function(mx, my) {
  return  (this.x <= mx) && (this.x + (this.w) >= mx) &&
          (this.y <= my) && (this.y + (this.h) >= my);
}

/*!
 * Single baseline
 * \class Line
 */
function Line(lineId, startx, y, finishx){
    this.id = lineId;
    this.startx = startx;
    this.y = y;
    this.finishx = finishx;
    this.selected = false;
}

/*!
 * Draw the line on the context
 * \memberof Line
 * \param ctx the context to draw on
 * \param image the image to ajust the coordonate
 */
Line.prototype.draw = function(ctx, image){
    ctx.beginPath();
    ctx.moveTo(image.x+ this.startx, image.y + this.y);
    ctx.lineTo(image.x + this.finishx, image.y + this.y);
    if(this.selected)
        ctx.strokeStyle = COLOR_BASELINE_SELECTED;
    else
        ctx.strokeStyle = COLOR_BASELINE;

    ctx.lineWidth = LINE_WIDTH_BASELINE;
    ctx.stroke();
}

/*!
 * Container of all baselines
 * \class Baseline
 */
function Baseline(lines) {
    this.lines = lines;
    this.visible = false;
    this.clickMargin = 0.4; //in percent of the image height
}

/*!
 * Draw all baseline on the context
 * \memberof Baseline
 * \param ctx the context to draw on
 * \param image the image to ajust the coordonate
 */
Baseline.prototype.draw = function(ctx, image){
    if(this.visible)
        for (index = 0; index < this.lines.length; index++)
            this.lines[index].draw(ctx, image);
}

/*!
 * Check if a point is on a baseline
 * \memberof Baseline
 * \param mx x coordonate
 * \param my y coordonate
 * \param image the image to ajust the coordonate
 * \return the id of the baseline where the point is on, false otherwise
 */
Baseline.prototype.contains = function(mx, my, image){
    if(this.visible == false)
        return "false";
    for (index = 0; index < this.lines.length; index++) {
        if((this.lines[index].startx + image.x <= mx)
            && (this.lines[index].finishx + image.x >= mx)
            && (this.lines[index].y + image.y - (this.clickMargin * image.h / 100) <= my)
            && (this.lines[index].y + image.y + (this.clickMargin * image.h / 100) >= my))
            return index;
    }
    return 'false';
}

/*!
 * Select a baseline
 * \memberof Baseline
 * \param id of the line to select
 */
Baseline.prototype.select = function (id){
    // Reset all selected lines
    for (index = 0; index < this.lines.length; index++) {
        this.lines[index].selected = false;
    }
    // Select the line
    if(id != 'false')
        this.lines[id].selected = true;
}

/*!
 * Rectangle of a BoundingBox
 * \class Rectangle
 */
function Rectangle(rect, idCC, idLine){
    this.rect = rect;
    this.idCC = idCC;
    this.idLine = idLine;
    this.visible = true;
    this.selected = false;
    this.labeled = false;
    this.hover = false;
}

/*!
 * Draw the rectangle
 * \memberof Rectangle
 * \param ctx context to draw on
 * \param image the image to ajust the coordonate
 */
Rectangle.prototype.draw = function(ctx, image){
    if(this.visible)
    {
        // Color according to the state
        ctx.fillStyle = COLOR_BOUNDINGBOX;
        if(this.labeled != false)
            ctx.fillStyle = COLOR_BOUNDINGBOX_LABELED;
        if(this.selected)
            ctx.fillStyle = COLOR_BOUNDINGBOX_SELECTED;
        if(this.hover)
            ctx.fillStyle = COLOR_BOUNDINGBOX_HOVER;

        ctx.globalAlpha = ALPHA_BOUNDINGBOX;
        ctx.fillRect(image.x + this.rect.x,image.y + this.rect.y, this.rect.w, this.rect.h);
    }
}

/*!
 * Container of all BoundingBox
 * \class BoundingBox
 */
function BoundingBox(rects) {
    this.rects = rects;
}

/*!
 * Draw all bounding box
 * \memberof BoundingBox
 * \param ctx context to draw on
 * \param image the image to ajust the coordonate
 */
BoundingBox.prototype.draw = function(ctx, image){
    for (index = 0; index < this.rects.length; index++) {
        this.rects[index].draw(ctx, image);
    }
}

/*!
 * Check if a point is on a bounding box
 * \memberof BoundingBox
 * \param mx x coordonate
 * \param my y coordonate
 * \param image the image to ajust the coordonate
 * \return the id of the boundingbox where the point is on, false otherwise
 */
BoundingBox.prototype.contains = function(mx, my, image){
    for (index = 0; index < this.rects.length; index++) {
        if((this.rects[index].rect.x + image.x <= mx)
            && (this.rects[index].rect.x + image.x + this.rects[index].rect.w >=mx)
            && (this.rects[index].rect.y + image.y <= my)
            && (this.rects[index].rect.y + image.y + this.rects[index].rect.h >= my))
            return index;
    }
    return 'false';
}

/*!
 * Select a boundingbox
 * \memberof BoundingBox
 * \param id of the boundingbox to select
 */
BoundingBox.prototype.select = function (id){
    // Reset all selected bounding box
    for (index = 0; index < this.rects.length; index++) {
        this.rects[index].selected = false;
    }
    // Select the boundingbox
    if(id != 'false')
        this.rects[id].selected = true;
}

/*!
 * add a boundingbox to the selection
 * \memberof BoundingBox
 * \param id of the boundingbox to add
 */
BoundingBox.prototype.addToSelection = function (id){
    if(id != 'false')
        this.rects[id].selected = true;
}

/*!
 * remove a boundingbox to the selection
 * \memberof BoundingBox
 * \param id of the boundingbox to remove
 */
BoundingBox.prototype.removeToSelection = function (id){
    if(id != 'false')
        this.rects[id].selected = false;
}

/*!
 * principal canvas
 * \class Canvas
 * \param canvas canvas element
 * \param image image draw on the canvas
 * \param baseline list of baselines
 * \param boundingBox list of boundingBox
 */
function Canvas(canvas, image, baseline, boundingBox, typeOfCanvas) {
    var myCanvas = this;

    this.canvas = canvas;
    this.width = canvas.width;
    this.height = canvas.height;
    this.ctx = canvas.getContext('2d');

    this.image = image;

    // Permet de savoir si c'est une dégradation ou un font-creator
    this.typeOfCanvas = typeOfCanvas;

    if (this.typeOfCanvas == "font-creator"){
	this.baseline = baseline;
	this.boundingBox = boundingBox;
    }

    this.typeOfCanvas = typeOfCanvas;

    this.dragging = false;

    this.scale = 1.0;

    this.panX = 0;
    this.panY = 0;

    this.selectedCC = [];

    this.dragoffx = 0;
    this.dragoffy = 0;

    if(image){
    this.image.img.onload = function(){
        myCanvas.image.w = this.width;
        myCanvas.image.h = this.height;


        myCanvas.image.initialx = myCanvas.width/2 - this.width/2;
        myCanvas.image.initialy = myCanvas.height/2 - this.height/2;

        myCanvas.image.x = myCanvas.image.initialx;
        myCanvas.image.y = myCanvas.image.initialy;


        if(this.width > this.height)
            myCanvas.scale = myCanvas.width /this.width;
        else
            myCanvas.scale = myCanvas.height /this.height;
        myCanvas.draw();
      }
    }
}


Canvas.prototype.formatCanvas = function(image) {
  var myCanvas = this;
  //this.image=image;


  this.image.img.onload = function(){
      myCanvas.image.w = this.width;
      myCanvas.image.h = this.height;

      myCanvas.image.initialx = myCanvas.width/2 - this.width/2;
      myCanvas.image.initialy = myCanvas.height/2 - this.height/2;

      myCanvas.image.x = myCanvas.image.initialx;
      myCanvas.image.y = myCanvas.image.initialy;


      if(this.width > this.height)
          myCanvas.scale = myCanvas.width /this.width;
      else
          myCanvas.scale = myCanvas.height /this.height;
      //myCanvas.draw();
    }
}




/*!
 * When the mouse down (Dectection for moving the image)
 * \memberof Canvas
 * \param e event
 */
Canvas.prototype.onMouseDown= function(e){
    var mouse = this.getMouse(e);
    var mx = mouse.x;
    var my = mouse.y;

    var image = this.image;

    if (image.contains(mx, my)) {
        this.dragoffx = mx - image.x;
        this.dragoffy = my - image.y;
        this.dragging = true;
        this.draw();
    }
}

/*!
 * When the mouse move (Dectection for moving the image)
 * \memberof Canvas
 * \param e event
 */
Canvas.prototype.onMouseMove = function(e){
    if (this.dragging){
        var mouse = this.getMouse(e);
        this.image.x = (mouse.x) - this.dragoffx;
        this.image.y = (mouse.y) - this.dragoffy;
        this.draw();
    }
}

/*!
 * When the mouse up (Dectection for moving the image)
 * \memberof Canvas
 * \param e event
 */
Canvas.prototype.onMouseUp = function(e){
    this.dragging = false;
}

/*!
 * Zoom in
 * \memberof Canvas
 */
Canvas.prototype.zoomIn = function(){
    this.scale += 0.1;
    this.draw();
}

/*!
 * Zoom out
 * \memberof Canvas
 */
Canvas.prototype.zoomOut = function(){
    this.scale -= 0.1;
    this.draw();
}

/*!
 * Reset the zoom value
 * \memberof Canvas
 */
Canvas.prototype.zoomReset = function(){
    if(this.width > this.height)
        this.scale = this.width /this.image.w;
    else
        this.scale = this.height /this.image.h;

    this.image.x = this.image.initialx;
    this.image.y = this.image.initialy;
    this.draw();
}

/*!
 * Clear the canvas context to redraw on it
 * \memberof Canvas
 */
Canvas.prototype.clear = function() {
  this.ctx.clearRect(0, 0, this.width, this.height);
}

/*!
 * Draw all element on the canvas
 * \memberof Canvas
 */
Canvas.prototype.draw = function() {
    if (!this.valid) {
	var ctx = this.ctx;

	// get the translation to apply to center the image
	var newWidth = this.width * this.scale;
	var newHeight = this.height * this.scale;
	this.panX = -((newWidth-this.width)/2);
	this.panY = -((newHeight-this.height)/2);

	this.clear();

	// Save the current context
	ctx.save();

	// Apply modifications (zoom, translation) all elements drawing after this will have the modifications
	ctx.translate(this.panX, this.panY);
	ctx.scale(this.scale, this.scale);

	// Draw all elements
	this.image.draw(ctx);

	if (this.typeOfCanvas == "font-creator"){
	    this.baseline.draw(ctx, this.image);
	    this.boundingBox.draw(ctx, this.image);
	}

	// Restore the context
	ctx.restore();

    }
}

/*!
 * Get the mouse coordonate
 * \memberof Canvas
 * \param e event
 * \return coordonate of the mouse
 */
Canvas.prototype.getMouse = function(e) {
    var rect = this.canvas.getBoundingClientRect();
    // Get mouse coordonate relative to the canvas
    var mx = Math.floor((e.clientX-rect.left)/(rect.right-rect.left)*this.canvas.width);
    var my = Math.floor((e.clientY-rect.top)/(rect.bottom-rect.top)*this.canvas.height);

    // Get real mouse coordonate, eliminate scale and translations
    var mouseXT = parseInt((mx - this.panX) / this.scale);
    var mouseYT = parseInt((my - this.panY) / this.scale);

    return {x: mouseXT, y: mouseYT};
}


Canvas.prototype.changeImage = function(imagePath) {

    var canvas = this;

    /*if (this.typeOfCanvas == 'create-your-document') {
      var image = ProcessingImage(imagePath);
      canvas.formatCanvas(image)
    }*/
    this.image.img.src = imagePath;
          this.image.img.onload = function(){
        canvas.draw();
    }
}


/*!
 * preview canvas
 * \class PreviewCanvas
 * \param canvas canvas element
 * \param image image draw on the canvas
 */
function PreviewCanvas(canvas, image)
{
    this.canvas = canvas;
    this.width = canvas.width;
    this.height = canvas.height;
    this.ctx = canvas.getContext('2d');

    this.image = image;

    this.visible = false;
    this.scaleX = 1;
    this.scaleY = 1;

    this.isBaseline = false;

    this.position_up_line = 0;
    this.position_down_line = 0;
    this.position_left_line = 0;
    this.position_right_line = 0;
    this.position_baseline = 0;
    this.idElementSelected = 0;
    var myPreviewCanvas = this;

    this.image.img.onload = function(){
        myPreviewCanvas.draw();
    }
}

/*!
 * Clear the context to redraw on it
 * \memberof PreviewCanvas
 */
PreviewCanvas.prototype.clear = function() {
    this.ctx.clearRect(0, 0, this.width, this.height);
}

/*!
 * Draw all element on the canvas
 * \memberof Canvas
 */
PreviewCanvas.prototype.draw = function() {
    if(this.visible)
    {
        var ctx = this.ctx;
        this.clear();

        // Save the current context
        ctx.save();

        // get the translation to apply to center the image
        var newWidth = this.width * this.scaleX;
        var newHeight = this.height * this.scaleY;
        var panX = -((newWidth-this.width)/2);
        var panY = -((newHeight-this.height)/2);

        // Apply modifications (zoom, translation) all elements drawing after this will have the modifications
        ctx.translate(panX, panY);
        ctx.scale(this.scaleX,this.scaleY);

        // Draw all elements
        this.image.draw(ctx);

        if(this.isBaseline)
        {
            var rect = this.canvas.getBoundingClientRect();
            var mx = Math.floor((0-rect.left)/(rect.right-rect.left)*this.canvas.width);
            var startX = parseInt((mx - panX) / this.scaleX);

            //Baseline
            ctx.beginPath();
            ctx.moveTo(startX, this.image.y + this.position_baseline);
            ctx.lineTo(this.width*2, this.image.y + this.position_baseline);
            ctx.lineWidth = 1;
            ctx.strokeStyle = COLOR_PREVIEW_BASELINE;
            ctx.stroke();
        }
        else
        {
            //UpLine
            ctx.beginPath();
            ctx.moveTo(0, this.image.y + this.position_up_line);
            ctx.lineTo(this.width, this.image.y + this.position_up_line);
            ctx.lineWidth = 1 / this.scaleY;
            ctx.strokeStyle = COLOR_PREVIEW_LINES_UP;
            ctx.stroke();

            //DownLine
            ctx.beginPath();
            ctx.moveTo(0, this.image.y + this.position_down_line);
            ctx.lineTo(this.width, this.image.y + this.position_down_line);
            ctx.lineWidth = 1 / this.scaleY;
            ctx.strokeStyle = COLOR_PREVIEW_LINES_DOWN;
            ctx.stroke();

            //LeftLine
            ctx.beginPath();
            ctx.moveTo(this.image.x + this.position_left_line, 0);
            ctx.lineTo(this.image.x + this.position_left_line, this.height);
            ctx.lineWidth = 1 / this.scaleX;
            ctx.strokeStyle = COLOR_PREVIEW_LINES_LEFT;
            ctx.stroke();

            //RightLine
            ctx.beginPath();
            ctx.moveTo(this.image.x + this.position_right_line, 0);
            ctx.lineTo(this.image.x + this.position_right_line, this.height);
            ctx.lineWidth = 1 / this.scaleX;
            ctx.strokeStyle = COLOR_PREVIEW_LINES_RIGHT;
            ctx.stroke();

            //Baseline
            ctx.beginPath();
            ctx.setLineDash([5, 2]);
            ctx.moveTo(0, this.image.y + this.position_baseline);
            ctx.lineTo(this.width , this.image.y + this.position_baseline);
            ctx.lineWidth = 1 / this.scaleX;
            ctx.strokeStyle = COLOR_PREVIEW_BASELINE;
            ctx.stroke();
        }
        // Restore the context
        ctx.restore();
    }
}

/*!
 * Zoom the image on a object
 * \memberof Canvas
 * \param obj object to zoom on
 * \param id of the selected object
 */
PreviewCanvas.prototype.zoomTo = function(obj, id){

    if(obj instanceof Rectangle)
    {
        this.idElementSelected = id;
        var rect = obj;
        this.image.x = (this.width/2) - rect.rect.x - rect.rect.w/2;
        this.image.y = (this.height/2) - rect.rect.y - rect.rect.h/2;

        this.scaleX = (this.height /rect.rect.h) - (this.height /rect.rect.h)/2;
        this.scaleY = this.scaleX;

        var actualRect = rect.rect;
        this.isBaseline = false;
    }
    else if (obj instanceof Line)
    {
        var line = obj;
        this.image.x = (this.width/2) - ((line.finishx - line.startx)/2 + line.startx);
        this.image.y = (this.height/2) - line.y;

        this.scaleX = this.width /(line.finishx - line.startx);
        this.scaleY = this.height / (this.image.img.height / 20);

        this.isBaseline = true;
        this.position_baseline = line.y;
        this.idElementSelected = id;
    }
    this.draw();

}

PreviewCanvas.prototype.changeImage = function(imagePath) {
    var canvas = this;
    this.image.img.src = imagePath;
    this.image.img.onload = function(){
        canvas.draw();
    }
}

/*!
 * List of character from labelised Component
 * \class ListCharacter
 */
function ListCharacter(){
    this.list = new Map();
}

/*!
 * Add character to the list
 * \memberof ListCharacter
 * \param character character to add
 */
ListCharacter.prototype.addToCharacter = function(character){
    var number = 0;
    if(this.list.has(character)) {
	number = this.list.get(character);
    }
    this.list.set(character, number+1);
}

/*!
 * Remove character to the list
 * \memberof ListCharacter
 * \param character character to remove
 */
ListCharacter.prototype.removeToCharacter = function(character){
    if(this.list.has(character))
    {
        var number = this.list.get(character);
        number--;
        if(number > 0)
            this.list.set(character, number);
        else
            this.list.delete(character);
    }
}

/*!
 * Draw the list of character
 * \memberof ListCharacter
 */
ListCharacter.prototype.draw = function(){
    var listHTML = $("#letter-list");
    listHTML.empty();
    for (var [key, value] of this.list)
      listHTML.append( "<li class=\"col-sm-3 listItem\" data-character=\"" + key + "\">" + key + "<span class=\"pull-right letter-number\">" + value + "</span></li>");
}

/*!
 * Init all element to usage
 * \param src source of the image
 * \param boundingBox list of boundingBox from the server
 * \param baselines list of baselines from the server
 */
function init(src, boundingBox, baseline, typeOfController) {
    if (typeOfController == "font-creator"){
	var image = new ProcessingImage(src);
	var imagePreview = new ProcessingImage(src);

	var listRect = new Array();
	for (var rect in boundingBox) {
            listRect.push(new Rectangle({x:boundingBox[rect].x, y:boundingBox[rect].y, w:boundingBox[rect].width, h: boundingBox[rect].height},boundingBox[rect].idCC, boundingBox[rect].idLine));
	}
	var boundingBox = new BoundingBox(listRect);

	var listBaseline = new Array();
	for (var line in baseline) {
            listBaseline.push(new Line(baseline[line].idLine, baseline[line].x_begin,baseline[line].y_baseline,baseline[line].x_end));
	}
	var baseline = new Baseline(listBaseline);

	var normalCanvas = new Canvas(document.getElementById('canvas'), image, baseline, boundingBox, "font-creator");
	var previewCanvas = new PreviewCanvas(document.getElementById('small_canvas'), imagePreview);
	var listCharacter = new ListCharacter();
	var controller = new ControllerFontCreator(normalCanvas, previewCanvas, listCharacter);
    } else if (typeOfController == "degradation"){
	var image = new ProcessingImage(src);
	var normalCanvas = new Canvas(document.getElementById('canvas'), image, null, null, "degradation");
	var controller = new ControllerDegradation(normalCanvas);
    } else if(typeOfController == "synthetize"){
	var image = new ProcessingImage(src);
	var normalCanvas = new Canvas(document.getElementById('canvas'), image, null, null, "synthetize");
	var controller = new ControllerSynthetize(normalCanvas);
    } else { // typeOfController == "createDocument"
    var image = new ProcessingImage('data/init.png');
    var normalCanvas = new Canvas(document.getElementById('canvas'), image, null, null, "create-your-document");
	var controller = new ControllerCreateDocument(normalCanvas);
    }
}
