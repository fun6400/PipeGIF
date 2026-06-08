# PipeGIF
GIF decoder piping data from one program to another

GIF-Data -> gifdecoder -> lzwdecoder -> display

The GIF decoder is made with a Stream Parser or Finite State Machine (FSM)
thus reading from the data source is not needed.

Features:
* GIF decoding
* Any data source: const, file, SD-card, network
* Animated GIF
* Interlaced image
* GIF87a & GIF89a
* Transparant
* Buffer: only GIF header and color table
* Any display
