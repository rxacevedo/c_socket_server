#!/usr/bin/node
var net = require('net'),
    os = require('os');

var host = "localhost",
    port = 8080;

if (process.argv[1])
  host = process.argv[1];

if (process.argv[2])
  port = parseInt(process.argv[2], 10);

var open = 0;

setInterval(function () {

  var socket = net.connect({
   host: host,
   port: port
  });

  socket.on('connect', function () {
    open++;
    showOpen();
    setTimeout(function () {
    socket.end('data\n');
    }, 500);
  });

  socket.on('data', function (d) {
    console.log('Got data: ', d);
  });

  socket.on('end', function () {
    open--;
    showOpen();
  });
}, 200);

var showOpen = function () {
  process.stdout.write('\u001B[2J\u001B[0;0f');
  console.log("Open connection count: ", open);
};
