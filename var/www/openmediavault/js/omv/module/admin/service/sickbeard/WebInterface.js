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
// require("js/omv/workspace/panel/Panel.js")

Ext.define("OMV.module.admin.service.sickbeard.WebInterface", {
    extend : "OMV.workspace.panel.Panel",

    initComponent : function() {
        var me = this;

        var link = "http://" + location.hostname + ":8081";

        me.html = "<a href='" + link + "' />Sickbeard</a>";
        me.callParent(arguments);
    }
});

OMV.WorkspaceManager.registerPanel({
    id        : "webinterface",
    path      : "/service/sickbeard",
    text      : _("Web Interface"),
    position  : 20,
    className : "OMV.module.admin.service.sickbeard.WebInterface"
});