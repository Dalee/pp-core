<?php
class NLAntiCheating {
	var $protectedId;
	var $userIP;
	var $userAgent;
	var $forbiddenTimeInterval;

	var $votingsFile;
	var $cheatersFile;
	var $serversFile;
	var $proxysFile;

	var $votingsDBA;
	var $cheatersDBA;
	var $serversDBA;
	var $proxysDBA;

	var $isServer;
	var $isOpenProxy;
	var $isClickCheater;
	var $isOutsideRequest;
	var $isRestrictedCheater;



	function NLAntiCheating($protectedId, $userIP, $userAgent, $forbiddenTimeInterval, $votingsFile, $cheatersFile, $serversFile, $proxysFile) {
		$this->protectedId           = $protectedId;
		$this->userIP                = $userIP;
		$this->userAgent             = $userAgent;
		$this->forbiddenTimeInterval = $forbiddenTimeInterval;
		$this->votingsFile           = $votingsFile;
		$this->cheatersFile          = $cheatersFile;
		$this->serversFile           = $serversFile;
		$this->proxysFile            = $proxysFile;

		$this->isServer              = false;
		$this->isOpenProxy           = false;
		$this->isClickCheater        = false;
		$this->isOutsideRequest      = false;
		$this->isRestrictedCheater   = false;

		$this->votingsDBA = new NLDBADatabase($this->votingsFile, 'db3');
		if (!is_file($this->votingsFile)) {
			$this->votingsDBA->Create();
		}

		$this->cheatersDBA = new NLDBADatabase($this->cheatersFile, 'db3');
		if (!is_file($this->cheatersFile)) {
			$this->cheatersDBA->Create();
		}

		$this->serversDBA = new NLDBADatabase($this->serversFile, 'db3');
		if (!is_file($this->serversFile)) {
			$this->serversDBA->Create();
		}

		$this->proxysDBA = new NLDBADatabase($this->proxysFile, 'db3');
		if (!is_file($this->proxysFile)) {
			$this->proxysDBA->Create();
		}
	}

	function IsCheater() {
		return ($this->IsRestrictedCheater() || $this->IsClickCheater());
//		return ($this->IsClickCheater());
	}

	function IsClickCheater() {
		$userKey = $this->protectedId."\t".$this->userIP."\t".$this->userAgent;
		$result = $this->votingsDBA->Select($userKey);
		if ($result) {
			if ($result + $this->forbiddenTimeInterval > time()) {
				$this->SaveCheater($userKey, 'CLICK ');
				$this->isClickCheater = true;
				return true;
			} else {
				$this->votingsDBA->Update($userKey, time());
				return false;
			}

		} else {
			$this->votingsDBA->Insert($userKey, time());
			return false;
		}
	}

	function IsProxy() {
		if (isset($_SERVER['HTTP_CLIENT_IP']) || isset($_SERVER['HTTP_X_FORWARDED_FOR'])) {
			$userKey = $this->protectedId."\t".$this->userIP."\t".$this->userAgent;
			$this->SaveCheater($userKey, 'PROXY ');
			return true;
		}
		return false;
	}

	function IsOpenProxy() {
		$userKey = $this->protectedId."\t".$this->userIP."\t".$this->userAgent;
		$result = $this->proxysDBA->Select($this->userIP);

		if ($result) {
			$this->SaveCheater($userKey, 'OPROXY ');
			$this->isOpenProxy = true;
			return true;
		} else {
			return false;
		}
	}

	function IsServer() {
		$userKey = $this->protectedId."\t".$this->userIP."\t".$this->userAgent;
		$result = $this->serversDBA->Select($this->userIP);

		if ($result) {
			$this->SaveCheater($userKey, 'SERVER');
			$this->isServer = true;
			return true;

		} else {
			$errstr = $errno = 0;
			$fd = @fsockopen($this->userIP, 80, $errno, $errstr, 0.2);

			if ($fd) {
				fclose($fd);
				$this->serversDBA->Insert($this->userIP, 1);
				$this->SaveCheater($userKey, 'SERVER');
				$this->isServer = true;
				return true;
			} else {
				return false;
			}
		}

	}

	function IsOutsideRequest() {
		$host       = RTUserRequest::GetHttpHost();
		$httpMethod = RTUserRequest::GetHttpMethod();
		$referer    = RTUserRequest::GetHttpReferer();

		if ($httpMethod = 'POST' && (!$referer || !is_integer(strpos($referer, $host)))) {
			$this->isOutsideRequest = true;
			return true;
		} else {
			return false;
		}
	}

	function IsRestrictedCheater() {
		$restricted = array('81.19.68', '81.19.65', '212.116.236', '80.241.32', '212.116.225', '212.100.132', '212.116.224', '217.77.109');
		$userKey = $this->protectedId."\t".$this->userIP."\t".$this->userAgent;
		foreach ($restricted as $i) {
			if (is_numeric(strpos($this->userIP, $i)) && strpos($this->userIP, $i)==0) {
				$this->SaveCheater($userKey, 'INSIDE');
				$this->isRestrictedCheater = true;
				return true;
			}
		}

		return false;
	}

	function SaveCheater($cheater, $type) {
		$fd = fopen($this->cheatersFile, 'a');
		flock($fd, LOCK_EX);
		fwrite($fd, $type."\t".date("Y/m/d H:i:s")."\t".$cheater."\n");
		flock($fd, LOCK_UN);
		fclose($fd);
	}
}

class NLAdvancedAntiCheating {
	function NLAdvancedAntiCheating() {
		session_start();
	}

	function CreateValue() {
		if (!isset($_SESSION['anticheatingImageValue'])) {
			mt_srand((double)microtime()*1000000000);
			$_SESSION['anticheatingImageValue'] = rand(99999,999998);
			return true;
		} else {
			return false;
		}
	}

	function GetValue() {
		if (isset($_SESSION['anticheatingImageValue'])) {
			return $_SESSION['anticheatingImageValue'];
		} else {
			return NULL;
		}
	}

	function IsUnCorrectValue($value) {
		if ($this->GetValue() === (int)$value) {
			return false;
		} else {
			return true;
		}
	}
}

?>
