/*
  Ce fichier contient les controllers des différentes pages web (font-creator, degradations et create-your-document).
  Chaque controller gère toutes les manipulations faites sur les différentes pages web.
*/


/* 
   ===========================================
   ======    CONTROLLER FONT CREATOR    ======
   ===========================================
*/

function ControllerFontCreator(canvas, previewCanvas, listCharacter) {
    this.canvas = canvas;
    this.previewCanvas = previewCanvas;
    this.listCharacter = listCharacter;

    var controller = this;

    // Init
    $('#fontCreator').hide();
    $('#fontCreatorPractice').show();

    // Select the first Component
    this.canvas.boundingBox.select(0);
    this.canvas.selectedCC = [0];
    session.getInfoOnCC(0,controller.canvas.boundingBox.rects[0].idCC, controller.canvas.boundingBox.rects[0].idLine, controller);
    this.previewCanvas.visible = true;
    this.previewCanvas.draw();


    canvas.canvas.addEventListener('selectstart', function(e) { e.preventDefault(); return false; }, false);

    // Detect when we click
    canvas.canvas.addEventListener('click',function(e) { controller.getInfoOnClickedObject(e); }, true);

    // Detect when we move the image
    canvas.canvas.addEventListener('mousedown', function(e) { canvas.onMouseDown(e); }, true);
    canvas.canvas.addEventListener('mousemove', function(e) { canvas.onMouseMove(e);}, true);
    canvas.canvas.addEventListener('mouseup', function(e) { canvas.onMouseUp(e);}, true);
    
    // Dectection of zoom
    document.getElementById('zoom-in').addEventListener('click', function(e){ canvas.zoomIn();}, true);
    document.getElementById('zoom-out').addEventListener('click', function(e){ canvas.zoomOut();}, true);
    document.getElementById('zoom-reset').addEventListener('click', function(e){ canvas.zoomReset();}, true);

    // Detect the toggle for showing the baseline
    document.getElementById('baseline').addEventListener('change', function(e){
        if(this.checked){
            controller.canvas.baseline.visible = true;
        }
        else{
            controller.canvas.baseline.visible = false;
        }     
        controller.canvas.draw();
    }, true);

    // Change the up line on the preview
    $( "#up" ).change(function() {
        controller.previewCanvas.position_up_line = parseFloat($(this).val());
        controller.previewCanvas.draw();
    });

    // Change the down line on the preview
    $( "#down" ).change(function() {
        controller.previewCanvas.position_down_line = parseFloat($(this).val());
        controller.previewCanvas.draw();
    });

    // Change the left line on the preview
    $( "#left" ).change(function() {
        controller.previewCanvas.position_left_line = parseFloat($(this).val());
        controller.previewCanvas.draw();
    });

    // Change the right line on the preview
    $( "#right" ).change(function() {
        controller.previewCanvas.position_right_line = parseFloat($(this).val());
        controller.previewCanvas.draw();
    });

    // Change the baseline on the preview
    $( "#baselineCC" ).change(function() {
        controller.previewCanvas.position_baseline = parseFloat($(this).val());
        controller.previewCanvas.draw();
    }); 

    $( "#baselineValue" ).change(function() {
        controller.previewCanvas.position_baseline = parseFloat($(this).val());
        controller.previewCanvas.draw();
    }); 

    // Click on the save button for Component
    document.getElementById('save').addEventListener('click', function(e){ controller.updateInfoOnObject(e); }, true);

    // Click on the merge button
    document.getElementById('mergeButton').addEventListener('click', function(e){ controller.merge(e); }, true);

    // target the save function when pressing the enter key
    $('#fontCreatorPractice').keypress(function (e) {
        var key = e.which;
        if(key == 13)  // the enter key code
        {
           $("#save").click(); 
        }
    });

    // Click on the trash button
    document.getElementById('resetImage').addEventListener('click', function(e){
        session.removeSession(true);	
        location.reload();
    }, true); 

    
    // Click on the export button
    document.getElementById('exportFont').addEventListener('click', function(e){
        var fontname = $("#fontName").val();
        session.extractFont(fontname);
    }, true);

    
    // Highlight labelised Component when we hover the letter in the letter list
    $("#letter-list").on({
        mouseenter: function(){
            var character = $(this).data("character");
            for (var i = 0, len = controller.canvas.boundingBox.rects.length; i < len; i++) {
                if(controller.canvas.boundingBox.rects[i].labeled == character)
                    controller.canvas.boundingBox.rects[i].hover = true;
            }
            controller.canvas.draw();
        },
        mouseleave: function(){
            var character = $(this).data("character");
            for (var i = 0, len = controller.canvas.boundingBox.rects.length; i < len; i++) {
                if(controller.canvas.boundingBox.rects[i].labeled == character)
                    controller.canvas.boundingBox.rects[i].hover = false;
            }
            controller.canvas.draw();
        }
    }, '.listItem');
    
    // Increment and Decrement the input values
    (function ($) {
      $('.spinner .btn:first-of-type').on('click', function() {
        var id = $(this).data("id");
        $("#"+id).val(parseInt($("#"+id).val(), 10) - 1).change();
      });
      $('.spinner .btn:last-of-type').on('click', function() {
        var id = $(this).data("id");
        $("#"+id).val(parseInt($("#"+id).val(), 10) + 1).change();
      });
    })(jQuery);

    // When the user quit the application
    window.onunload = function() { 
	session.removeSession(false);
    };
}

