$(document).ready(function() {    
    function updateState() {
        $("#loader").show();
        $.get("/state", function(data) {
            $("#on, #off").hide();

            var state = parseInt(data);
            if (state) {
                $("#on").show();
            } else {
                $("#off").show();
            }
            $("#loader").hide();
        });
    }

    function command(url) {
        $("#loader").show();
        $.get(url, updateState);
    }

    $("#on").click(function() {
        command("/off");
    });

    $("#off").click(function() {
        command("/on");
    });

    $("#on, #off").hide();
    updateState();
});
