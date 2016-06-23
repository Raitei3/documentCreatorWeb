function Controller(canvas, previewCanvas, listCharacter) {
    this.canvas = canvas;
    this.previewCanvas = previewCanvas;
    this.listCharacter = listCharacter;

    var controller = this;

    // Init
    $(".container_right").show();
    $('#baseline_options').hide();
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

     // Click on the save button for baseline
    document.getElementById('saveBaseline').addEventListener('click', function(e){
        var value = parseFloat($("#baselineValue").val());
        controller.canvas.baseline.lines[controller.previewCanvas.idElementSelected].y = value;
        controller.canvas.draw();

        session.updateBaseline(controller.canvas.baseline.lines[controller.previewCanvas.idElementSelected].id, value);
    }, true);

    // target the save function when pressing the enter key
    $('.container_right').keypress(function (e) {
        var key = e.which;
        if(key == 13)  // the enter key code
        {
           $("#save").click(); 
        }
    });

    // Click on the trash button
    document.getElementById('button_trash').addEventListener('click', function(e){
        session.removeSession(true);
	alert("removeSession true - trash!");
	
        location.reload();
    }, true); 
    
    // Click on the menu
    document.getElementById('button_menu').addEventListener('click', function(e){
        session.removeSession(true);
	alert("removeSession true - menu!");
        location.reload();
    }, true);   

    // Click on the export button
    document.getElementById('exportFont').addEventListener('click', function(e){
        var fontname = $("#fontName").val();
        session.extractFont(fontname);
    }, true);


    // Click to download image
    document.getElementById('download_img').addEventListener('click', function(){
	var img = controller.canvas.image.img.src;
	var img_split = img.split('/');
	var extension = img.split('.');
	img = img_split[img_split.length - 1];
	extension = extension[extension.length - 1];
	document.getElementById('download_img').setAttribute('href','data/' + img);
	document.getElementById('download_img').setAttribute('download', "doc-online." + extension);
    }, true);

    /* ====================
       === DEGRADATIONS ===
       ==================== */
    
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
    
    
    document.getElementById('GrayScaleCharacterDegradationExec').addEventListener('click', function(){
        session.grayScaleCharsDegradation(
	    document.getElementById('GrayScaleCharacterDegradationValue').value,
	    controller);
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
	alert("removeSession false !");
    };

}

/*!
 * Ask the informations about selected objects
 * \memberof Controller
 * \param e event object 
 */
Controller.prototype.getInfoOnClickedObject = function getInfoOnClickedObject(e)
{
    // get the coordinates of the click
    var mouse = this.canvas.getMouse(e);
    var mx = mouse.x;
    var my = mouse.y;

    // Check if we clic on a BoundingBox
    var id = this.canvas.boundingBox.contains(mx, my, this.canvas.image);
    if(id != 'false')
    {   
        $("#letter_options").show();
        $('#baseline_options').hide();
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
        $('#letter_options').hide();
        $('#baseline_options').show();
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
Controller.prototype.updateInfoOnObject = function(e)
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
Controller.prototype.manipulateInfos = function manipulateInfos(id, left, right, up, down, letter, baseline){

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

Controller.prototype.merge = function merge(e)
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

Controller.prototype.mergeComponent = function mergeComponent(id, idLine, left, right, up, down, jsonId)
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


Controller.prototype.replaceImage = function replaceImage(imagePath)
{
    this.canvas.changeImage("data/" + imagePath);
    this.previewCanvas.changeImage("data/" + imagePath);
}
