<?php
function process_script() {
	
	$action=strtolower(getRequest('action',true,'get'));
	$accepted_actions='login,app,logout';

	// Get an array from the accepted actions (prevents 'login,signup' from being an accepted action)
	$accepted_actions = explode(',', $accepted_actions);
	
	// Make action login if it is not an accepted action
	if(!in_array($action, $accepted_actions, true)) { $action = 'login'; }

	return get_template($action, $action()); 
}



function set_header($action=null) {
	$url = (empty($action)) ? urlhost() : urlhost().'?action='.$action;
	header('Location: '. $url );
	exit();
}


function &app() {
	
	// Make sure there is a valid session
	validate_session();
	
	$HTML=array();
	$HTML['dob'] = get_SESSION('dob');
	$HTML['datepicker'] = date("m-d-Y") ;
	return $HTML;
}


function &logout() {
	// Make sure there is a valid session
	validate_session();
	
	// Destroy the session and return to login screen
	kill_session();
	set_header('login'); 
	exit();
}


function &login(){

	// Check if there is still a valid session
	validate_session(true);
	
	// Create the HTML array and initialize values
	$HTML=array();
	$HTML['email']='';
	$HTML['datepicker']='';	
	$HTML['login_error']='';

	// Check if it was a page load or a submission
	if (getRequest('submitted',true,'post') !== 'yes') {return $HTML;}
	
	// It was a page load so get the post values
	foreach($HTML as $key=> &$value) {
		$value=getRequest($key, true, 'post');
	}
	
	// Validate the email and password
	$date = explode('-', $HTML['datepicker']);
	if (empty($HTML['email'])) {
		$HTML['login_error'] = 'Email Cannot be empty';
	}
	elseif (empty($HTML['datepicker'])) { 
		$HTML['login_error'] ='Date of birth cannot be empty';
	}
	elseif (filter_var($HTML['email'],FILTER_VALIDATE_EMAIL) === false) {
		$HTML['login_error'] ='Invalid Email Address';
	}
	else{
		// The datepicker makes sure it is a valid date
		// But should still check incase someone scripts a request
		
		if(count($date) != 3 or !checkdate(intval($date[0]), intval($date[1]), intval($date[2]))){
			$HTML['login_error'] = "Invalid Date";
		}
	}
	
	// If no errors, set session variable and go to account
	if (empty($HTML['login_error'])) {
		// Create a database record
		if (empty($GLOBALS['DB'])) {die ('Database Link is not set'); }
		$query = sprintf('INSERT INTO project (email, dob, ipaddr) VALUES (\'%s\',\'%s\',\'%s\')', 
					mysql_real_escape_string($HTML['email']),
					mysql_real_escape_string($date[2] . '-' . $date[0] . '-' . $date[1]),
					mysql_real_escape_string(util_getenv('REMOTE_ADDR')));
		
		$result = mysql_query($query);
		
		// Make sure it executed properly
		if(!$result) {
			$HTML['login_error'] = 'Database Error. Please try again later';
			return $HTML;
		}
		
		set_SESSION('dob', $HTML['datepicker']);
		set_header('app');
		exit();
	}
	
	// There were errors so load the login page again with errors
	return $HTML;
}

function get_SESSION($key) {
	return ( !isset($_SESSION[$key]) ) ? NULL : decrypt($_SESSION[$key]);
}


function set_SESSION($key, $value='') {
	if (!empty($key)) {
		$_SESSION[$key]=encrypt($value);
		return true;
	}
	return false;
}

function util_getenv ($key) {
		return ( isset($_SERVER[$key])? $_SERVER[$key]:(isset($_ENV[$key])? $_ENV[$key]:getenv($key)) );
}


function host ($protocol=null) {
	$host = util_getenv('SERVER_NAME');
	if (empty($host)) {	$host = util_getenv('HTTP_HOST'); }
	return (!empty($protocol)) ? $protocol.'//'.$host  : 'http://'.$host;
}


function urlhost ($protocol=null) {
	return host($protocol).$_SERVER['SCRIPT_NAME'];
}


function get_template($file, &$HTML=null){
	
	foreach ($HTML as $key => &$value) {
		$value = utf8HTML($value);
	}
	
	$content='';
	ob_start();
	if (@include(TMPL_DIR . '/' .$file .'.tmpl.php')):
		$content=ob_get_contents();
	endif;
	ob_end_clean();
	return $content;
}

	
function getRequest($str='', $removespace=false, $method=null){
	if (empty($str)) {return '';}
  		switch ($method) {
			case 'get':
				$data=(isset($_GET[$str])) ? $_GET[$str] : '' ;
				break;
			case 'post':
				$data=(isset($_POST[$str])) ? $_POST[$str] : '';
				break;
				
			default:
				$data=(isset($_REQUEST[$str])) ? $_REQUEST[$str] : '';
		}
 		
		if (empty($data)) {return $data;}
		
		
		if (get_magic_quotes_gpc()) {
			$data= (is_array($data)) ? array_map('stripslashes',$data) : stripslashes($data);	
		}

		if (!empty($removespace)) {
			$data=(is_array($data)) ? array_map('removeSpacing',$data) : removeSpacing($data);
		}

		return $data;
	}


	
function removeSpacing($str) {
		return trim(preg_replace('/\s\s+/', ' ', $str));
}
	
	
function utf8HTML ($str='') {
  	   	return htmlentities($str, ENT_QUOTES, 'UTF-8', false); 
}
	

function encrypt($text, $SALT=SECURE_KEY) {
	return base64_encode(mcrypt_encrypt(MCRYPT_BLOWFISH, md5($SALT), $text, MCRYPT_MODE_ECB));
} 

    
function decrypt($text, $SALT=SECURE_KEY) { 
	return mcrypt_decrypt(MCRYPT_BLOWFISH, md5($SALT), base64_decode($text), MCRYPT_MODE_ECB);
}  

function validate_session($exists = false) {
	if($exists) {
		if(isset($_SESSION['dob'])) {
			set_header('app');
			exit();
		}
	} else {
		if(!isset($_SESSION['dob'])) {
			set_header('login');
			exit();
		}
	}
}

function kill_session() {
	// Destroy the cookie
	if (isset($_COOKIE[session_name()])) {
		setcookie(session_name(),'',time()-42000,'/');
	}
	
	// Destroy the session
	session_destroy();
}

?>
