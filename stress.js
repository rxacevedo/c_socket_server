var net = require('net');

var open = 0;

setInterval(function () {
  var socket = net.connect({
   host: '10.0.1.20',
   port: 8080
   // host: 'robertoacevedo.net',
   // port: 9000
  });
  socket.on('connect', function () {
    open++;
    showOpen();
    setTimeout(function () {
    socket.end('data\n');
    }, 500);
    // }, 2000);
  });
  socket.on('end', function () {
    open--;
    showOpen();
  });
}, 200);

var showOpen = function () {
  process.stdout.write('\u001B[2J\u001B[0;0f');
  console.log("Open", open);
};
