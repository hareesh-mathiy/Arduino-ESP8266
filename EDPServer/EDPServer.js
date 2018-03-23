var express = require("express");
var app = express();
var http = require("http").Server(app);
var io = require("socket.io")(http);
var port = process.env.PORT || 3001;
var fs = require('fs');
var shortid = require('shortid');

http.listen(port, function (){
    console.log("Listening on ", port);
});

console.log('Server started.');

io.sockets.on('connection', function(socket){

    socket.on('disconnect', function(){

    });

    socket.on('sendData', function (data) {
        var filename = "data.txt";
        console.log(data);
        fs.open(filename,'r',function(err, fd){
            if (err) {
                fs.writeFile(filename, data + '\n', function(err) {
                    if(err) {
                        console.log(err);
                    }
                    console.log("The file was saved!");
                });
            } else {
                console.log("The file exists!");
                fs.appendFile(filename, data + '\n', function(err) {
                    if(err) {
                        console.log(err);
                    }
                    console.log("The file was saved!");
                });
            }
        });
        socket.emit("sentData");
    });
});
