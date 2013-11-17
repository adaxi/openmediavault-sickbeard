<?php
/**
 *
 * @license   http://www.gnu.org/licenses/gpl.html GPL Version 2
 * @author    Gerik Bonaert <dev@adaxisoft.be>
 * @copyright Copyright (c) 2013 Gerik Bonaert
 *
 * Sickbeard for OpenMediaVault is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * any later version.
 *
 * OpenMediaVault is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenMediaVault. If not, see <http://www.gnu.org/licenses/>.
 */
require_once("openmediavault/object.inc");
require_once("openmediavault/config.inc");
require_once("openmediavault/error.inc");
require_once("openmediavault/rpcservice.inc");
require_once("openmediavault/notify.inc");
require_once("openmediavault/configobj.inc");

class OMVRpcServiceSickbeard extends OMVRpcServiceAbstract {
	/**
	 * Get the RPC service name.
	 */
	public function getName() {
		return "Sickbeard";
	}

	/**
	 * Initialize the RPC service.
	 */
	public function initialize() {
		$this->registerMethod("getSettings");
		$this->registerMethod("setSettings");
		$this->registerMethod("getNewznabProviderList");
		$this->registerMethod("getNewznabProvider");
		$this->registerMethod("setNewznabProvider");
		$this->registerMethod("deleteNewznabProvider");
	}

	/**
	 * Get configuration object.
	 * @param params The method parameters.
	 * @param context The context of the caller.
	 * @return The requested configuration object.
	 * @throw E_CONFIG_GET_OBJECT_FAILED
	 */
	function getSettings($params, $context) {
		global $xmlConfig;
		// Validate the RPC caller context.
		$this->validateMethodContext($context, array(
			  "role" => OMV_ROLE_ADMINISTRATOR
		  ));
		// Get the configuration object.
		$xpath = "//services/sickbeard";
		$object = $xmlConfig->get($xpath);
		if(is_null($object)) {
			throw new OMVException(OMVErrorMsg::E_CONFIG_GET_OBJECT_FAILED,
			  $xpath);
		}

		$sickbeardIni = new ConfigObj("/etc/sickbeard/config.ini");

		return array(
			"enable" => boolval($object['enable']),
			"api_key" => $sickbeardIni->get('General', 'api_key',""),
		);
	}

	/**
	 * Set configuration object.
	 * @param params The method parameters.
	 * @param context The context of the caller.
	 * @return The stored configuration object.
	 * @throw E_CONFIG_GET_OBJECT_FAILED
	 * @throw E_CONFIG_SET_OBJECT_FAILED
	 */
	function setSettings($params, $context) {
		global $xmlConfig;
		// Validate the RPC caller context.
		$this->validateMethodContext($context, array(
			  "role" => OMV_ROLE_ADMINISTRATOR
		  ));
		// Validate the parameters of the RPC service method.
		$this->validateMethodParams($params, '{
			  "type":"object",
			  "properties":{
				  "enable":{"type":"boolean"},
				  "api_key":{"type":"string"},
			  }
		  }');
		// Get the existing configuration object.
		$xpath = "//services/sickbeard";
		$object = $xmlConfig->get($xpath);
		if(is_null($object)) {
			throw new OMVException(OMVErrorMsg::E_CONFIG_GET_OBJECT_FAILED,
			  $xpath);
		}
		// Update the existing configuration object.
		$object = array_merge($object, array(
			"enable" => array_boolval($params, "enable"),
		));
		if(FALSE === $xmlConfig->replace($xpath, $object)) {
			throw new OMVException(OMVErrorMsg::E_CONFIG_SET_OBJECT_FAILED);
		}
		
		$object = array_merge($object, array(
			"api_key" => $params['api_key'],
		));
		
		$sickbeardIni = new ConfigObj("/etc/sickbeard/config.ini");
		$sickbeardIni->set('General', 'api_key', $params["api_key"]);
		$sickbeardIni->save();		
		
		// Notify configuration changes.
		$dispatcher = &OMVNotifyDispatcher::getInstance();
		$dispatcher->notify(OMV_NOTIFY_MODIFY,
		  "org.openmediavault.services.sickbeard", $object);
		// Return the configuration object.
		return $object;
	}
	

