$(document).ready(function() {    
    function displayError(xhr) {
        alert("Error: " + xhr.responseText);
    }

    function fenceState() {
        $("#loader").show();
        $.get("/fence", function(data) {
            $("#on, #off").hide();

            var state = parseInt(data);
            if (state) {
                $("#on").show();
            } else {
                $("#off").show();
            }
            $("#loader").hide();
        }).fail(displayError);
    }

    function switchFence() {
        $("#loader").show();
        $.post("/fence", fenceState).fail(displayError);
    }

    $("#on, #off").click(function() {
        switchFence();
    });

    $("#on, #off").hide();
    fenceState();
});
