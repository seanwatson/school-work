<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" dir="ltr" xml:lang="en-US" lang="en-US">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
<title>Sean Watson | CS3336 HW3</title>

<link rel="stylesheet" href="./css/hw3.css" type="text/css" media="screen" />
<script src="//ajax.googleapis.com/ajax/libs/jquery/1.8.2/jquery.min.js" type="text/javascript"></script>

<script type="text/javascript">
	//<![CDATA[
	$(document).ready(function() {

	var $loading = $("#load");
	var $results = $("#results");
	var $error = $("#error");
	var $query = $("#email input");
	var jqxhr = null;
		
	$("#button").click(function(){
		$results.hide();
		$error.hide();
		$loading.show();
		
		var email = $query.val().replace(/\s/g, '');
		
		if(jqxhr){
			jqxhr.abort();
		}
		
		jqxhr = $.getJSON("./process.php",{query:email},function(data){
				jqxhr = null;
				$loading.hide();
				if(data.error){
					$error.text(data.error).show();
				}
				else{
					$("#name").html(data.name.replace(/\\/g, ''));
					$("#area").html(data.area);
					$("#addr").html(data.email);	
					$results.show();
				}
		});
		
	});
	
	$query.focus(function(){
		$results.hide();
		$error.hide();
	});

	});
	//]]>
</script>

</head>

<body>
<div id="mainbody">

	<p id="email">Email: <input type="text" name="mail" value=""/></p>
	<a id="button" href="#">Find</a>

	<img id="load" src="./images/loading.gif" alt="" width="227" height="92"/>

	<p id="error"></p>

	<div id="results">
		
		<div id="labels">
			<p>Name:</p>
			<p>Area:</p>
			<p>Email:</p>
		</div>
		
		<div id="values">
			<p id="name"></p>
			<p id="area"></p>
			<p id="addr"></p>
		</div>
		
	</div>

</div>
</body>
</html>
