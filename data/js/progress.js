var duration=30000;
	
$(document).ready(function(){
process();
$('.form-inline').on('submit',function()
        {  
          duration=1000;
          return true;
        });
});

function process(){
    $.getJSON(
    "/getdata",
    {page:"update"},
    function(data){
        var ps=0;
        if (data['WORK']==1){
            if (data['ALL']>0) ps=Math.round(data['PROGRESS']/data['ALL']);
            $('PRG').css('width',ps+'%');
            $('PRGVAL').innerhtml(ps);
            duration=1;
        }
        else{
            $('PRG').css('width',ps+'%');
            $('PRGVAL').innerhtml(ps);
            duration=30000;
        }
        $('PRGTITLE').innerhtml(data['ERROR']);
    });
    
    setTimeout('process()',duration);
}

