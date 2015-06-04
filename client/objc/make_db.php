<?php
if ($argc != 3) {
	die('usage ' . $argv[0] . ' <cc.json> <db_file>' . "\n");
}

$jsonFile = $argv[1];
$dbFile = $argv[2];

if (!is_readable($jsonFile)) {
	die("cannot read ${jsonFile}\n");
}

if (file_exists($dbFile)) {
	if (!unlink($dbFile)) {
		die("DB ${dbFile} exists and we cannot overwrite it\n");
	}
}

$content = gzencode(file_get_contents($jsonFile));
$hash = pack("H*", md5($content));

$db = new SQLite3($dbFile);
$db->exec("CREATE TABLE config (`hash` CHAR(16) NOT NULL DEFAULT '' PRIMARY KEY, `content` MEDIUMBLOB)");
$stmt = $db->prepare('REPLACE INTO config (`hash`, `content`) VALUES(:hash, :content)');
$stmt->bindValue(':hash', $hash, SQLITE3_TEXT);
$stmt->bindValue(':content', $content, SQLITE3_BLOB);
$stmt->execute();
