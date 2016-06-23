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

    $('.fileUpload').hide();
    
    session.startSession(data, askForInfo);  
}

function askForInfo(filename)
{
    session.imageInfos(filename, init);
}