	/**
	 * Get list of newznab provider configuration objects.
	 * @param params An array containing the following fields:
	 *   \em start The index where to start.
	 *   \em limit The number of objects to process.
	 *   \em sortfield The name of the column used to sort.
	 *   \em sortdir The sort direction, ASC or DESC.
	 * @param context The context of the caller.
	 * @return An array containing the requested objects. The field \em total
	 *   contains the total number of objects, \em data contains the object
	 *   array. An exception will be thrown in case of an error.
	 * @throw E_CONFIG_GET_OBJECT_FAILED
	 */
	public function getNewznabProviderList($params, $context) {
		global $xmlConfig;
		// Validate the RPC caller context.
		$this->validateMethodContext($context, array(
				"role" => OMV_ROLE_ADMINISTRATOR
		));
		// Validate the parameters of the RPC service method.
		$this->validateMethodParams($params, '{
			  "type":"object",
			  "properties":{
				  "start":{"type":"integer"},
				  "limit":{'.$GLOBALS['OMV_JSONSCHEMA_COUNTFIELD'].'},
				  "sortfield":{'.$GLOBALS['OMV_JSONSCHEMA_SORTFIELD'].'},
				  "sortdir":{'.$GLOBALS['OMV_JSONSCHEMA_SORTDIR'].'}
			  }
		  }');
		// Get the configuration objects.
		$sickbeardIni = new ConfigObj("/etc/sickbeard/config.ini");
		$newznab_data = $sickbeardIni->get('Newznab', 'newznab_data', '');
		
		$objects = array();
		$newznabs = explode("!!!", $newznab_data);
		foreach ($newznabs as $newznab) {
			if (!empty($newznab)) {
				$newznab = split("|", $newznab);
				$objects[] = array(
					'name' => $newznab[0],
					'url' => $newznab[1],
					'hash' => $newznab[2],
					'enable' => boolval($newznab[3]),
				);
			}
		}
		
		// Filter result.
		return $this->applyFilter($objects, $params['start'],
				$params['limit'], $params['sortfield'], $params['sortdir']);
	}
	
	/**
	 * Get a newznab provider configuration object.
	 * @param params An array containing the following fields:
	 *   \em uuid The UUID of the configuration object.
	 * @param context The context of the caller.
	 * @return The requested configuration object.
	 * @throw E_CONFIG_GET_OBJECT_FAILED
	 */
	function getNewznabProvider($params, $context) {
		global $xmlConfig;
		// Validate the RPC caller context.
		$this->validateMethodContext($context, array(
				"role" => OMV_ROLE_ADMINISTRATOR
		));
		// Validate the parameters of the RPC service method.
		$this->validateMethodParams($params, '{
			  "type":"object",
			  "properties":{
				  "uuid": { "type":"string" }
			  }
		  }');
		
		
		// Get the configuration object.
		$sickbeardIni = new ConfigObj("/etc/sickbeard/config.ini");
		$newznab_data = $sickbeardIni->get('Newznab', 'newznab_data', '');
		
		$newznabs = explode("!!!", $newznab_data);
		foreach ($newznabs as $newznab) {
			if (!empty($newznab)) {
				$newznab = split("|", $newznab);
				$object = array(
						'name' => $newznab[0],
						'url' => $newznab[1],
						'hash' => $newznab[2],
						'enable' => boolval($newznab[3]),
				);
				if ($object['name'] === $param['uuid']) {
					return $object;
				}
			}
		}
		
		return array(
			'enable' => false,
			'url' => '',
			'hash' => '',
			'name' => ''
		);
		
	}
	
	
	/**
	 * Set (add/update) a newznab provider configuration object.
	 * @param params The method parameters.
	 * @param context The context of the caller.
	 * @return The stored configuration object.
	 * @throw E_CONFIG_SET_OBJECT_FAILED
	 */
	function setNewznabProvider($params, $context) {
		global $xmlConfig;
		// Validate the RPC caller context.
		$this->validateMethodContext($context, array(
				"role" => OMV_ROLE_ADMINISTRATOR
		));
		// Validate the parameters of the RPC service method.
		$this->validateMethodParams($params, '{
			  "type":"object",
			  "properties":{
				  "uuid":{"type":"string"},
				  "enable":{"type":"boolean"},
				  "name":{"type":"string"},
				  "url":{"type":"string"},
				  "hash":{"type":"string"}
			  }
		  }');
		
		
		// Get the configuration objects.
		$sickbeardIni = new ConfigObj("/etc/sickbeard/config.ini");
		$newznab_data = $sickbeardIni->get('Newznab', 'newznab_data');
		
		// Retrieve the object to be updated
		$objects = array();
		$object = array();
		$notify = OMV_NOTIFY_CREATE;
		$newznabs = explode("!!!", $newznab_data);		
		foreach ($newznabs as $newznab) {
			$newznab = split("|", $newznab);
			if ($newznab[0] === $params['name']) {
				$object = array(
					'name' => $newznab[0],
					'url' => $newznab[1],
					'hash' => $newznab[2],
					'enable' => boolval($newznab[3]),
				);
				$notify = OMV_NOTIFY_MODIFY;
			} else {
				$objects[] = array(
					'name' => $newznab[0],
					'url' => $newznab[1],
					'hash' => $newznab[2],
					'enable' => boolval($newznab[3]),
				);
			}
		}
			
		// Update the object to be updated
		$object = array_merge($object, array(
				"enable" => array_boolval($params, 'enable'),
				"name" => $params['name'],
				"url" => $params['url'],
				"hash" => $params['hash']
		));
		$objects[] = $object;

		// Rebuild the ini string
		$newznabs = array();
		foreach ($objects as $object) {
			$newznab = sprintf("%s|%s|%s|%s", 
					$object['name'],
					$object['url'],
					$object['hash'],
					($object['enable']?"1":"0")
			);
		}
		$newznab_data = implode("!!!", $newznabs);
		
		$sickbeardIni->set('Newznab', 'newznab_data', $newznab_data);
		$sickbeardIni->save();
		
		// Notify configuration changes.
		$dispatcher = &OMVNotifyDispatcher::getInstance();
		$dispatcher->notify($notify,
				"org.openmediavault.services.sickbeard.newznab_provider.newznab", $object);
		// Return the configuration object.
		return $object;
	}
	

