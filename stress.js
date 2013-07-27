var net = require('net');

var open = 0;

// Note: EOENT means the FS is no longer allowing node to connect,
// re-run the script and the server will resume handling requests
setInterval(function () {
  var socket = net.connect({
    host: process.argv[2],
      port: process.argv[3]
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
}, 10);

var showOpen = function () {
  process.stdout.write('\u001B[2J\u001B[0;0f');
  console.log("Open", open);
};
