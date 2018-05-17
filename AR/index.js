var express = require('express');
var app = express();
var path = require('path');

app.use(express.static(path.join(__dirname)));
app.use("/styles", express.static(__dirname));
app.use("/data", express.static(__dirname + '/data'));
app.use("/js", express.static(__dirname + '/js'));
app.use("/obj", express.static(__dirname + '/obj'));
app.use("/stl", express.static(__dirname + '/stl'));

// viewed at based directory http://localhost:8080/
app.get('/', function (req, res) {
  res.sendFile(path.join(__dirname + 'index.html'));
});

app.get('/pef', function (req, res) {
  res.sendFile(path.join(__dirname + '/vr.html'));
});

app.get('/work', function (req, res) {
  res.sendFile(path.join(__dirname + '/vr-work.html'));
});

app.listen(process.env.PORT || 8080);