/*!
 * Ask the informations about selected objects
 * \memberof Controller
 * \param e event object 
 */
ControllerFontCreator.prototype.getInfoOnClickedObject = function getInfoOnClickedObject(e)
{
    // get the coordinates of the click
    var mouse = this.canvas.getMouse(e);
    var mx = mouse.x;
    var my = mouse.y;

    // Check if we clic on a BoundingBox
    var id = this.canvas.boundingBox.contains(mx, my, this.canvas.image);
    if(id != 'false')
    {   
        this.canvas.baseline.select("false");

        // If its a ctrl/cmd click 
        if(e.metaKey || e.ctrlKey)
        {
            var i = this.canvas.selectedCC.indexOf(id);
            if(i != -1)
            {
                if(this.previewCanvas.idElementSelected == id)
                {
                    if(this.canvas.selectedCC.length > 1)
                    {
                        session.getInfoOnCC(this.canvas.selectedCC[1],this.canvas.boundingBox.rects[this.canvas.selectedCC[1]].idCC, this.canvas.boundingBox.rects[this.canvas.selectedCC[1]].idLine, this);
                        this.canvas.selectedCC.splice(i, 1);
                        this.canvas.boundingBox.removeToSelection(id);
                    }
                }
                else
                {
                    this.canvas.selectedCC.splice(i, 1);
                    this.canvas.boundingBox.removeToSelection(id);
                }
            }
            else
            {
                this.canvas.selectedCC.push(id);
                this.canvas.boundingBox.addToSelection(id);
            }
            if(this.canvas.selectedCC.length > 1)
                $("#mergeButton").removeAttr('disabled');
            else
                $("#mergeButton").prop("disabled", true);
        }
        else
        {
            $("#mergeButton").prop("disabled", true);
            this.canvas.boundingBox.select(id);
            this.canvas.selectedCC = [id];
            session.getInfoOnCC(id,this.canvas.boundingBox.rects[id].idCC, this.canvas.boundingBox.rects[id].idLine, this);
        } 
        this.canvas.draw();
        return;  
    }
    
    // Check if we clic on a Baseline
    var id = this.canvas.baseline.contains(mx, my, this.canvas.image);
    if(id != 'false')
    {
        this.canvas.boundingBox.select("false");
        this.canvas.baseline.select(id);
        this.previewCanvas.zoomTo(this.canvas.baseline.lines[id], id);
        this.previewCanvas.visible = true;
        $("#baselineValue").val(this.canvas.baseline.lines[id].y);
        this.canvas.draw();
    }  
}

