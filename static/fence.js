$(document).ready(function() {    
    var ARDUINO_URL = "http://192.168.167.100";

    function getArduinoUrl(suffix) {
        return ARDUINO_URL + "/fence/" + suffix;
    }

    function updateState() {
        $.get(getArduinoUrl("state"), function(data) {
            var state = parseInt(data);
            if (state) {
                $("#state").attr("src", "/static/on.png");
                $("#state").attr("data-state", 1);
            } else {
                $("#state").attr("src", "/static/off.png");
                $("#state").attr("data-state", 0);
            }
            $("#loader").hide();
        });
    }

    $("#state").click(function() {
        $("#loader").show();
        if (parseInt($("#state").attr("data-state"))) {
            $.get(getArduinoUrl("off"), updateState);
        } else {
            $.get(getArduinoUrl("on"), updateState);
        }
    });

    updateState();
});
