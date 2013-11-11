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
// require("js/omv/WorkspaceManager.js")

OMV.WorkspaceManager.registerNode({
	id: "sickbeard",
	path: "/service",
	text: _("Sickbeard"),
	icon16: "images/sickbeard.png",
	position: 40
});

/**
* @class OMV.module.admin.privilege.sickbeard.Settings
* @derived OMV.workspace.form.Panel
*/
Ext.define("OMV.module.admin.service.sickbeard.Settings", {
	extend: "OMV.workspace.form.Panel",

	rpcService: "Sickbeard",
	rpcGetMethod: "getSettings",
	rpcSetMethod: "setSettings",

	getFormItems: function() {
		return [{
			xtype: "fieldset",
			title: _("General settings"),
			fieldDefaults: {
				labelSeparator: ""
			},
			items: [{
				xtype: "checkbox",
				name: "enable",
				fieldLabel: _("Enable"),
				checked: false
			},{
				  xtype: 'box',
				  autoEl: {tag: 'a', href: 'javascript:window.location.replace(window.location + ":8081/sickbeard");', html: 'Open sickbeard'}
			}]
		}];
	}
});


OMV.WorkspaceManager.registerPanel({
	id: "settings",
	path: "/service/sickbeard",
	text: _("Settings"),
	position: 10,
	className: "OMV.module.admin.service.sickbeard.Settings"
});