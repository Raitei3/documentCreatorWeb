/*!
 * Session to communate with the server
 * \class Session
 */
function Session() {
    this.token;
    var that = this;
}

/*!
 * Start the communication with server by sending the image
 * The server will return the token of the session
 * \memberof Session
 * \param file file to upload
 * \param callback callback method
 */
Session.prototype.startSession = function(typeOfSession, file, callback)
{
    // Loading panel
    $('.overlay').show();
    $('.loader').show();

    $.ajax({
	url: 'startSession.txt',
	type: 'POST',
	data: file,
	cache: false,
	processData: false,
	contentType: false,
	context : this,
	success: function(responseJson, textStatus, jqXHR)
	{
    	    var response = JSON.parse(responseJson);
    	    if(response.error == null)
    	    {
    		this.token = response.token;
    		callback(typeOfSession, response.filename);
    	    }
	    $('.overlay').hide();
	    $('.loader').hide();
	},
	error: function(jqXHR, textStatus, errorThrown)
	{
            console.log('ERRORS: ' + textStatus);
	}
    });
}


/*!
 * Delete the session with the server
 * \memberof Session
 */
Session.prototype.removeSession = function(isAsync)
{
    $.ajax({
	url: 'stopSession.txt',
	type: 'POST',
	data: "token=" + this.token,
	async: isAsync,
	success: function(data, textStatus, jqXHR)
	{
	},
	error: function(error)
	{
 	    console.log('ERRORS: ' + error);
	}
    });
}


Session.prototype.uploadImage = function(file, callback)
{
    // Loading panel
    $('.overlay').show();
    $('.loader').show();

    $.ajax({
	url: 'uploadImage.txt',
	type: 'POST',
	data: file,
	cache: false,
	processData: false,
	contentType: false,
	context : this,
	success: function(responseJson, textStatus, jqXHR)
	{
    	    var response = JSON.parse(responseJson);
    	    if(response.error == null)
    	    {
    		callback(response.filename);
    	    }
	    $('.overlay').hide();
	    $('.loader').hide();
	},
	error: function(jqXHR, textStatus, errorThrown)
	{
            console.log('ERRORS: ' + textStatus);
	}
    });
}


/*!
 * Get informations about the image
 * \memberof Session
 * \param filename filename where the image is stored
 * \param callback callback method
 */
Session.prototype.imageInfos = function(typeOfSession, filename, callback)
{
    var imagePath = "data/" + filename;

    $.ajax({
	url: 'getBoundingBox.txt',
	type: 'POST',
	data: "token=" + this.token,
	success: function(data, textStatus, jqXHR)
	{
	    $('#titlesession').hide();
	    $('#tutosession').hide();
	    $('#canvas').show();
	    $('.zoom-icon').show();
	    $('.navbar-nav').show();

	    callback(imagePath, JSON.parse(data).boundingbox, JSON.parse(data).baseline, typeOfSession);

	    $('.loader').hide();
	    $('.overlay').hide();
	},
	error: function(error)
	{
 	    console.log('ERRORS: ' + error);
	}
    });
}


/*!
 * Get informations on a Component
 * \memberof Session
 * \param id id of the Component in the client
 * \param idCC id of the component
 * \param idLine id of the line where is the component
 * \param callback callback method
 */
Session.prototype.getInfoOnCC = function(id, idCC, idLine, callback)
{
    $.ajax({
	url: 'getInfoOnCC.txt',
	type: 'POST',
	data: "token=" + this.token + "&idCC=" + idCC +"&idLine="+ idLine,
	context: callback,
	success: function(data, textStatus, jqXHR)
	{
    	    var info = JSON.parse(data);
    	    callback.manipulateInfos(id, info.left, info.right, info.up, info.down, info.letter, info.baseline);
	},
	error: function(error)
	{
 	    console.log('ERRORS: ' + error);
	}
    });
}

/*!
 * update component value on the server
 * \memberof Session
 * \param activeId id of the component selected on the preview canvas
 * \param activeLine id of the line where is the component selected on the preview canvas
 * \param jsonId llist of identifiers of components that we want to change
 * \param left value of the left line
 * \param right value of the right line
 * \param up value of the up line
 * \param down value of the down line
 * \param baseline value of the baseline line
 * \param letter value of the letter
 */
