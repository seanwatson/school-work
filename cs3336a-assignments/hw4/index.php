<?php 
require_once('./webroot.conf.php');

$page=process_script();

?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" dir="ltr" xml:lang="en-US" lang="en-US">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
<title>Sean Watson | CS3336 HW4</title>

<link rel="stylesheet" href="./css/hw4.css" type="text/css" media="screen" />

</head>

<body>
<div id="mainBody">


<?php echo $page; ?>

 
</div>

</body>
</html>