/*!
 * Update the informations on the server
 * \memberof Controller
 * \param e event object 
 */
ControllerFontCreator.prototype.updateInfoOnObject = function(e)
{
    if($("#letter").val() != "")
    {
        // label characters
        for(var i = 0; i < this.canvas.selectedCC.length; i++)
        {
            if(this.canvas.boundingBox.rects[this.canvas.selectedCC[i]].labeled != false)
                this.listCharacter.removeToCharacter(this.canvas.boundingBox.rects[this.canvas.selectedCC[i]].labeled);

            this.canvas.boundingBox.rects[this.canvas.selectedCC[i]].labeled = $("#letter").val();
            this.listCharacter.addToCharacter($("#letter").val());
        }
    }
    else
    {
        for(var i = 0; i < this.canvas.selectedCC.length; i++)
        {
            if(this.canvas.boundingBox.rects[this.canvas.selectedCC[i]].labeled != false)
                this.listCharacter.removeToCharacter(this.canvas.boundingBox.rects[this.canvas.selectedCC[i]].labeled);
            this.canvas.boundingBox.rects[this.canvas.selectedCC[i]].labeled = false;
        }
    }
    var ids = this.canvas.selectedCC;

    // Construct the json object containing all components ids
    var jsonId = "{";
    for (index = 0; index < ids.length; index++) 
    {
        jsonId += "\"" + index + "\"\:{";
        jsonId += "\"idCC\"\:" + this.canvas.boundingBox.rects[ids[index]].idCC + ",";
        jsonId += "\"idLine\"\:" + this.canvas.boundingBox.rects[ids[index]].idLine;
        jsonId += "}";
        if(index < ids.length-1)
            jsonId += ",";    
    }
    jsonId += "}";

    this.canvas.boundingBox.rects[this.previewCanvas.idElementSelected].rect.x = parseFloat($("#left").val());
    this.canvas.boundingBox.rects[this.previewCanvas.idElementSelected].rect.y = parseFloat($("#up").val());
    this.canvas.boundingBox.rects[this.previewCanvas.idElementSelected].rect.w = parseFloat($("#right").val()) - parseFloat($("#left").val());
    this.canvas.boundingBox.rects[this.previewCanvas.idElementSelected].rect.h = parseFloat($("#down").val()) - parseFloat($("#up").val());

    session.updateInfoOnCC(this.canvas.boundingBox.rects[this.previewCanvas.idElementSelected].idCC, this.canvas.boundingBox.rects[this.previewCanvas.idElementSelected].idLine,jsonId, parseFloat($("#left").val()), parseFloat($("#right").val()),  parseFloat($("#up").val()), parseFloat($("#down").val()),parseInt($("#baselineCC").val()), $("#letter").val());

    this.canvas.draw();
    this.listCharacter.draw();
}

/*!
 * Update infos on the client with values receive from the server
 * \memberof Controller
 * \param id id of the wanted object
 * \param left value of the left line
 * \param right value of the right line
 * \param up value of the up line
 * \param down value of the down line
 * \param letter value of the letter
 * \param baseline value of the baseline
 */
ControllerFontCreator.prototype.manipulateInfos = function manipulateInfos(id, left, right, up, down, letter, baseline){

    this.previewCanvas.zoomTo(this.canvas.boundingBox.rects[id], id);
    this.previewCanvas.visible = true;

    // Set all lines
    this.previewCanvas.position_up_line = up;
    this.previewCanvas.position_down_line = down;
    this.previewCanvas.position_left_line = left;
    this.previewCanvas.position_right_line = right;
    this.previewCanvas.position_baseline = baseline;

    // Set the labeled letter
    if(letter == "null")
        $("#letter").val("");
    else
        $("#letter").val(letter);

    // Set all fields data
    $("#up").val(up);
    $("#up").data('old-value', up);
    $("#down").val(down);
    $("#down").data('old-value', down);
    $("#left").val(left);
    $("#left").data('old-value', left);
    $("#right").val(right);
    $("#right").data('old-value', right);
    $("#baselineCC").val(baseline);
    $("#baselineCC").data('old-value', baselineCC);
    this.canvas.draw();
    this.previewCanvas.draw();
}

