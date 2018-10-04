const PORT = 3000;
var app = require('express')();
var http = require('http').Server(app);
var io = require('socket.io')(http);
var ip = require('ip');

var cli = io.of('/client');
var cliesp = io.of('/esp8266');

var json = {
    "target": "1", // 1: led bao ket noi, 2: dieu khien 4 den led
    "data": true    // target=1==> true | false, target=2 ==> chuoi json
};
console.log("Server nodejs chay tai dia chi:" + ip.address() + ": " + PORT);
cli.on('connection', function(socket){
    console.log("client connected");
    socket.on('send', function(data){
        console.log(data);
        json = {
            "target" : 2,
            "data" : data
        };
        console.log(json);
        cliesp.emit('COM', json);
    });
    socket.on('disconnect', function(){
        console.log("web disconnect")
    });
});
cliesp.on('connection', function(socket){
    console.log("esp8266 connected");
    socket.on('atime', function(data){
        console.log(data);
        json = {
            "target" : 1,
            "data" : (data % 2 == 0)
        };
        console.log(json);
        cliesp.emit('COM', json);
    });
    socket.on('disconnect', function(){
        console.log("esp8266 disconnected");
    });
});
http.listen(PORT, function() {
    console.log('listening on localhost:' + PORT);
 });
 

