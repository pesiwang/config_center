<?php
class CCItem {
	private $_name;

	public function __construct($data, $name) {
		$this->_name = $name;
	}

	public function getClassName() {
		return $this->_name;
	}
}

class CCInteger extends CCItem {
	public $value;

	public function __construct($data, $name) {
		parent::__construct($data, $name);
		$this->value = (int)$data;
	}

	public function getClassName() {
		return 'CCInteger';
	}
}

class CCBool extends CCItem {
	public $value;

	public function __construct($data, $name) {
		parent::__construct($data, $name);
		$this->value = (bool)$data;
	}

	public function getClassName() {
		return 'CCBool';
	}
}

class CCString extends CCItem {
	public $value;

	public function __construct($data, $name) {
		parent::__construct($data, $name);
		$this->value = (string)$data;
	}

	public function getClassName() {
		return 'CCString';
	}
}

class CCObject extends CCItem {
	public $value;

	public function __construct($data, $name) {
		parent::__construct($data, $name);
		$this->value = array();
		foreach ($data as $k => $v) {
			$this->value[$k] = CCReader::smartItem($v, $name . '.' . $k);
		}
	}
}

class CCArray extends CCItem {
	public $value;

	public function __construct($data, $name) {
		parent::__construct($data, $name);
		$this->value = CCReader::smartItem($data[0], $name);
	}

	public function getClassName() {
		return $this->value->getClassName();
	}
}

class CCReader {
	static public function parse($jsonFile) {
		$content = @file_get_contents($jsonFile);
		$json = @json_decode($content, true);
		if ($json === false) {
			throw new Exception('failed to parse json file [' . $jsonFile . ']');
		}

		$root = CCReader::smartItem($json);
		if (!is_a($root, 'CCObject')) {
			throw new Exception('root should be an object');
		}

		return $root;
	}

	static public function smartItem($data, $name = '') {
		if (is_bool($data)) {
			return new CCBool($data, $name);
		}
		else if (is_int($data)) {
			return new CCInteger($data, $name);
		}
		else if (is_string($data)) {
			return new CCString($data, $name);
		}
		else if (is_array($data)) {
			if (array_keys($data) !== range(0, count($data) - 1)) {
				return new CCObject($data, $name);
			}
			else {
				return new CCArray($data, $name);
			}
		}
		throw new Exception('unknown type encountered');
	}
}
