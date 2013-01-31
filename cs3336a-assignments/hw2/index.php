<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" dir="ltr" xml:lang="en-US" lang="en-US">
<head>
	<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
	<title>Sean Watson | CS3336 HW2</title>

	<link rel="stylesheet" href="./themes/smoothness/jquery-ui-1.9.0.custom.min.css" type="text/css" media="screen" />
	<link rel="stylesheet" href="./css/hw2.css" type="text/css" media="screen" />
		
	<script src="//ajax.googleapis.com/ajax/libs/jquery/1.8.2/jquery.min.js" type="text/javascript"></script>
	<script src="./js/jQuery/jquery-ui-1.9.0.custom.min.js" type="text/javascript"></script>
	
	<script type="text/javascript">
		//<![CDATA[
		
		$(document).ready(function() {
			
			function updateLucky() {
				
				// Get the datepicker value
				var $datestring = $("#datepicker").val();
				var $checkstring = "";
				
				// Get the checked boxes into a string
				var $checked_boxes = $("#leftside input:checked");
				for (var i = 0; i < $checked_boxes.length; ++i) {
					$checkstring += $($checked_boxes[i]).attr("value");
				}				

				// Do the get request and change the answer text
				$.get("./process.php",
					{date: $datestring, check: $checkstring},
					function(data) {
						$("#answer").text(data);
					}
				);		
			}
			
			// Cache the datepicker element for efficiency
			var $datepicker = $("#datepicker");
			
			// Create the datepicker
			$datepicker.datepicker({
				showOn: "button",
				buttonImage: "./images/calendar.gif",
				buttonImageOnly: true,
				buttonText: "Please select your date",
				dateFormat: "mm-dd-yy"
			});
			
			// Create the tooltips
			$(function() {
				$datepicker.next("img").tooltip();
				$("#info img").tooltip();
			});
			
			// Set the datepicker date, bind the event, and trigger the event to calculate the number
			$datepicker.datepicker('setDate', new Date()).bind('keyup change', updateLucky).keyup();

			// Bind the checkboxes
			$("#leftside input").bind('change', updateLucky);
			
		});
		//]]>
	</script>
</head>

<body>
	
	<div id="mainbody">
		
		<div id="leftside">
			<p>Pick Numbers</p>
			<?php
				for($num = 1; $num < 10; ++$num) {
					echo "<input type=\"checkbox\" name=\"boxes\" value=\"{$num}\"/>{$num}<br/>\n";
				}
			?>
		</div>
		
		<div id="rightside">
			<p>Pick a Date:</p>
			<input type="text" id="datepicker" />

			<p id="answer">Your lucky number is </p>

			<p id="info">This script calculates your lucky number <img src="./images/info.jpg" alt="" width="14" height="14" title="Based on single-digit summation"/></p>
		</div>
		
	</div>
	
</body>
</html>