ControllerFontCreator.prototype.merge = function merge(e)
{
    var ids = this.canvas.selectedCC;
    // Construct the json object containing all components ids
    var jsonId = "{";
    for (index = 0; index < ids.length; index++) 
    {
        jsonId += "\"" + index + "\"\:{";
        jsonId += "\"idCC\"\:" + this.canvas.boundingBox.rects[ids[index]].idCC + ",";
        jsonId += "\"idLine\"\:" + this.canvas.boundingBox.rects[ids[index]].idLine + ",";
        jsonId += "\"id\"\:" + ids[index];
        jsonId += "}";
        if(index < ids.length-1)
            jsonId += ",";    
    }
    jsonId += "}";

    session.merge(this.canvas.boundingBox.rects[this.previewCanvas.idElementSelected].idCC, this.canvas.boundingBox.rects[this.previewCanvas.idElementSelected].idLine,jsonId, this);
}

ControllerFontCreator.prototype.mergeComponent = function mergeComponent(id, idLine, left, right, up, down, jsonId)
{
    for(var i in jsonId) { 
        if(jsonId[i].idCC == id && jsonId[i].idLine == idLine)
        {
            this.canvas.boundingBox.rects[jsonId[i].id].rect.x = left;
            this.canvas.boundingBox.rects[jsonId[i].id].rect.y = up;
            this.canvas.boundingBox.rects[jsonId[i].id].rect.w = right - left;
            this.canvas.boundingBox.rects[jsonId[i].id].rect.h = down - up;
        }
        else
        {
            //this.canvas.boundingBox.rects.splice(jsonId[i].id, 1);
            this.canvas.boundingBox.rects[jsonId[i].id].rect.x = -1;
            this.canvas.boundingBox.rects[jsonId[i].id].rect.y = -1;
            this.canvas.boundingBox.rects[jsonId[i].id].rect.w = 0;
            this.canvas.boundingBox.rects[jsonId[i].id].rect.h = 0;
            if(this.canvas.boundingBox.rects[jsonId[i].id].labeled != false)
                this.listCharacter.removeToCharacter(this.canvas.boundingBox.rects[jsonId[i].id].labeled);
        }
    }
    this.canvas.boundingBox.select(this.previewCanvas.idElementSelected);
    this.canvas.selectedCC = [this.previewCanvas.idElementSelected];
    this.manipulateInfos(this.previewCanvas.idElementSelected, left, right, up, down, $("#letter").val(), down);
    this.listCharacter.draw();
}


ControllerFontCreator.prototype.replaceImage = function replaceImage(imagePath)
{
    this.canvas.changeImage("data/" + imagePath);
    this.previewCanvas.changeImage("data/" + imagePath);
}

/* 
   ==========================================
   ======    CONTROLLER DEGRADATION    ======
   ==========================================
 */

