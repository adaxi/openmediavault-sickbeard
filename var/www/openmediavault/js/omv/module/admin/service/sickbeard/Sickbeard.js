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
	id: "addns",
	path: "/service",
	text: _("Sickbeard"),
	icon16: "images/sickbeard.png",
	position: 40
});

/**
* @class OMV.module.admin.privilege.sickbeard.View
* @derived OMV.workspace.form.Panel
*/
Ext.define("OMV.module.admin.service.sickbeard.View", {
    extend: "Ext.panel.Panel",
    initComponent: function() {
            var me = this;
            window.open("http://localhost:8081/sickbeard","blank");
            me.callParent(Ext.panel.panel);
    },
});


OMV.WorkspaceManager.registerPanel({
	id: "view",
	path: "/service/sickbeard",
	text: _("View"),
	position: 5,
	className: "OMV.module.admin.service.sickbeard.View"
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