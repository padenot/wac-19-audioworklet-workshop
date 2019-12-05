var $ = document.querySelectorAll.bind(document);

$(".source-language")[0].onchange = do_it;

let ctx = null;

function do_it() {
  if (ctx) {
    ctx.close();
  }
  ctx = new AudioContext

  if (ctx.audioWorklet === undefined) {
    alert("no audioworklet")
  } else {
    ctx.audioWorklet.addModule('js/processor.js').then(() => {
      // Use a sine wave so it's easier to hear glitches
      var osc = new OscillatorNode(ctx);
      const n = new AudioWorkletNode(ctx, 'processor');
      osc.connect(n);
      osc.start();
      n.connect(ctx.destination);

      var source_lang = $('.source-language')[0].value;

      var is_wasm = source_lang.indexOf("wasm") != -1;
      console.log("Loading the " + source_lang + " version");
      if (is_wasm) {
        fetch(source_lang)
          .then(r => r.arrayBuffer())
          .then(r => n.port.postMessage({ type: 'load-processor',
            data: r ,
            wasm: source_lang.indexOf("wasm") != -1 }))
      } else {
        fetch(source_lang)
          .then(r => r.text())
          .then(r => n.port.postMessage({ type: 'load-processor',
            data: r ,
            wasm: source_lang.indexOf("wasm") != -1 }))
      }

      const load = $('.load')[0]
      const label = $('.loadLabel')[0]
      load.addEventListener('input', e => {
        label.innerText = e.target.value;
        n.port.postMessage({type: 'set-load', data: e.target.value });
      })
    });
  }
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

do_it();
