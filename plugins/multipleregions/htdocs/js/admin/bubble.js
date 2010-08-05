$(document).ready(function() {
	var fullBrief = $("div.fullBrief");

	var aTD = fullBrief.parent()
	aTD.css('cursor','pointer')

	aTD.bind("mouseenter",function(){
		$(this).children('div.fullBrief').show()
	})

	aTD.bind("mouseleave",function(){
		$(this).children('div.fullBrief').hide()
	})
})
