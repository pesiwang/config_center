<?php
date_default_timezone_set('Asia/Shanghai');
if ($argc != 4) {
	die('usage ' . $argv[0] . ' <host> <port> <version>' . "\n");
}

$host = $argv[1];
$port = $argv[2];
$version = $argv[3];

require_once __DIR__ . '/../../lib/Smarty/Smarty.class.php';
$smarty = new Smarty();
$smarty->compile_dir = '/tmp';
$smarty->left_delimiter = '<%';
$smarty->right_delimiter = '%>';
$smarty->caching = false;

$smarty->assign('host', $argv[1]);
$smarty->assign('port', $argv[2]);
$smarty->assign('version', $argv[3]);
$content = $smarty->fetch(__DIR__ . '/templates/CCDefinition_h.tpl');
echo $content;
