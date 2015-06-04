<?php
date_default_timezone_set('Asia/Shanghai');
if ($argc != 2) {
	die('usage ' . $argv[0] . ' <cc.json>' . "\n");
}

$jsonFile = $argv[1];

function traverse($node, $prefix) {
	global $nodesMap;
	if (is_a($node, 'CCInteger')) {
	}
	else if (is_a($node, 'CCBool')) {
	}
	else if (is_a($node, 'CCString')) {
	}
	else if (is_a($node, 'CCObject')) {
		$nodesMap[$prefix] = $node;
		foreach ($node->value as $k => $v) {
			traverse($v, $prefix . '_' . ucfirst($k));
		}
	}
	else if (is_a($node, 'CCArray')) {
		traverse($node->value, $prefix);
	}
}

function cc_item_2_objc_classname($item) {
	if (is_a($item, 'CCObject')) {
		return 'CCNode' . ucfirst(preg_replace('/\\.([a-z])/e', "'_' . strtoupper('\\1')", $item->getClassName()));
	}
	else if (is_a($item, 'CCArray')) {
		return 'NSArray<' . cc_item_2_objc_classname($item->value) . '>';
	}
	return $item->getClassName();
}


require_once __DIR__ . '/../../lib/cc_reader.class.php';
$root = CCReader::parse($jsonFile);
$nodesMap = array();

traverse($root, 'CCNode');

require_once __DIR__ . '/../../lib/Smarty/Smarty.class.php';
$smarty = new Smarty();
$smarty->compile_dir = '/tmp';
$smarty->left_delimiter = '<%';
$smarty->right_delimiter = '%>';
$smarty->caching = false;

$smarty->assign('nodesMap', $nodesMap);
$content = $smarty->fetch(__DIR__ . '/templates/CCItem_m.tpl');
echo $content;