function ControllerDegradation(canvas) {
    this.canvas = canvas;

    var controller = this;
    
    // Init
    $('#degradation').hide();
    $('#degradationPractice').show();

    // Detect when we move the image
    canvas.canvas.addEventListener('mousedown', function(e) { canvas.onMouseDown(e); }, true);
    canvas.canvas.addEventListener('mousemove', function(e) { canvas.onMouseMove(e);}, true);
    canvas.canvas.addEventListener('mouseup', function(e) { canvas.onMouseUp(e);}, true);
    
    // Dectection of zoom
    document.getElementById('zoom-in').addEventListener('click', function(e){ canvas.zoomIn();}, true);
    document.getElementById('zoom-out').addEventListener('click', function(e){ canvas.zoomOut();}, true);
    document.getElementById('zoom-reset').addEventListener('click', function(e){ canvas.zoomReset();}, true);

    // Click on the trash button
    document.getElementById('resetImage').addEventListener('click', function(e){
        session.removeSession(true);
        location.reload();
    }, true);
    
    // Click to download image
    document.getElementById('downloadImage').addEventListener('click', function(){
	var img = controller.canvas.image.img.src;
	var img_split = img.split('/');
	var extension = img.split('.');
	img = img_split[img_split.length - 1];
	extension = extension[extension.length - 1];

	document.getElementById('downloadImage').setAttribute('href','data/' + img);
	document.getElementById('downloadImage').setAttribute('download', 'doc-online.' + extension);
    }, true);

    /* =======================
       === DEGRADATIONS 2D ===
       ======================= */
    
    // Blur Filter
    document.getElementById('blurFilterExec').addEventListener('click', function(){
	var tabTypeIntensity = document.getElementsByName("blurFilterTypeIntensity");
	var typeIntensity;
	for (var i = 0; i < tabTypeIntensity.length; i++) {
	    if (tabTypeIntensity[i].checked) {
		typeIntensity = tabTypeIntensity[i].value;
		break;
	    }
	}

	var intensity;
	if (typeIntensity == "value"){
	    intensity = document.getElementById('blurFilterIntensityValue').value;
	} else { // typeIntensity == "image"
	    intensity = document.getElementById('blurFilterIntensityImage').src;
	    intensity = intensity.split('/');
	    intensity = intensity[intensity.length - 1];
	}

	var tabMethod = document.getElementsByName("blurFilterMethod");
	var method;
	for (var i = 0; i < tabMethod.length; i++) {
	    if (tabMethod[i].checked) {
		method = tabMethod[i].value;
		break;
	    }
	}

	session.blurFilter(method, typeIntensity, intensity, controller);
    }, true);

    // Blur Filter - change l'image du blur
    document.getElementById('changeImgBlurFilter').addEventListener('click', function(event){
	if(event.target.nodeName == "BUTTON") {
	    var src = document.getElementById('blurFilterIntensityImage').src;
	    numImg = src.charAt(src.length - 5);
	    if ($(event.target).data("id") == "changeImgLeft") {
		numImg = parseInt(numImg) - 1;
		if (numImg < 1){
		    numImg = 7;
		}
	    } else if ($(event.target).data("id") == "changeImgRight") {
		numImg = parseInt(numImg) + 1;
		if (numImg > 7){
		    numImg = 1;
		}
	    }
	    var new_src = "img/blurExamples/blurGenerated" + numImg + ".png";
	    document.getElementById('blurFilterIntensityImage').setAttribute('src', new_src);
	}
    });

    // Bleed Through
    document.getElementById('BleedThroughExec').addEventListener('click', function(){
        session.bleedThrough(
	    document.getElementById('BleedThroughNbIteration').value,
	    document.getElementById('BleedThroughImgVerso').src,
	    controller);
    }, true);

    
    // Shadow Binding
    document.getElementById('ShadowBindingExec').addEventListener('click', function(){
	var all_border = document.getElementsByName("ShadowBindingBorder");
	var border;
	for (var i = 0; i < all_border.length; i++) {
	    if (all_border[i].checked) {
		border = all_border[i].value;
		break;
	    }
	}
	
        session.shadowBinding(
	    border,
	    document.getElementById('ShadowBindingWidth').value,
	    document.getElementById('ShadowBindingIntensity').value,
	    document.getElementById('ShadowBindingAngle').value,
	    controller);
    }, true);
    

    // Phantom Character
    document.getElementById('PhantomCharacterExec').addEventListener('click', function(){
	var all_frequency = document.getElementsByName('PhantomCharacterFrequency');
	var frequency;
	for (var i = 0; i < all_frequency.length; i++) {
	    if (all_frequency[i].checked) {
		frequency = all_frequency[i].value;
		break;
	    }
	}
	
        session.phantomCharacter(
	    frequency,
	    controller);
    }, true);
    
    // GrayScale Character Degradation
    document.getElementById('GrayScaleCharacterDegradationExec').addEventListener('click', function(){
        session.grayScaleCharsDegradation(
	    document.getElementById('GrayScaleCharacterDegradationValue').value,
	    controller);
    }, true);


    // When the user quit the application
    window.onunload = function() { 
	session.removeSession(false);
    };
}


