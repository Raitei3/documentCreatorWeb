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

    
    if (event.target.id == "img_start_session"){
	$('.fileUpload').hide();
	session.startSession(data, askForInfo);
    }
    
    else if (event.target.id == "img_verso_bleed_through"){
	session.uploadImgVerso(data, updateImgVerso);
    } 
}

function updateImgVerso(filename)
{
    $("#BleedThroughImgVerso").attr('src','data/' + filename);
}

function askForInfo(filename)
{
    session.imageInfos(filename, init);
}
