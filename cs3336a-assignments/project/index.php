<?php 
require_once('./webroot.conf.php');

$page=process_script();

?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" dir="ltr" xml:lang="en-US" lang="en-US">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
<title>Sean Watson | CS3336 Project</title>

<link rel="stylesheet" href="./themes/smoothness/jquery-ui-1.9.0.custom.min.css" type="text/css" media="screen" />
<link rel="stylesheet" href="./css/project.css" type="text/css" media="screen" />

<script src="//ajax.googleapis.com/ajax/libs/jquery/1.8.2/jquery.min.js" type="text/javascript"></script>
<script src="./js/jQuery/jquery-ui-1.9.0.custom.min.js" type="text/javascript"></script>

<?php echo $page; ?>
