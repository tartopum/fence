$(document).ready(function() {    
    var ids = {
        fence: {
            iconPrefix: "current_"
        },
        light_in1: {
            iconPrefix: "light_"
        },
        light_in2: {
            iconPrefix: "light_"
        },
        light_out: {
            iconPrefix: "light_"
        },
        alarm: {
            iconPrefix: "alarm_"
        },
    };

    function displayError(xhr) {
        alert("Error " + xhr.status + ": " + xhr.responseText);
    }

    function getImageUrl(id, suffix) {
        return "/static/" + ids[id].iconPrefix + suffix;
    }

    function stateUpdated(id, state) {
        if (state) {
            $("#" + id + " img").attr("src", getImageUrl(id, "on.png"));
        } else {
            $("#" + id + " img").attr("src", getImageUrl(id, "off.png"));
        }
        $("#loader").css("visibility", "hidden");
    }

    function updateStates() {
        $("#loader").css("visibility", "visible");
        for(let id in ids) {
            $.get("/" + id, function(data) {
                stateUpdated(id, parseInt(data)); 
                if(id == "alarm") {
                    $.get("/alarm_detector", function(isOn) {
                        if(isOn) {
                            $("#" + id + " img").attr("src", getImageUrl(id, "detector_on.gif"));
                        }
                    });
                }
            })
            .fail(displayError);
        }
    }

    function switchState(id) {
        $("#loader").css("visibility", "visible");
        $.post("/" + id, function(data) {
            updateStates();
        })
        .fail(displayError);
    }

    for(let id in ids) {
        $("#" + id).click(function() {
            switchState(id);
        });
    }
    updateStates();
});