Session.prototype.updateInfoOnCC = function(activeId, activeLine, jsonId, left, right, up, down, baseline,letter)
{
    $.ajax({
	url: 'updateInfoOnCC.txt',
	type: 'POST',
	data: "token=" + this.token + "&activeid=" + activeId + "&activeline=" + activeLine + "&id=" + jsonId + "&left=" + left + "&right=" + right + "&up=" + up + "&down=" + down + "&baseline=" + baseline + "&letter=" + letter,
	success: function(data, textStatus, jqXHR)
	{
	},
	error: function(error)
	{
 	    console.log('ERRORS: ' + error);
	}
    });
}

/*!
 * Ask the xml font document to the server
 * \memberof Session
 * \param fontname name we want to give the font
 */
Session.prototype.extractFont = function(fontname)
{
    $('.overlay').show();
    $('.loader').show();

    $.ajax({
	url: 'extractFont.txt',
	type: 'POST',
	data: "token=" + this.token +"&fontname=" + fontname,
	success: function(data, textStatus, jqXHR)
	{
            var file = new Blob([data], {type: "text/plain;charset=utf-8"});
            saveAs(file, fontname + ".of");

            $('.loader').hide();
            $('.overlay').hide();
            $('#saveModal').modal('hide');

	},
	error: function(error)
	{
 	    console.log('ERRORS: ' + error);
	}
    });
}

/*!
 * update a baseline value on the server
 * \memberof Session
 * \param idLine id of the line on which we want to change the baseline
 * \param value new value of this baseline
 */
Session.prototype.updateBaseline = function(idLine, value)
{
    $.ajax({
	url: 'updateBaseline.txt',
	type: 'POST',
	data: "token=" + this.token +"&idLine=" + idLine + "&value=" + value,
	success: function(data, textStatus, jqXHR)
	{

	},
	error: function(error)
	{
            console.log('ERRORS: ' + error);
	}
    });
}
/*!
 * merge different Components
 * \memberof Session
 * \param activeId id of the component selected on the preview canvas
 * \param activeLine id of the line where is the component selected on the preview canvas
 * \param jsonId list of identifiers of components that we want to change
 */
Session.prototype.merge = function(activeId, activeLine, jsonId, callback)
{
    $.ajax({
        url: 'merge.txt',
        type: 'POST',
        data: "token=" + this.token + "&activeid=" + activeId + "&activeline=" + activeLine + "&id=" + jsonId,
        context: callback,
        success: function(data, textStatus, jqXHR)
        {
            var info = JSON.parse(data);
            callback.mergeComponent(info.id, info.idLine, info.left, info.right, info.up, info.down, JSON.parse(jsonId));
        },
        error: function(error)
        {
            console.log('ERRORS: ' + error);
        }
    });
}

// Envois les informations nécessaire pour faire la GrayScale Characters Degradations, et récupère le path de l'image dégradé.
Session.prototype.grayScaleCharsDegradation = function(level, callback)
{
    // Loading panel
    $('.overlay').show();
    $('.loader').show();

    $.ajax({
        url: 'grayScaleCharsDegradation.txt',
        type: 'POST',
        data: 'token=' + this.token + '&level=' + level,
        context: callback,
        success : function(data, textStatus, jqXHR)
        {
            var response = JSON.parse(data);
    	    if(response.filename == null)
    	    {
		alert(response.error);
	    } else {
		callback.replaceImage(response.filename);
	    }

            $('.overlay').hide();
            $('.loader').hide();
        },
        error: function(error)
        {
            console.log('ERRORS: ' + error);
        }
    });
}

// Envois les informations nécessaire pour faire Shadow Binding, et récupère le path de l'image dégradé.
Session.prototype.shadowBinding = function(border, width, intensity, angle, callback)
{
    // Loading panel
    $('.overlay').show();
    $('.loader').show();

    $.ajax({
        url: 'shadowBinding.txt',
        type: 'POST',
        data: 'token=' + this.token + '&border=' + border + '&width=' + width + "&intensity=" + intensity + "&angle=" + angle,
        context: callback,
        success : function(data, textStatus, jqXHR)
        {
            var response = JSON.parse(data);
    	    if(response.filename == null)
    	    {
		alert(response.error);
	    } else {
		callback.replaceImage(response.filename);
	    }

            $('.overlay').hide();
            $('.loader').hide();
        },
        error: function(error)
        {
            console.log('ERRORS: ' + error);
        }
    });
}

