<?php
class CCReaderParsedKey {
	const TYPE_NAME = 0x01;
	const TYPE_INDEX = 0x02;

	public $type;
	public $name;
}

class CCReaderWalkResult {
	public $type;
	public $length;
	public $valueOffset;
}

class CCReader {
	private $_shmId;
	private $_root;

	public function __construct($shmKey) {
		$this->_shmId = shmop_open($shmKey, 'a', 0, 0);
		if ($this->_shmId === FALSE) {
			die("failed to attach to shared memory\n");
		}

		$data = shmop_read($this->_shmId, 0, 4);
		$data = unpack('Noffset', $data);
		$rootOffset = $data['offset'];

		$this->_root = new CCReaderWalkResult();
		$data = shmop_read($this->_shmId, 4 + $rootOffset, 5);
		$data = unpack('Ctype/Nlength', $data);

		$this->_root->valueOffset = $rootOffset + 5;
		$this->_root->type = chr($data['type']);
		$this->_root->length = $data['length'];
	}

	public function get($keys) {
		$result = new CCReaderWalkResult();
		$result->type = $this->_root->type;
		$result->length = $this->_root->length;
		$result->valueOffset = $this->_root->valueOffset;

		while($key = $this->_parse($keys)) {
			if ($this->_walk($key, $result) === FALSE) {
				return null;
			}
		}

		return $this->_dump($result);
	}

	private function _walk(CCReaderParsedKey $parsedKey, CCReaderWalkResult $result) {
		if ($result->type == 'O') {
			return $this->_walkNode($parsedKey, $result);
		}
		else if ($result->type == 'A') {
			return $this->_walkArray($parsedKey, $result);
		}
		return FALSE;
	}

	private function _walkNode(CCReaderParsedKey $parsedKey, CCReaderWalkResult $result) {
		if ($parsedKey->type != CCReaderParsedKey::TYPE_NAME) {
			return FALSE;
		}

		$currOffset = 0;
		for ($i = 0; $i < $result->length; ++$i) {
			$data = shmop_read($this->_shmId, 4 + $result->valueOffset + $currOffset, 4);
			$data = unpack('Nlength', $data);
			$length = $data['length'];
			$currOffset += 4;
			if ($length != strlen($parsedKey->key)) {
				$currOffset += $length + 4;
				continue;
			}

			$data = shmop_read($this->_shmId, 4 + $result->valueOffset + $currOffset, $length + 4);
			$currOffset += $length + 4;
			$data = unpack('a' . $length . 'key/Noffset', $data);

			if (strcmp($data['key'], $parsedKey->key) == 0) {
				$nodeOffset = $data['offset'];
				$data = shmop_read($this->_shmId, 4 + $nodeOffset, 5);
				$data = unpack('Ctype/Nlength', $data);
				$result->type = chr($data['type']);
				$result->length = $data['length'];
				$result->valueOffset = $nodeOffset + 5;
				return TRUE;
			}
		}
		return FALSE;
	}

	private function _walkArray(CCReaderParsedKey $parsedKey, CCReaderWalkResult $result) {
		if ($parsedKey->type != CCReaderParsedKey::TYPE_INDEX) {
			return FALSE;
		}
		if ($result->length <= $parsedKey->key) {
			return FALSE;
		}
		$data = shmop_read($this->_shmId, 4 + $result->valueOffset + 4 * $parsedKey->key, 4);
		$data = unpack('Noffset', $data);
		$nodeOffset = $data['offset'];
		$data = shmop_read($this->_shmId, 4 + $nodeOffset, 5);
		$data = unpack('Ctype/Nlength', $data);
		$result->type = chr($data['type']);
		$result->length = $data['length'];
		$result->valueOffset = $nodeOffset + 5;
		return TRUE;
	}

	private function _dump(CCReaderWalkResult $result) {
		if (($result->type == 'I') || ($result->type == 'S') || ($result->type == 'B')) {
			$data = shmop_read($this->_shmId, 4 + $result->valueOffset, $result->length);
			switch($result->type) {
				case 'I':
					$data = unpack('Nvalue', $data);
					$data['value'] = (int)($data['value']);
					break;
				case 'S':
					$data = unpack('a*value', $data);
					break;
				case 'B':
					$data = unpack('Cvalue', $data);
					break;
				default:
					return NULL;
					break;
			}
			return $data['value'];
		}
		else if ($result->type == 'O') {
			return $this->_dumpNode($result);
		}
		else if ($result->type == 'A') {
			return $this->_dumpArray($result);
		}
		return NULL;
	}

	public function _dumpNode(CCReaderWalkResult $result) {
		$arr = array();
		$currOffset = 0;
		for ($i = 0; $i < $result->length; ++$i) {
			$data = shmop_read($this->_shmId, 4 + $result->valueOffset + $currOffset, 4);
			$data = unpack('Nlength', $data);
			$length = $data['length'];
			$currOffset += 4;

			$data = shmop_read($this->_shmId, 4 + $result->valueOffset + $currOffset, $length + 4);
			$currOffset += $length + 4;

			$data = unpack('a' . $length . 'key/Noffset', $data);
			$name = $data['key'];
			$nodeOffset = $data['offset'];
			$subResult = new CCReaderWalkResult();
			$data = shmop_read($this->_shmId, 4 + $nodeOffset, 5);
			$data = unpack('Ctype/Nlength', $data);
			$subResult->type = chr($data['type']);
			$subResult->length = $data['length'];
			$subResult->valueOffset = $nodeOffset + 5;
			$arr[$name] = $this->_dump($subResult);
		}
		return $arr;
	}

	private function _dumpArray(CCReaderWalkResult $result) {
		$arr = array();
		for ($i = 0; $i < $result->length; ++$i) {
			$data = shmop_read($this->_shmId, 4 + $result->valueOffset + 4 * $i, 4);
			$data = unpack('Noffset', $data);
			$nodeOffset = $data['offset'];
			$subResult = new CCReaderWalkResult();
			$data = shmop_read($this->_shmId, 4 + $nodeOffset, 5);
			$data = unpack('Ctype/Nlength', $data);
			$subResult->type = chr($data['type']);
			$subResult->length = $data['length'];
			$subResult->valueOffset = $nodeOffset + 5;
			$arr[] = $this->_dump($subResult);
		}
		return $arr;
	}

	private function _parse(&$keys) {
		if (strlen($keys) == 0) {
			return false;
		}

		$parsedKey = new CCReaderParsedKey();
		$parsedKey->key = '';
		$offset = 0;
		$charStart = $keys[$offset];
		if ($charStart == '[') {
			$parsedKey->type = CCReaderParsedKey::TYPE_INDEX;
			$offset++;
			while ($offset < strlen($keys) && ($keys[$offset] != ']')) {
				$parsedKey->key .= $keys[$offset++];
			}
		}
		else {
			$parsedKey->type = CCReaderParsedKey::TYPE_NAME;
			while ($offset < strlen($keys) && ($keys[$offset] != '.') && ($keys[$offset] != '[')) {
				$parsedKey->key .= $keys[$offset++];
			}
		}

		if (strlen($parsedKey->key) == 0) {
			return false;
		}

		$keys = ($offset < strlen($keys)) ? substr($keys, $offset) : '';
		$keys = ltrim($keys, '.]');
		return $parsedKey;
	}
}
