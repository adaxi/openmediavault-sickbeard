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

class OMVModuleSickbeard extends \OMV\Engine\Module\ServiceAbstract
  implements \OMV\Engine\Notify\IListener, \OMV\Engine\Module\IServiceStatus {
	/**
	 * Get the module name.
	 */
	public function getName() {
		return "Sickbeard";
	}

	/**
	 * Generate the configuration.
	 */
	public function applyConfig() {
		$cmd = new \OMV\System\Process("omv-mkconf", "sickbeard");
		$cmd->setRedirect2to1();
		$cmd->execute();
	}
	
	/**
	 * Stop the managed service.
	 */
	public function stopService() {
		$systemCtl = new \OMV\System\SystemCtl("sickbeard");
		$systemCtl->disable(TRUE);
	}
	
	/**
	 * Start the managed service.
	 */
	public function startService() {
		$db = \OMV\Config\Database::getInstance();
		$object = $db->get("conf.service.sickbeard");
		if (TRUE !== $object->get("enable"))
			return;
		$systemCtl = new \OMV\System\SystemCtl("sickbeard");
		$systemCtl->enable(TRUE);
	}
	
	/**
	 * Get the status of the managed service.
	 */
	public function getStatus() {
		$db = \OMV\Config\Database::getInstance();
		$object = $db->get("conf.service.sickbeard");
		$systemCtl = new \OMV\System\SystemCtl("sickbeard");
		return [
			"name" => $this->getName(),
			"title" => gettext("Sickbeard"),
			"enabled" => $object->get("enable"),
			"running" => $systemCtl->isActive()
		];
	}
	

	/**
	 * Bind listeners.
	 */
	function bindListeners(\OMV\Engine\Notify\Dispatcher $dispatcher) {
		$dispatcher->addListener(
		  OMV_NOTIFY_MODIFY,
		  "org.openmediavault.conf.service.sickbeard",
		  [$this, "setDirty"]);
	}
}
