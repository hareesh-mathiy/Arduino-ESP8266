var express = require("express");
var app = express();
var http = require("http").Server(app);
var io = require("socket.io")(http);
var port = process.env.PORT || 3030;
var fs = require('fs');
var net = require('net');

console.log('Server started.');

var date;
var times = [];
var lastMinute = -1;
var currentState = 0;
var sockets = {};

var server = net.createServer(function(socket) {
    date = new Date().toUTCString();
    var str = "[" + date + "] " + "connected" + "\r\n";
    console.log(str);
    sockets["curr"] = socket;

    socket.on('data', function(data) {
        date = new Date();
        var day = date.getDay();
        var hour = date.getHours();
        var minute = date.getMinutes();

        var formattedDate = ToDateString(day) + " " + hour + ":" + minute;

        if (data.indexOf("off") > -1) {
            console.log("[" + formattedDate + "] LED turned off.");
            var time = {
                day: day,
                hour: hour,
                min: minute,
                state: 0
            };
            times.push(time);
            currentState = 0;
        } else if (data.indexOf("on") > -1) {
            console.log("[" + formattedDate + "] LED turned on.");
            var time = {
                day: day,
                hour: hour,
                min: minute,
                state: 1
            };
            times.push(time);
            currentState = 1;
        }

        LogTimes();
    });


    function lightLoop() {
        var currentTime = new Date();
        var currentHour = currentTime.getHours();
        var currentMinute = currentTime.getMinutes();
        var currentDay = currentTime.getDay();
        if (lastMinute != currentMinute) {
            lastMinute = currentMinute;
            console.log("Checking Time");
            console.log("Current: " + currentDay + " " + currentHour + ":" + currentMinute);
            for (var key in times) {
                console.log(times[key].day + " " + times[key].hour + ":" + times[key].min + "|" + times[key].state);
                if (times[key].hour == currentHour &&
                    times[key].min == currentMinute &&
                    times[key].day == currentDay) {
                    if (times[key].state == 1) {
                        console.log("Send turn on LED");
                        socket.write('on');
                    } else if (times[key].state == 0) {
                        console.log("Send turn off LED");
                        socket.write('off');
                    }
                }
            }
        }
    }

    lightLoop();
    setInterval(lightLoop, 1000);
});

server.listen(3002);



http.listen(port, function() {
    console.log("Listening on ", port);
});
console.log('Socket server started.');

io.sockets.on('connection', function(socket) {

    console.log("App connected");

    socket.on('toggleLight', function(data) {
        console.log("Toggling light.");
        if (currentState == 0) {
          currentState = 1;
          sockets["curr"].write('on');
        }
        else if (currentState == 1) {
          currentState = 0;
          sockets["curr"].write('off');
        }
    });

    socket.on('sendTime', function(data) {
        console.log(data);
        //Format: "Monday|7:30AM|9:30PM"
        var info = data.split("|");
        var day = info[0];
        var leave = info[1];
        var ret = info[2];
        var leaveHour;
        var leaveMin;
        var retHour;
        var retMin;

        if (leave.indexOf("AM") > -1) {
            var leaveBase = leave.substring(0, leave.length - 2);
            var leaveArr = leaveBase.split(":");
            leaveHour = parseInt(leaveArr[0]);
            leaveMin = parseInt(leaveArr[1]);
        } else if (leave.indexOf("PM") > -1) {
            var leaveBase = leave.substring(0, leave.length - 2);
            var leaveArr = leaveBase.split(":");
            leaveHour = parseInt(leaveArr[0]);
            leaveMin = parseInt(leaveArr[1]);

            leaveHour = leaveHour + 12;
        }

        var time = {
            day: ToDateInt(day),
            hour: leaveHour,
            min: leaveMin,
            state: 0
        };
        times.push(time);


        if (ret.indexOf("AM") > -1) {
            var retBase = ret.substring(0, ret.length - 2);
            var retArr = retBase.split(":");
            retHour = parseInt(retArr[0]);
            retMin = parseInt(retArr[1]);
        } else if (ret.indexOf("PM") > -1) {
            var retBase = ret.substring(0, ret.length - 2);
            var retArr = retBase.split(":");
            retHour = parseInt(retArr[0]);
            retMin = parseInt(retArr[1]);

            retHour = retHour + 12;
        }

        var time = {
            day: ToDateInt(day),
            hour: retHour,
            min: retMin,
            state: 1
        };
        times.push(time);

        LogTimes();
        LogData(1, "You will be gone from " + leaveHour + ":" + leaveMin + " to " + retHour + ":" + retMin + " on " + day + ".");
        socket.emit("sentTime");
    });

    socket.on('disconnect', function() {
        console.log("App disconnected");
    });
});



//Day Conversion
{
    function ToDateString(day) {
        var dayString;

        switch (day) {
            case 0:
                dayString = "Sunday";
                break;
            case 1:
                dayString = "Monday";
                break;
            case 2:
                dayString = "Tuesday";
                break;
            case 3:
                dayString = "Wednesday";
                break;
            case 4:
                dayString = "Thursday";
                break;
            case 5:
                dayString = "Friday";
                break;
            case 6:
                dayString = "Saturday";
                break;
        }

        return dayString;
    }

    function ToDateInt(day) {
        var dayInt;

        switch (day) {
            case "Sunday":
                dayInt = 0;
                break;
            case "Monday":
                dayInt = 1;
                break;
            case "Tuesday":
                dayInt = 2;
                break;
            case "Wednesday":
                dayInt = 3;
                break;
            case "Thursday":
                dayInt = 4;
                break;
            case "Friday":
                dayInt = 5;
                break;
            case "Saturday":
                dayInt = 6;
                break;
        }

        return dayInt;
    }
}


//File IO
{
    function LogTimes() {
        var logdata = "";
        for (var key in times) {
            logdata +=
                ToDateString(times[key].day) + " " +
                times[key].hour + ":" +
                times[key].min + "- " +
                times[key].state + "\r\n";
        }

        LogData(2, logdata);
    }

    var logcode = {
        USERINPUT: 1,
        BUTTONPRESS: 2
    };

    function LogData(code, data) {
        var filename = "EDPLogs/";

        switch (code) {
            case logcode.USERINPUT:
                filename += "userinput.txt";
                date = new Date().toUTCString();
                var str = "[" + date + "] " + data + "\r\n";
                fs.appendFile(filename, str, function(err) {
                    if (err) {
                        console.error("Error writing to " + filename);
                    }
                });
                break;
            case logcode.BUTTONPRESS:
                filename += "buttonpress.txt";
                fs.writeFile(filename, data, function(err) {
                    if (err) {
                        console.error("Error writing to " + filename);
                    }
                });
                break;
        }
    }
}