// Envois les informations nécessaire pour faire Phantom Character, et récupère le path de l'image dégradé.
Session.prototype.phantomCharacter = function(frequency, callback)
{
    // Loading panel
    $('.overlay').show();
    $('.loader').show();

    $.ajax({
        url: 'phantomCharacter.txt',
        type: 'POST',
        data: 'token=' + this.token + '&frequency=' + frequency,
        context: callback,
        success : function(data, textStatus, jqXHR)
        {
	    var response = JSON.parse(data);
    	    if(response.filename == null)
    	    {
		alert(response.error);
	    } else {
		callback.replaceImage(response.filename);
	    }

            $('.overlay').hide();
            $('.loader').hide();
        },
        error: function(error)
        {
            console.log('ERRORS: ' + error);
        }
    });
}

// Envois les informations nécessaires pour faire Bleed Through, et récupère le path de l'image dégradé.
Session.prototype.bleedThrough = function(nbIterations, imgVerso, callback)
{
    // Loading panel
    $('.overlay').show();
    $('.loader').show();
    var img_split = imgVerso.split("/");
    filenameImgVerso = img_split[img_split.length - 1];

    $.ajax({
        url: 'bleedThrough.txt',
        type: 'POST',
        data: 'token=' + this.token + '&nbIterations=' + nbIterations + '&imgVerso=' + filenameImgVerso,
        context: callback,
        success : function(data, textStatus, jqXHR)
        {
	    var response = JSON.parse(data);
    	    if(response.filename == null)
    	    {
		alert(response.error);
	    } else {
		callback.replaceImage(response.filename);
	    }

            $('.overlay').hide();
            $('.loader').hide();
        },
        error: function(error)
        {
            console.log('ERRORS: ' + error);
        }
    });
}

// Envois les informations nécessaires pour faire BlurFilter, et récupère le path de l'image dégradé.
Session.prototype.blurFilter = function(method, typeIntensity, intensity, callback)
{
    // Loading panel
    $('.overlay').show();
    $('.loader').show();

    $.ajax({
        url: 'blurFilter.txt',
        type: 'POST',
        data: 'token=' + this.token + '&method=' + method + '&typeIntensity=' + typeIntensity + '&intensity=' + intensity,
        context: callback,
        success : function(data, textStatus, jqXHR)
        {
            var response = JSON.parse(data);
    	    if(response.filename == null)
    	    {
		alert(response.error);
	    } else {
		callback.replaceImage(response.filename);
	    }
            $('.overlay').hide();
            $('.loader').hide();
        },
        error: function(error)
        {
            console.log('ERRORS: ' + error);
        }
    });
}

//--------------test blur effect 2 -------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Session.prototype.blurFilter2 = function(method, typeIntensity, intensity, callback)
{
    // Loading panel
    $('.overlay').show();
    $('.loader').show();

    $.ajax({
        url: 'blurFilter2.txt',
        type: 'POST',
        data: 'token=' + this.token + '&method=' + method + '&typeIntensity=' + typeIntensity + '&intensity=' + intensity,
        context: callback,
        success : function(data, textStatus, jqXHR)
        {
            var response = JSON.parse(data);
    	    if(response.filename == null)
    	    {
		alert(response.error);
	    } else {
		callback.replaceImage(response.filename);
	    }
            $('.overlay').hide();
            $('.loader').hide();
        },
        error: function(error)
        {
            console.log('ERRORS: ' + error);
        }
    });
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// Envois les informations nécessaires pour créer un document, et récupère le path du document créer.
Session.prototype.downloadCreateDocument = function(typeDownload, font, background, text, callback)
{
    // Loading panel
    $('.overlay').show();
    $('.loader').show();

    $.ajax({
        url: 'downloadCreateDocument.txt',
        type: 'POST',
        data: 'typeDownload=' + typeDownload + '&font=' + font + '&background=' + background + '&text=' + text,
        context: callback,
        success : function(data, textStatus, jqXHR)
        {
	    callback.changeDownload(data);

            $('.overlay').hide();
            $('.loader').hide();
        },
        error: function(error)
        {
            console.log('ERRORS: ' + error);
        }
    });
}

// Envois le nom du répertoire situé dans server/data/ duquel on veut récupèrer tous les fichiers,
// et récupère la liste des fichiers espacés par un ";" et possédant au tout début de la chaîne de caractère,
// le nombre de fichiers.
Session.prototype.getElemsDirectory = function(directory, callback)
{
    // Loading panel
    $('.overlay').show();
    $('.loader').show();

    $.ajax({
        url: 'getElemsDirectory.txt',
        type: 'POST',
        data: 'directory=' + directory,
        context: callback,
        success : function(data, textStatus, jqXHR)
        {
	    callback.updateElemsDirectory(directory, data);

            $('.overlay').hide();
            $('.loader').hide();
        },
        error: function(error)
        {
            console.log('ERRORS: ' + error);
        }
    });
}
