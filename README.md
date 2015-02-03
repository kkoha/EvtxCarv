# EvtxCarv
*by Chanung Pak, Jaeman Park, HyeonGyu Jang* 

EvtxCarv is a tool for fragmented Evtx files forensics.

**Supported platforms**

 * Windows (VS 2010) C++

## Usage

Execute `EvtxCarv` to analyze an image file
<pre>
EvtxCarv.exe (-r|-c) 'target image path' 'output path'
Options
    --record   (-r)    : Recover by record
    --complete (-c)    : Recover by chunk
</pre>

Examples of usage
<pre>
EvtxCarv.exe -c c:\\image.raw c:\\output\\
EvtxCarv.exe -r image.raw output
</pre>

## License

[DFRC@KU](https://github.com/kkoha/EvtxCarv/blob/master/COPYING)

## Feedback

Please submit feedback via the EvtxCarv [tracker](https://github.com/kkoha/EvtxCarv/issues)

Author: Chanung Pak (kkoha@msn.com)
