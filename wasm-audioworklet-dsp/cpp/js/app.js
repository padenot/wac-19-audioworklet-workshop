var $ = document.querySelectorAll.bind(document);
const ctx = new AudioContext()
if (ctx.audioWorklet === undefined) {
  alert("no audioworklet")
} else {
  fetch('audio-samples/Alzir-Break-Mono.wav').then(raw => raw.arrayBuffer()).then(b => ctx.decodeAudioData(b)).then(function(audiobuffer) {
    ctx.audioWorklet.addModule('js/processor.js').then(() => {
      // Use a sine wave so it's easier to hear glitches
      var source = new AudioBufferSourceNode(ctx);
      source.buffer = audiobuffer;
      source.start();
      source.loop = true;
      const n = new AudioWorkletNode(ctx, 'processor');
      source.connect(n);
      n.connect(ctx.destination);

      fetch("wasm/wasm_audioworklet.wasm")
        .then(r => r.arrayBuffer())
        .then(r => n.port.postMessage({ type: 'load-processor', data: r}));
    });
  })
}

var gc_pressure = false;
var gc = $(".gc")[0];

gc.onclick = function() {
  if (!gc_pressure) {
    gc.innerText = "Stop generating garbage";
    gc_pressure = true;
  } else {
    gc.innerText = "Start generating garbage";
    gc_pressure = false;
  }
}

var array = [];

function render() {
  if (gc_pressure) {
    // Ring buffer of big allocations, one allocation per frame.
    array.push(new Float32Array(100000 + Math.floor(1000000 * Math.random())));
    if (array.length > 15) {
      array.splice(0, 1);
    }
    console.log("Latest Len: " + array[array.length - 1].length);
  }
  requestAnimationFrame(render);
}
requestAnimationFrame(render);


var start = $(".start")[0];

start.onclick = function() {
  if (ctx.state == "running") {
    ctx.suspend();
    start.innerText = "Start";
  } else {
    ctx.resume();
    start.innerText = "Stop";
  }
}
