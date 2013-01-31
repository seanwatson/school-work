<?php

// Calculates a lucky number as the sum of its single digits
function mycount ($str) {
	
	// If string is null or 1 the lucky number is done
	if (strlen($str) < 2) {
		return $str;
	} else {
		$sum = 0; // Start the sum at 0
		
		// Iterate through all the characters
		$length = strlen($str);
		for ($i = 0; $i < $length; ++$i) {
			
			// Check if they are a number
			if(is_numeric($str[$i])) {
				
				// Add the value to the sum
				$sum += intval($str[$i]);
			}
		}
		// Recursively calculate the lucky number
		return mycount(strval($sum));
	}
}

// Get the lucky number
$lucky = mycount($_GET['date'] . $_GET['check']);

// Get the output based on the lucky number value
$lucky = (empty($lucky)) ? 'Error: Invalid Number' : 'Your lucky number is ' . $lucky;

// Return the lucky number answer phrase
echo $lucky;
?>
