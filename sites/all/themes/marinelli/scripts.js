$(document).ready(function(){
    // unhide the location fieldset's first description block
    $('fieldset.location .description:first').css('display','block');
    
    // tasteful fading of status messages
    $('div.messages.status').animate({backgroundColor: '#ffff00'},500).animate({ backgroundColor: '#D8FAB6' }, 1000);
});