	/**
	 * Delete a newznab provider configuration object.
	 * @param params An array containing the following fields:
	 *   \em uuid The UUID of the configuration object.
	 * @param context The context of the caller.
	 * @return The deleted configuration object.
	 */
	public function deleteNewznabProvider($params, $context) {
		// Validate the RPC caller context.
		$this->validateMethodContext($context, array(
				"role" => OMV_ROLE_ADMINISTRATOR
		));
		// Validate the parameters of the RPC service method.
		$this->validateMethodParams($params, '{
			  "type":"object",
			  "properties":{
				  "name":{'.$GLOBALS['OMV_JSONSCHEMA_UUID'].'}
			  }
		  }');
		
		// Get the configuration objects.
		$sickbeardIni = new ConfigObj("/etc/sickbeard/config.ini");
		$newznab_data = $sickbeardIni->get('Newznab', 'newznab_data');
		
		// Retrieve the object to be deleted
		$objects = array();
		$object = array();
		$newznabs = explode("!!!", $newznab_data);
		foreach ($newznabs as $newznab) {
			$newznab = split("|", $newznab);
			if ($newznab[0] === $params['name']) {
				$object = array(
						'name' => $newznab[0],
						'url' => $newznab[1],
						'hash' => $newznab[2],
						'enable' => boolval($newznab[3]),
				);
				$notify = OMV_NOTIFY_MODIFY;
			} else {
				$objects[] = array(
						'name' => $newznab[0],
						'url' => $newznab[1],
						'hash' => $newznab[2],
						'enable' => boolval($newznab[3]),
				);
			}
		}
		
		// Rebuild the ini string
		$newznabs = array();
		foreach ($objects as $object) {
			$newznab = sprintf("%s|%s|%s|%s",
					$object['name'],
					$object['url'],
					$object['hash'],
					($object['enable']?"1":"0")
			);
		}
		$newznab_data = implode("!!!", $newznabs);
		
		$sickbeardIni->set('Newznab', 'newznab_data', $newznab_data);
		$sickbeardIni->save();
		
		// Delete the configuration object.
		return $object;
	}

	
}

// Register the RPC service.
$rpcServiceMgr = &OMVRpcServiceMgr::getInstance();
$rpcServiceMgr->registerService(new OMVRpcServiceSickbeard());
?>
