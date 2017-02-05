var files;

$('input[type=file]').on('change', prepareUpload);

function errorMessage(message)
{
  $("#statusMsg").show(message);
}

function prepareUpload(event)
{
    files = event.target.files;

    event.stopPropagation(); 
    event.preventDefault(); 

    var data = new FormData();
    $.each(files, function(key, value)
    {
        data.append(key, value);
    });

    if(event.target.id == "startDegradation"){
	session.startSession("degradation", data, askForInfo);
    } else if (event.target.id == "startFontCreator"){
	session.startSession("font-creator", data, askForInfo);
    } else if (event.target.id == "BleedThroughImageVersoInput"){
	session.uploadImage(data, updateImgVerso);
    } else if (event.target.id == "startSynthetize"){
	session.uploadImage("synthetize", data, askForInfo);
    }
     else if(event.target.id == startSynthetize){
       session.startSession("synthetize",data,askForInfo)
    }	
}
function updateImgVerso(filename)
{
    $("#BleedThroughImgVerso").attr('src','data/' + filename);
}


function askForInfo(typeOfSession, filename)
{
    session.imageInfos(typeOfSession, filename, init);
}
