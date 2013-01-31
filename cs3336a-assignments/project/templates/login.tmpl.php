<?php if (!defined('TMPL_DIR')) return; ?>

<script type="text/javascript">
	//<![CDATA[
	
	$(document).ready(function() {

		// Create the datepicker
		$("#datepicker").datepicker({
			changeMonth: true,
			changeYear: true,
			dateFormat: "mm-dd-yy",
			maxDate: "-1w",
			yearRange: "c-200:2012"
			
		}).attr('readOnly', true);
		
	});
	//]]>
</script>

</head>

<body>

<div id="loginbody">
	<form id="inputArea" action="index.php?action=login" method="post">

			<label for="email">Email</label>
			<input type="text" name="email" id="email" value="<?php echo $HTML['email'];?>" />

			<label for="datepicker">Date of Birth (mm-dd-yyyy)</label>
			<input type="text" name="datepicker" id="datepicker" value="<?php echo $HTML['datepicker'];?>"/>

			<span><?php echo $HTML['login_error'];?></span>

		<input class="submit" type="submit" value="Login" />
			
		<input type="hidden" name="submitted" value="yes" />
	</form>
</div>

</body>
</html>
