<?php
function process_script() {
	
	$action=strtolower(getRequest('action',true,'get'));
	$accepted_actions='login,signup,account,edit,logout,cancel';

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


function &account() {
	
	// Make sure there is a valid session
	validate_session();
	
	$HTML=array();
	return $HTML;
}


function &cancel() {
	// Make sure there is a valid session
	validate_session();
	
	if (empty($GLOBALS['DB'])) {die ('Database Link is not set'); }
	
	$query = sprintf('DELETE FROM users WHERE userID=\'%s\'',
						mysql_real_escape_string(get_SESSION('userID')));
	
	// Delete the user
	$result = mysql_query($query);
	if(!$result) { die ('Could not delete user'); }
	
	kill_session();
	
	set_header('login'); 
	exit();
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
	$HTML['password']='';	
	$HTML['login_error']='';

	// Check if it was a page load or a submission
	if (getRequest('submitted',true,'post') !== 'yes') {return $HTML;}
	
	// It was a page load so get the post values
	foreach($HTML as $key=> &$value) {
		$value=getRequest($key, true, 'post');
	}
	
	// Validate the email and password
	$userID=0;
	if (empty($HTML['email'])) {
		$HTML['login_error'] = 'Email Cannot be empty';
	}
	elseif (empty($HTML['password'])) { 
		$HTML['login_error'] ='Password cannot be empty';
	}
	elseif (filter_var($HTML['email'],FILTER_VALIDATE_EMAIL) === false) {
		$HTML['login_error'] ='Invalid Email Address';
	}
	else {
		$userID=validate_record($HTML['email'],$HTML['password']);
		if ($userID < 1) {
			$HTML['login_error'] ='Account not found or invalid Email/Password';
		}
	}
	
	// If no errors, set session variable and go to account
	if (empty($HTML['login_error'])) {
		set_SESSION('userID', $userID);
		set_header('account');
		exit();
	}
	
	// There were errors so load the login page again with errors
	$HTML['password']='';
	return $HTML;
}

function &edit() {
	return signup(true);
} 


function &signup($edit=false){
	
	// Check if there is still a valid session
	validate_session(!$edit);
	
	$valid_input = true;
	
	// Create the HTML array with empty values
	$HTML=array();
	
	$HTML['password'] = '';
	$HTML['confirm_password'] = '';
	$HTML['email'] = '';
	$HTML['city'] = '';
	$HTML['countryID'] = '';
	$HTML['password_encrypted'] = '';

		
	// Check the submitted field
	if (getRequest('submitted',true,'post') !== 'yes') {
				
		// Fill in the fields if it is an edit
		if($edit) {
			$fields = getUserInfo(get_SESSION('userID'));
			$HTML['email'] = $fields['email'];
			$HTML['city'] = $fields['city'];
			$HTML['countryID'] = $fields['countryID'];
			$HTML['password_encrypted'] = $fields['password_encrypted'];
		}
		
		// Populate the select drop down box
		getCountries($HTML['country_options_escape'], $HTML['countryID']);
		
		return $HTML;
	}
	
	// Fill the array with the form data
	foreach($HTML as $key=> &$value) {
		$value=getRequest($key, true, 'post');
	}
	
	// Validate the email
	if(empty($HTML['email'])) {
		$HTML['email_error'] = 'Email cannot be empty';
		$valid_input = false;
	} else if(filter_var($HTML['email'],FILTER_VALIDATE_EMAIL) === false) {
		$HTML['email_error'] = 'Invalid Email Address';
		$valid_input = false;
	} else {
		$newID = validate_record($HTML['email']);
		if($newID > 0 and (!$edit or ($newID !== trim(get_SESSION('userID'))))) {
			$HTML['email_error'] = 'An account already exists with that email';
			$valid_input = false;
		}		
	}
	
	// Validate the password
	$saved_pass = false;
	if (($edit and empty($HTML['password'])) or ($HTML['password'] === PASS_HOLDER and !empty($HTML['password_encrypted']))) {
		// use the password that was saved before
		$saved_pass = true;
	} else if(empty($HTML['password'])) {
		$HTML['confirm_password_error'] = 'Password cannot be empty';
		$valid_input = false;
	} else if($HTML['password'] !==  $HTML['confirm_password']) {
		$HTML['confirm_password_error'] = 'Passwords do not match';
		$valid_input = false;
	} else if(!preg_match('/(?=.*\d)(?=.*[a‐z])(?=.*[A-Z])(?=.*[\.\+\\\*\?\[\^\]\$\(\)\{\}\=\!\<\>\|\:\',"~`&@_;\/#%-]).{6,20}/', $HTML['password'])) {
		$HTML['confirm_password_error'] = 'Password must be 6-20 chars and more secure!';
		$valid_input = false;
	}
	
	// Validate the city
	if(empty($HTML['city'])) {
		$HTML['city_error'] = 'City cannot be empty';
		$valid_input = false;
	} else if(preg_match('/(?=.*[\d\.\+\\\*\?\[\^\]\$\(\)\{\}\=\!\<\>\|\:\'"~`&@_;\/#%])/', $HTML['city'])) {
		$HTML['city_error'] = 'Special characters are not allowed';
		$valid_input = false;
	}
	
	// Validate the country
	if(empty($HTML['countryID'])){
		$HTML['countryID_error'] = 'Please select your country';
		$valid_input = false;
	} else if(!validate_countryID($HTML['countryID'])){
		$HTML['countryID_error'] = 'Invalid country submitted';
		$valid_input = false;
	}
		
	if ($valid_input) {
		
		// If it is the edit page update the user and return to account page
		if ($edit) {
			updateUser($HTML, $saved_pass);
			set_header('account');
			exit();
		}
		
		if (empty($GLOBALS['DB'])) {die ('Database Link is not set'); }
		// Create a user
		$query = sprintf('INSERT INTO users (email, password, city, countryID) VALUES (\'%s\',\'%s\',\'%s\',\'%s\')', 
					mysql_real_escape_string($HTML['email']),
					mysql_real_escape_string($saved_pass ? $HTML['password_encrypted'] : md5($HTML['password'])),
					mysql_real_escape_string($HTML['city']),
					mysql_real_escape_string($HTML['countryID']));
					
		$result = mysql_query($query);
		
		// Make sure it executed properly
		if(!$result) {
			$HTML['signup_error'] = 'Error adding user';
			return $HTML;
		}
		
		set_SESSION('userID', validate_record($HTML['email']));
		set_header('account'); 
		exit();
	}
	
	// Populate the select drop down box since we have to go back to the page
	getCountries($HTML['country_options_escape'], $HTML['countryID']);
	
	// Store the password if it was valid and changed
	if (empty($HTML['confirm_password_error'])) {
		if (!empty($HTML['password']) and $HTML['password'] !== PASS_HOLDER) {
			$HTML['password_encrypted'] = md5($HTML['password']);
			$HTML['password'] = PASS_HOLDER;
			$HTML['confirm_password'] = PASS_HOLDER;
		}
	// Clear the password if it was invalid or set it back to do not change if it is an edit page
	} else {
		if(!$edit) {
			$HTML['password_encrypted'] = '';
		}
		$HTML['password'] = '';
		$HTML['confirm_password'] = '';
	}
	return $HTML;
}


function validate_record($email, $password=NULL, $userID=null) {
	if (empty($GLOBALS['DB'])) {die ('Database Link is not set'); }
	
	// Build the query
	$query = 'SELECT userID FROM users WHERE email=\'' . mysql_real_escape_string($email) . '\'';
	if(!empty($password)) $query .= ' AND password=\'' . mysql_real_escape_string(md5($password)) . '\'';
	if(!empty($userID)) $query .= ' AND userID=\'' . mysql_real_escape_string($userID) . '\'';
	
	// Execute the query
	$result = mysql_query($query);
	
	$id = 0;
	// Check if the record was found
	if(mysql_num_rows($result) > 0 ) {
		$id = mysql_fetch_array($result);
		$id = $id[0];
	}
	
	// Free the resource
	mysql_free_result($result);
	
	// Return the ID
	return $id;
	
}

// Validates countryID incase user scripts a post request
function validate_countryID($id) {
	if(strlen($id) !== 2) return false;
	if (empty($GLOBALS['DB'])) {die ('Database Link is not set'); }
	
	// Generate the query
	$query = sprintf('SELECT countryID FROM countries WHERE countryID=\'%s\' and active=\'YES\'',
						mysql_real_escape_string($id));
	
	// Query for the country code
	$result = mysql_query($query);
	
	$found = false;
	// Check if the record was found
	if(mysql_num_rows($result) > 0 ) {
		$found = true;
	}
	
	// Free the resource
	mysql_free_result($result);
	return $found;
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
		if($key != 'country_options_escape') {
			$value = utf8HTML($value);
		}
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

function getCountries(&$options, &$selected = '') {
	if (empty($GLOBALS['DB'])) {die ('Database Link is not set'); }
	
	// Create the first option as an empty string
	$options = '<option value="">Please Select</option>' . "\n";	
	
	// Query database for all countries with YES
	$result = mysql_query('SELECT countryID, country FROM countries WHERE active=\'YES\'');
	
	// Add all countries as an option
	while($row = mysql_fetch_array($result, MYSQL_NUM)) {
		if($row[0] === $selected)  {
			$options .= "<option value=\"{$row[0]}\" selected='selected' >{$row[1]}</option>\n";
		} else {
			$options .= "<option value=\"{$row[0]}\" >{$row[1]}</option>\n";
		}
	}

	// Free the resource
	mysql_free_result($result);

	return;
}

function getUserInfo($id) {
	if(empty($id)) return false;
	if (empty($GLOBALS['DB'])) {die ('Database Link is not set'); }
	
	// Generate the query
	$query = sprintf('SELECT email, city, countryID, password FROM users WHERE userID=\'%s\'',
						mysql_real_escape_string($id));
	
	// Execute the query
	$result = mysql_query($query);
	
	// Check if the user was found
	if(mysql_num_rows($result) === 0 ) {
		// Free the resource
		mysql_free_result($result);
		return false;
	}
	
	// Get the users info
	$info = mysql_fetch_assoc($result, MYSQL_NUM);
	
	// Free the resource and return the info
	mysql_free_result($result);

	return array('email' => $info[0], 'city' => $info[1], 'countryID' => $info[2], 'password_encrypted' => $info[3]);
}

function updateUser(&$info, $use_enc_pass) {
	if (empty($GLOBALS['DB'])) {die ('Database Link is not set'); }
	
	// Build the query string
	$query = sprintf('UPDATE users SET email=\'%s\',  city=\'%s\', countryID=\'%s\', password=\'%s\' WHERE userID=\'%s\'',
				mysql_real_escape_string($info['email']),
				mysql_real_escape_string($info['city']),
				mysql_real_escape_string($info['countryID']),
				mysql_real_escape_string($use_enc_pass ? $info['password_encrypted'] : md5($info['password'])),
				mysql_real_escape_string(get_SESSION('userID')));
	
	// Exceute the query
	$result = mysql_query($query);
	
	// Check that it worked
	if(!$result) { die('Could not update user' . $query . mysql_error()); }
	
	return;	
}

function validate_session($exists = false) {
	if($exists) {
		if(isset($_SESSION['userID'])) {
			set_header('account');
			exit();
		}
	} else {
		if(!isset($_SESSION['userID'])) {
			set_header('login');
			exit();
		}
	}
}

function kill_session() {
	// Destroy the cookie
	//if (isset($_COOKIE[session_name()])) {
	//	setcookie(session_name(),'',time()-42000,'/');
	//}
	
	// Destroy the session
	session_destroy();
}

?>
