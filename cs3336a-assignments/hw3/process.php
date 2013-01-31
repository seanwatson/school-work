<?php
require_once('./includes/config.inc.php');
require_once('./includes/functions.inc.php');

sleep(2); // Don't touch this!

$email = $_GET['query'];

// Check if it is a valid email
if (!valid_email($email)) {
	// Return an error if not valid
	echo json_encode(array('error' => 'Invalid Email address!'));
} else {
	// Check that the file exists and is not empty
	if (!file_exists(FACULTY_FILE) || filesize(FACULTY_FILE) == 0 ) {	
		// If not, create the file
		if (!create_faculty_file()) {
			echo json_encode(array('error' => 'Server error. Please try again.'));
			return;
		}
	}
	
	$result = find_email($email);	// Search for the email
	
	// Check if a result was found
	if (isset($result)) {
		// Return the result 
		echo $result;
	} else {
		// Return an error not found
		echo json_encode(array('error' => 'Email not found!'));
	}
}

?>
