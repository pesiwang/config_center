<?php
class CCWriter {
	public $data;
	public function write($node) {
		$this->data = '';
		$rootOffset = $this->_serialize($node);
		$this->data = pack('N', $rootOffset) . $this->data;
	}

	public function _serialize($node) {
		if (is_array($node)) {
			if (array_keys($node) !== range(0, count($node) - 1)) {
				return $this->_serializeNode($node);
			}
			else {
				return $this->_serializeArray($node);
			}
		}
		else {
			return $this->_serializeData($node);
		}
	}

	private function _serializeData($node) {
		$offset = strlen($this->data);
		if (is_integer($node)) {
			$this->data .= pack('CNl', ord('I'), 4, $node);
		}
		else if (is_bool($node)) {
			$this->data .= pack('CNC', ord('B'), 1, $node);
		}
		else if (is_string($node)) {
			$this->data .= pack('CNa*', ord('S'), strlen($node), $node);
		}
		else {
			die("unknown data\n");
		}
		return $offset;
	}

	private function _serializeNode($node) {
		$data = pack('CN', ord('O'), count($node));
		ksort($node, SORT_STRING);
		foreach ($node as $name => $sub) {
			$data .= pack('Na' . strlen($name) . 'N', strlen($name), $name, $this->_serialize($sub));
		}
		$this->data .= $data;
		return strlen($this->data) - strlen($data);
	}

	private function _serializeArray($node) {
		$data = pack('CN', ord('A'), count($node));
		foreach ($node as $sub) {
			$data .= pack('N', $this->_serialize($sub));
		}

		$this->data .= $data;
		return strlen($this->data) - strlen($data);
	}
}
