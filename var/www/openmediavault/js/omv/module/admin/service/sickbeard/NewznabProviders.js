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
// require("js/omv/workspace/grid/Panel.js")
// require("js/omv/workspace/window/Form.js")
// require("js/omv/workspace/window/plugin/ConfigObject.js")
// require("js/omv/Rpc.js")
// require("js/omv/data/Store.js")
// require("js/omv/data/Model.js")
// require("js/omv/data/proxy/Rpc.js")
// require("js/omv/window/Execute.js")
// require("js/omv/form/field/SharedFolderComboBox.js")

/**
 * @class OMV.module.admin.service.sickbeard.NewznabProviders
 * @derived OMV.workspace.window.Form
 */
Ext.define("OMV.module.admin.service.sickbeard.NewznabProvider", {
	extend: "OMV.workspace.window.Form",
	uses: [
		"OMV.form.field.SharedFolderComboBox",
		"OMV.workspace.window.plugin.ConfigObject"
	],

	rpcService: "Sickbeard",
	rpcGetMethod: "getNewznabProvider",
	rpcSetMethod: "setNewznabProvider",
	plugins: [{
		ptype: "configobject"
	}],
	width: 540,
	height: 375,

	/**
	 * The class constructor.
	 * @fn constructor
	 * @param uuid The UUID of the database/configuration object. Required.
	 */

	getFormItems: function() {
		return [{
			xtype: "checkbox",
			name: "enable",
			fieldLabel: _("Enable"),
			checked: true
		},{
			xtype: "textfield",
			name: "name",
			fieldLabel: _("Name"),
			allowBlank: true,
			vtype: "name"
		},{
			xtype: "textfield",
			name: "hash",
			fieldLabel: _("Hash"),
			allowBlank: true,
			vtype: "hash"
		},{
			xtype: "textfield",
			name: "url",
			fieldLabel: _("URL"),
			allowBlank: true,
			vtype: "url"
		}];
	}
});

/**
 * @class OMV.module.admin.service.sickbeard.NewznabProviders
 * @derived OMV.workspace.grid.Panel
 */
Ext.define("OMV.module.admin.service.sickbeard.NewznabProviders", {
	extend: "OMV.workspace.grid.Panel",
	requires: [
		"OMV.Rpc",
		"OMV.data.Store",
		"OMV.data.Model",
		"OMV.data.proxy.Rpc",
		"OMV.util.Format",
		"OMV.window.Execute"
	],
	uses: [
		"OMV.module.admin.service.sickbeard.NewznabProvider"
	],

	hidePagingToolbar: false,
	stateful: true,
	stateId: "f8a8cf1c-a107-11e1-a5a1-00221568ca88",
	columns: [{
		xtype: "booleaniconcolumn",
		text: _("Enabled"),
		sortable: true,
		dataIndex: "enable",
		stateId: "enable",
		align: "center",
		width: 80,
		resizable: false,
		trueIcon: "switch_on.png",
		falseIcon: "switch_off.png"
	},{
		text: _("Name"),
		sortable: true,
		dataIndex: "name",
		stateId: "name"
	},{
		text: _("URL"),
		sortable: true,
		dataIndex: "url",
		stateId: "url"
	}],

	initComponent: function() {
		var me = this;
		Ext.apply(me, {
			store: Ext.create("OMV.data.Store", {
				autoLoad: true,
				model: OMV.data.Model.createImplicit({
					idProperty: "name",
					fields: [
						{ name: "name", type: "string" },
						{ name: "enable", type: "boolean" },
						{ name: "hash", type: "string" },
						{ name: "url", type: "string" },
					]
				}),
				proxy: {
					type: "rpc",
					rpcData: {
						service: "Sickbeard",
						method: "getNewznabProviderList"
					}
				}
			})
		});
		me.callParent(arguments);
	},

	getTopToolbarItems: function() {
		var me = this;
		var items = me.callParent(arguments);
		return items;
	},

	onSelectionChange: function(model, records) {
		var me = this;
		me.callParent(arguments);
		// Process additional buttons.
		var tbarRunCtrl = me.queryById(me.getId() + "-run");
		if(records.length <= 0)
			tbarRunCtrl.disable();
		else if(records.length == 1)
			tbarRunCtrl.enable();
		else
			tbarRunCtrl.disable();
	},

	onAddButton: function() {
		var me = this;
		Ext.create("OMV.module.admin.service.sickbeard.NewznabProviders", {
			title: _("Add job"),
			name: "",
			listeners: {
				scope: me,
				submit: function() {
					this.doReload();
				}
			}
		}).show();
	},

	onEditButton: function() {
		var me = this;
		var record = me.getSelected();
		Ext.create("OMV.module.admin.service.sickbeard.NewznabProviders", {
			title: _("Edit job"),
			name: record.get("name"),
			listeners: {
				scope: me,
				submit: function() {
					this.doReload();
				}
			}
		}).show();
	},

	doDeletion: function(record) {
		var me = this;
		OMV.Rpc.request({
			scope: me,
			callback: me.onDeletion,
			rpcData: {
				service: "Sickbeard",
				method: "deleteNewznabProvider",
				params: {
					name: record.get("name")
				}
			}
		});
	},

});

OMV.WorkspaceManager.registerPanel({
	id: "newznab_providers",
	path: "/service/sickbeard",
	text: _("Newsnab Providers"),
	position: 20,
	className: "OMV.module.admin.service.sickbeard.NewznabProviders"
});