ControllerDegradation.prototype.replaceImage = function replaceImage(imagePath)
{
    this.canvas.changeImage("data/" + imagePath);
}


/* 
   =============================================
   ===    CONTROLLER CREATE YOUR DOCUMENT    ===
   =============================================
*/

function ControllerCreateDocument() {
    var controller = this;
    
    // Init
    $('#createDocument').hide();
    $('#createDocumentPractice').show();

    // Récupère les fonts situés dans le dossier server/data/font/
    session.getElemsDirectory("font", controller);
    // Récupère les backgrounds situés dans le dossier server/data/background/
    session.getElemsDirectory("background", controller);

    // Envois les informations nécessaire pour télécharger le document à créé (font, background, texte, format XML/PNG)
    var downloads = document.getElementsByName("createDocumentDownload");
    for (var i = 0; i < downloads.length; i++){
	document.getElementById(downloads[i].id).addEventListener('click', function(event){	    
	    var fonts = document.getElementById("createDocumentFont");
	    var font = fonts.options[fonts.selectedIndex].value;

	    var backgrounds = document.getElementById("createDocumentBackground");
	    var background = backgrounds.options[backgrounds.selectedIndex].value;

	    var text = document.getElementById("createDocumentText").value;
	    
	    var type;
	    if (this.id == "createDocumentDownloadXML"){
		type = "xml";
	    } else { // this.id == "createDocumentDownloadPNG"
		type = "png";
	    }
	    	    
	    session.downloadCreateDocument(type, font, background, text, controller);
	}, true);
    }
}

// Mets à jour les selects "Font" et "Background" à partir des données renvoyées par le serveur.
ControllerCreateDocument.prototype.updateElemsDirectory = function(directory, elems){
    var tabElems = elems.split(';');
    var nbElems = tabElems[0];
    
    var idSelect = "#createDocument" + directory.charAt(0).toUpperCase() + directory.substring(1).toLowerCase()
    var selectHTML = $(idSelect);
    selectHTML.empty();
    
    for (var i = 1; i <= nbElems; i++){
	selectHTML.append("<option value=\"" + tabElems[i] +"\">" + tabElems[i] + "</option>")
    }
}

// Modifie les liens de téléchargement de "Download XML" et "DownloadPNG"
ControllerCreateDocument.prototype.changeDownload = function(filename){
    var downloadXML = document.getElementById("createDocumentDownloadXML");
    downloadXML.setAttribute('href','background/' + filename);
    downloadXML.setAttribute('download', 'doc-online.xml');

    var downloadPNG = document.getElementById("createDocumentDownloadPNG");
    downloadPNG.setAttribute('href','background/' + filename);
    downloadPNG.setAttribute('download', 'doc-online.png');
}

/*
   =============================================
   ===    CONTROLLER SYNTHETIZE_IMAGE        ===
   =============================================
*/
function ControllerSynthetize(canvas) {
    this.canvas = canvas;

    var controller = this;

    // Init
    $('#synthetize').hide();
    $('#synthetizePractice').show();

var el = document.getElementById("synthetizeExec");
if (el.addEventListener)
    //el.addEventListener("click", function(){alert("button detected");},true);

}
ControllerSynthetize.prototype.replaceImage = function replaceImage(imagePath)
{    
    this.canvas.changeImage("data/" + imagePath);
   
}

