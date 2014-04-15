$(document).ready(function(){
	setAll();
});

$(window).load(function(){
	
});

function setAll() {
	$("#submit").on("click", function(){
		var vs =  parseFloat($("#range").val());

		// $.get( "controller.php", {
		// 	temperature : data.temperature,
		// 	motion : data.motion,
		// 	sound : data.sound,
		// 	vibescore:vs
		// });

		$.get( "http://192.168.1.177/")
		.done(function(data) {
			console.log( data.temperature, data.motion, data.sound, vs );
			$.get( "controller.php", {
				temperature : data.temperature,
				motion : data.motion,
				sound : data.sound,
				vibescore : vs
			});
		})
		.fail(function() {
			console.log( "error" );
		})
	})
}

