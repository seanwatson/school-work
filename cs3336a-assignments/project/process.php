<?php

function generateLabels(&$dob, &$off) {
	$parts = explode('-', $dob);
	$date = @date_create("{$parts[2]}-{$parts[0]}-{$parts[1]}");
	if (!$date) {
		$e = date_get_last_errors();
		foreach ($e['errors'] as $error) {
			echo "$error\n";
		}
		exit(1);
	}
	date_add($date, date_interval_create_from_date_string($off .' days'));
	date_add($date, date_interval_create_from_date_string('-10 days'));
	$labels = '0:'; 
	for($i = 0; $i <= 10; ++$i){
		$labels .= '|' . date_format($date, 'M d');
		date_add($date, date_interval_create_from_date_string('2 days'));
	}
	
	return $labels;
}

// validate the date

$filename = './images/graphs/' . $_POST['dob'] . '-' . $_POST['offset'] . '.png';

// look if the file exists
if(!file_exists($filename)) {
	
	if (!function_exists('curl_init')) {
		echo json_encode(array('error' => 'Error: Google charts unavailable'));
		return;
	}
	$offset = intval($_POST['offset']);
	$min = $offset - 10;
	$max = $offset + 10;
	
	$physical = round(sin(2*pi()*$offset/23) * 100);
	$emotional = round(sin(2*pi()*$offset/28) * 100);
	$intellectual = round(sin(2*pi()*$offset/33) * 100);
	
	$fields = array(
			'cht' => urlencode('lc'),
			'chs' => urlencode('700x408'),
            'chd' => urlencode('t:-1|-1|-1'),
            'chds' => urlencode('-100,100'),
            'chxt' => urlencode('x,y'),
            'chdl' => urlencode("Physical: {$physical}|Emotional: {$emotional}|Intellectual: {$intellectual}"),
            'chco' => urlencode('FF0000,00FF00,0000FF'),
            'chfd' => urlencode("0,x,{$min},{$max},0.1,sin(2*3.14159*x/23)*100|1,x,{$min},{$max},0.1,sin(2*3.14159*x/28)*100|2,x,{$min},{$max},0.1,sin(2*3.14159*x/33)*100"),
            'chg' => urlencode('5,10,2,4'),
            'chxr' => urlencode('0,-10,10,2|1,-100,100,20'),
            'chma' => urlencode('150,20,20,20|100,350'),
            'chdls' => urlencode(',14'),
            'chm' => urlencode('h,FFAA00,0,0.5,1|V,FFAA00,0,100,1'),
            'chxl' => urlencode(generateLabels($_POST['dob'], $_POST['offset']))
    );
    
    $fields_string = '';
	foreach($fields as $key=>$value) { $fields_string .= $key.'='.$value.'&'; }
	rtrim($fields_string, '&');
	
	$fp = fopen($filename, "w");

	$ch = @curl_init("https://chart.googleapis.com/chart");	// Create the curl

	// Set options
	@curl_setopt($ch, CURLOPT_HEADER, false);
	@curl_setopt($ch, CURLOPT_FILE, $fp);
	@curl_setopt($ch, CURLOPT_BINARYTRANSFER, true);
	@curl_setopt($ch,CURLOPT_POST, true);
	@curl_setopt($ch,CURLOPT_POSTFIELDS, $fields_string);
	@curl_setopt($ch, CURLOPT_USERAGENT, 'Mozilla/5.0 (X11; Linux i686) AppleWebKit/537.4 (KHTML, like Gecko) Chrome/22.0.1229.94 Safari/537.4');
	
	@curl_exec($ch);		// Execute the request
	$statuscode = curl_getinfo($ch, CURLINFO_HTTP_CODE);
	
	@curl_close($ch);			// Free resources
	fclose($fp);
	
	if($statuscode != 200) {
		echo json_encode(array('error' => 'Error: Google charts unavailable'));
		return;
	}
	
}

// return the name of the file

echo json_encode(array("file" => $filename));
?>
