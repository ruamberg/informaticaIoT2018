#ifndef INCLUDE_VIEW_H
#define INCLUDE_VIEW_H

#include "Arduino.h"
#include "./wifi.h"
#include "./storage.h"
#include <ESP8266WiFi.h>

#include "../constants/events.h";
#include "../constants/routes.h";

#define TITLE String("L2IoT Local Device")
namespace View
{
    namespace
    {
        inline String networkSelectForm(void)
        {

            int n = 0;
            String *aps = Wifi::getWifiList(&n);

            if(n == 0)
            {
                return "<p>No networks found</p>";
            }

            String s = "<form action='" + STA_CONNECT + "' method='GET'><select name='ssid'>";

            for(int i = 0; i < n; i++)
            {
                s += "<option value='" + aps[i] + "'>" + aps[i] + "</option>";
            }

            s += "</select><input type='password' name='password' /><button type='submit'>Connect</button></form>";

            return s;
        }

        inline String networkManualForm(void)
        {
            String s = "<form action='" + STA_CONNECT + "' method='GET'><input name='ssid' type='text' placeholder='SSID'/><input type='password' name='password' placeholder='Password'/><button type='submit'>Connect</button></form>";

            return s;
        }

        inline String ssid(void)
        {
            String ssid;
            String password;
            Storage::get(&ssid, &password);

            return ssid;
        }
    }

    String evt = "<script>const EventManager = function(){this.events = {};}; EventManager.prototype.on = function(evt, handler) {if(this.events[evt]) this.events[evt].push(handler); else this.events[evt] = [handler];}; EventManager.prototype.emit = function(evt, payload) {this.events[evt].forEach(handler => handler(payload));};</script>";
    String wsc = "<script>const ws = new WebSocket('ws://' + window.location.hostname + ':" + String(WS_PORT) + "/'); ws.event = new EventManager(); ws.emitEvent = function(event, payload) {const obj = {event: event}; if(payload) Object.assign(obj, {payload: payload}); this.send(JSON.stringify(obj));}; ws.onopen = function() {ws.emitEvent('" + EVT_PULL + "')}; /*ws.onclose = function() {console.log('ws closed');}; ws.onerror = function() {console.log('ws error');};*/ ws.onmessage = function(evt) {const request = JSON.parse(evt.data); ws.event.emit(request.event, request.payload);};</script>";
    String dom = "<script>const dom = {}; dom.body = document.querySelector('body'); function populateDom() {dom.tableCells = document.querySelectorAll('.table-cell'); dom.connection = document.querySelector('#connection');};</script>";
    String upd = "<script>function update(data) {if(data.sensors && dom.tableCells) {console.log(dom.tableCells);data.sensors.forEach((sensor, index) => dom.tableCells[index].textContent = sensor)} if(data.connection && dom.connection) {dom.connection.textContent = (data.connection.flag? 'CONNECTED to ' + data.connection.ssid + '.' : 'DISCONNECTED. Go to the WiFi manager by clicking '); if(!data.connection.flag) {dom.connection.innerHTML += '<a href=\"" + STA_WIFI_FORM + "\">here</a>.';}}}</script>";
    String rnd = "<script>function render(data) {const x = data.sensors.length; const table = '<table>' + data.sensors.map((s,i) => '<tr><th>Valor ' + (i + 1) + '</th><td class=\"table-cell\"></td></tr>').join('') + '</table>'; const body = \"<main><h1>Informatica IoT Local Device Home Page</h1><h4>Status: <span id='connection'></span></h4>\" + table + \"</main>\"; dom.body.innerHTML = body;populateDom();update(data)}</script>";
    String sys = "<script>const status = {}; ws.event.on('" + EVT_UPDT + "', payload => {const shouldRender = Object.keys(status).length == 0; Object.assign(status, payload); if(shouldRender) {setTimeout(() => {dom.body.innerHTML = '';render(status);main();}, 1000);} else {update(payload)}});function main(){/*dom.testBtn.addEventListener('click', () => {status.test = dom.test.value; ws.emitEvent('" + EVT_PUSH + "', status)})*/}</script>";
    String js = evt + wsc + dom + upd + rnd + sys;

    String css = "<style></style>";
    String doctype = "<!DOCTYPE html>";
    String head = "<head><meta charset='utf-8'/><meta name='viewport' content='width = device-width, initial-scale = 1.0, maximum-scale = 1.0, user-scalable=0'/><title>" + TITLE + "</title>" + css + "</head>";
    String index = doctype + "<html>" + head + "<body><h1>Loading</h1>" + js + "</body></html>";
    String connectionFail = doctype + "<html>" + head + "<body><h1>Connection Failed</h1><h4>We couldn't connect with the provided  WiFi credentials.</h4><a href='" + STA_WIFI_FORM + "'>Click here to try again.</a></body></html>";
    String connectionSuccess = doctype + "<html>" + head + "<body><h1>Connection successful</h1><h4>You can always change your netwok by navigating to " + STA_WIFI_FORM + ".</h4><a href='/'>Go to the home page</a></body></html>";
    String tokenMismatch = doctype + "<html>" + head + "<body><h1>Token Mismatch</h1><h4>The security token received for this communication was wrong. Please <a href='" + STA_WIFI_FORM + "'>go back and retry to connect.</a></h4></body></html>";
    String wifiManager = doctype + "<html>" + head + "<body><h1>Connect to a network</h1><p>To refresh this list, please, reload the page.</p>" + networkSelectForm() + "<h4>Manual connection</h4>" + networkManualForm() + "</body></html>";
}


#endif
