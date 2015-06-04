<?php
require_once __DIR__ . '/cc_reader.class.php';

if ($argc != 3) {
	die("usage: $argv[0] <shm> <keys>\n");
}

$reader = new CCReader($argv[1]);
$key = $argv[2];
var_dump($reader->get($argv[2]));
