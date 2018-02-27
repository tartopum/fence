$(document).ready(function() {    
    function displayError(xhr) {
        alert("Error: " + xhr.responseText);
    }

    function getImageUrl(suffix) {
        return "/static/" + suffix;
    }

    function stateUpdated(id, state) {
        if (state) {
            $("#" + id + " img").attr("src", getImageUrl("light_on.png"));
        } else {
            $("#" + id + " img").attr("src", getImageUrl("light_off.png"));
        }
        $("#loader").css("visibility", "hidden");
    }

    function switchState(id) {
        $("#loader").css("visibility", "visible");
        $.post("/" + id, function(data) {
            stateUpdated(id, parseInt(data));
        })
        .fail(displayError);
    }

    var ids = ["fence", "light_in1", "light_in2", "light_out"];
    for(let id of ids) {
        $("#" + id).click(function() {
            switchState(id);
        });

        $("#loader").css("visibility", "visible");
        $.get("/" + id, function(data) {
            stateUpdated(id, parseInt(data)); 
        })
        .fail(displayError);
    }
});
