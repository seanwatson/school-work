<?php

// Code based off Dave Child's blog at ILoveJackDaniel's (ilovejackdaniels.com)
// http://www.linuxjournal.com/article/9585
//
// This will validate strings against the RFC 3696 spec
// It follows the spec with the exception of rejecting strings
// containing '\@'
function valid_email(&$email) {

	// First, we check that there's one @ symbol, 
	// and that the lengths are right.
	if (!preg_match("/^[^@]{1,64}@[^@]{1,255}$/", $email)) {
		
		// Email invalid because wrong number of characters 
		// in one section or wrong number of @ symbols.
		return false;
	}
	
	// Split it into sections to make life easier
	$email_array = explode("@", $email);
	$local_array = explode(".", $email_array[0]);
	
	// Validate the local part of the address
	for ($i = 0; $i < sizeof($local_array); ++$i) {
		if (!preg_match('/^(([A-Za-z0-9\!\#\$%&\'\*\+\/\=\?\^_`\{\|\}~\-][A-Za-z0-9\!\#\$%&\'\*\+\/\=\?\^_`\{\|\}~\.\-]{0,63})|(\"[^(\\|\")]{0,62}\"))$/', $local_array[$i])) {
			return false;
		}
	}
	
	// Check if domain is IP. If not, it should be valid domain name
	if (!preg_match("/^\[?[0-9\.]+\]?$/", $email_array[1])) {
		$domain_array = explode(".", $email_array[1]);
		
		// Check that there is at least 2 domain parts
		if (sizeof($domain_array) < 2) {
			return false;
		}
		
		// Validate each part of the domain
		for ($i = 0; $i < sizeof($domain_array); $i++) {
			if (!preg_match("/^(([A-Za-z0-9][A-Za-z0-9-]{0,61}[A-Za-z0-9])|([A-Za-z0-9]+))$/", $domain_array[$i])) {
				return false;
			}
		}
	}
	
	// Passed all validations
	return true;
}

function create_faculty_file() {

	$fac_file = get_file();									// Get the html file
	
	if (empty($fac_file)) {
		return false;
	}

	preg_match('/<tbody(.+)tbody>/sm', $fac_file, $table); 	// Get the table
	$rows = preg_split('/<tr/', $table[0]); 				// Get each row
	array_shift($rows);										// Get rid of the first element
	
	$fp = fopen(FACULTY_FILE, 'w');							// Open the file for writing
	if (empty($fp)) {
		return false;
	}
	
	if(@flock($fp, LOCK_EX)) {								// Prevent multiple creations
		foreach($rows as $row) {							// Iterate through the rows
			
			$cols = preg_split('/<td/', $row);	// Split up the columns
			
			$name = filter_name($cols[1]);		// Get the name
			$area = filter_area($cols[2]);		// Get the area of research
			$mail = filter_email($cols[3]);		// Get the email
			
			fwrite($fp, "{$mail},{$name},{$area}\n");// Write the output to the file
		}
		flock($fp, LOCK_UN);
	} else {
		fclose($fp);						// Couldn't get lock
		return false;
	}
	fclose($fp);							// Close the file
	return true;
}

function get_file() {
	if (!function_exists('curl_init')) {
		return false;
	}
	$ch = @curl_init(FACULTY_WEB);	// Create the curl

	// Set options
	@curl_setopt($ch, CURLOPT_HEADER, false);
	@curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
	@curl_setopt($ch, CURLOPT_USERAGENT, 'Mozilla/5.0 (X11; Linux i686) AppleWebKit/537.4 (KHTML, like Gecko) Chrome/22.0.1229.94 Safari/537.4');
	
	$file = @curl_exec($ch);		// Execute the request
	$statuscode = curl_getinfo($ch, CURLINFO_HTTP_CODE);
	@curl_close($ch);			// Free resources

	return ($statuscode == 200) ? $file : false;				// Return the result of the request
}

function filter_name(&$str){
	preg_match('/http.+</U', $str, $name);				// Extract around the name
	
	$name[0] = preg_replace('/.+>|</', '', $name[0]);	// Get just the name
	$name[0] = preg_replace('/"/', '\"', $name[0]);		// Escape quotes
	$name[0] = trim($name[0]);							// Remove trailing whitespace
	
	$name = preg_split('/,? Dr\. /', $name[0]);			// Split at Dr.
	
	return "\"Dr. {$name[1]} {$name[0]}\"";				// Piece it back together
}

function filter_area(&$str) {
	preg_match('/>.+</U', $str, $area);					// Extract around the area
	
	$area[0] = preg_replace('/>|</', '', $area[0]);		// Get just the area
	$area[0] = trim($area[0]);							// Remove trailing whitespace
	return "\"{$area[0]}\"";							// Return the research area
}

function filter_email(&$str) {
	preg_match('/>.+\*/', $str, $email);				// Extract around the email
	
	$email[0] = preg_replace('/>/', '', $email[0]);			// Get rid of <
	$email[0] = preg_replace('/\*/', '@uwo.ca', $email[0]); // Insert the email portion
	
	return "\"{$email[0]}\"";							// Return the email
}

function find_email(&$email) {
	
	$fp = fopen('faculty.csv', 'r');			// Open the file
	if (empty($fp)) {
		return null;
	}
	
	if (@flock($fp, LOCK_EX)) {					// Prevent reading while writing
		while (!feof($fp)) {					// Read lines until end of file
			$person = fgetcsv($fp);
			if ($person[0] == $email) {			// Check if the line matches the one looking for
				fclose($fp);					// Close the file
				flock($fp, LOCK_UN);
				// Return the result
				return json_encode(array('name' => $person[1], 'area' => $person[2], 'email' => $person[0]));
			}
		}
		flock($fp, LOCK_UN);					// Release the lock
	}
	fclose($fp);	// Close the file
	return null;	// Return null for not found
}

?>
