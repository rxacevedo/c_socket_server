var net = require('net');

var open = 0;

setInterval(function () {
  var socket = net.connect({
   host: 'arm.robertoacevedo.net',
   port: 8080
  });
  socket.on('connect', function () {
    open++;
    showOpen();
    socket.write('data\n');
  });
  socket.on('end', function () {
    open--;
    showOpen();
  });
}, 20);

var showOpen = function () {
  process.stdout.write('\u001B[2J\u001B[0;0f');
  console.log("Open", open);
};
