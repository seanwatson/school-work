<?php if (!defined('TMPL_DIR')) return; ?>

<script type="text/javascript">
	//<![CDATA[
	
	$(document).ready(function() {

		// Create the datepicker
		$("#datepicker").datepicker({
			showOn: "button",
			buttonImage: "./images/calendar.gif",
			buttonImageOnly: true,
			changeMonth: true,
			changeYear: true,
			dateFormat: "mm-dd-yy",
			yearRange: "c-200:c+200"
			
		});
		
		// Change the color of arrows when hovered over
		$("#leftarrow, #rightarrow").hover(
		function() {
			$(this).children(":first").hide().next().show();
		},
		function() {
			$(this).children(":first").show().next().hide();
		});
		
		var jqxhr = null;
		
		// Called to update the graphic
		function updateBiorhythm(off) {
			
			var birthday = $("#dob").text();
			var birthdayDate = parseDate(birthday);
			var offsetDate = parseDate($("#datepicker").val());
			offsetDate.setDate(offsetDate.getDate() + off);
			
			var num_days = Math.ceil(((offsetDate - birthdayDate)/(1000*60*60*24)));
			
			$("#datepicker").datepicker('setDate', offsetDate);
			
			if(num_days < 0){
				$("#error").text("Error: Date can not be before birthday").show();
				$("#graph").children(":first").hide();
			}
			else {
				
				if(jqxhr){
					jqxhr.abort();
				}
				
				$("#loading").show();
				
				jqxhr = $.post("./process.php",
						{dob: birthday, offset: num_days},
						function(data) {
							$("#loading").hide();
							if(data.error){
								$("#error").text(data.error).show();
								$("#graph").children(":first").hide();
							}
							else {
								$("#graph").children(":first").attr('src', data.file).show();
								$("#error").hide();
							}
							
						},
						"json"
				);
			}
			
			
		}
		
		function parseDate(str){
			var parts = str.split('-');
			return new Date(parts[2], parts[0]-1, parts[1]);
		}
		
		$("#leftarrow").click( function() {
			updateBiorhythm(-10);
		});
		
		$("#rightarrow").click( function() {
			updateBiorhythm(10);
		});
		
		$("#datepicker").datepicker('setDate', new Date()).bind('change', function() {
			updateBiorhythm(0);
		}).change();
	});
	//]]>
</script>

</head>

<body>
	
<div id="appbody">
	<div id="top">
		<a href="./index.php?action=logout">Logout</a>
	</div>

	<div id="leftside">
		<p>Day of Birth: <span id='dob'><?php echo $HTML['dob'];?></span></p>
		<label for="datepicker">Select a date: <br/></label>
		<input type="text" name="datepicker" id="datepicker" value=""/>
	</div>

	<div id="leftarrow">
		<img src="./images/arrow.png" alt="" width="49" height="151"/>
		<img class="hoverimage" src="./images/redarrow.png" alt="" width="49" height="151"/>
	</div>

	<div id="graph">
		<img src="" alt="" width="700" height="408"/>
		<img id="loading" src="./images/loading.gif" alt="" width="227" height="92"/>
		<p id="error"></p>
	</div>

	<div id="rightarrow">
		<img src="./images/arrow.png" alt="" width="49" height="151"/>
		<img class="hoverimage" src="./images/redarrow.png" alt="" width="49" height="151"/>
	</div>
</div>

</body>
</html>
