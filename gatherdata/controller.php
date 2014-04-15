<?php
	$dbHost = "69.195.124.104";
	$dbPort = 3306;
	$dbUsername = "jaredaro_all";
	$dbPass = "winthropd2324";
	$dbName = "jaredaro_cs144";

	$mysqli = new mysqli("$dbHost", "$dbUsername", "$dbPass", "$dbName");

	// $dbh = mysql_connect ("$dbHost", "$dbUsername", "$dbPass") or die ('I cannot connect to the database.');
	// mysql_select_db ("$dbName");
	if($mysqli->connect_errno)
	{
		echo "Failed to connect to MySQL: (" . $mysqli->connect_errno . ") " . $mysqli->connect_error;
	}
	else
	{
		$temp = $_GET["temperature"];
		$motion = $_GET["motion"];
		$sound = $_GET["sound"];
		$vs = $_GET["vibescore"];

		$mysqli->query("INSERT INTO data (temperature, motion, sound, vibescore) VALUES ('$temp', '$motion', '$sound', '$vs')");
		
		// header('Location: /index.html');
	}
?>