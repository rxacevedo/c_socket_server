var net = require('net');

var open = 0;

setInterval(function () {
  var socket = net.connect({
   // Substitute IP/port of machine you run server on
   // Note: EOENT means the FS is no longer allowing node to connect,
   // re-run the script and the server will resume handling requests
   host: 'robertoacevedo.net',
   port: 9000 
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
}, 5);

var showOpen = function () {
  process.stdout.write('\u001B[2J\u001B[0;0f');
  console.log("Open", open);
};
