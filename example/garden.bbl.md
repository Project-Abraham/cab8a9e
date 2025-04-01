# .bbl
Brush Based Level


## Overview

Stores blocks of data for defining a level from generic parts
Certain parts are assumed to go together
e.g. Binary Brush, + BrshSide

A `PlainTxt` `Entities` lump would represent a full `.map`
`BrshSide` w/ 0 length could state variant to help parser
or `Txt V220`

> TODO: compare Binary vs PlainTxt
> -- file size, write time & read time

> TODO: reference external file
> -- edit history checkpoints (e.g. A1 release)
> -- instances

> TODO: groups & visgroups (Source .vmf feature)

### Lump Relationships

```
Entity (Binary) -> Brush (Binary) -> BrshSide (Bin*) -> Plane (Ratio) -> Ratio
               \-> KeyValue                         \-> Material
```
`PackFile` is for sharing assets
`EditHist` is for CRDT (allows for multi-user edits)
Constructive R? Deterministic? T?

## Header

### Format ID

```
     | hexmagic | txtmagic | vers | numlumps | birthday |
     | 0x00..04 | 0x04..0C | 0x0C | 0x10..14 | 0x14..18 |
0x00 | CAB8A9E0 | CABBAGE  | v1.0 | 00000007 | YYYYMMDD |
```


### Lump Headers

```
     | lumpname | offset   | length   | variant  |
     | 0x00..08 | 0x08..0C | 0x0C..10 | 0x10..18 |
0x18 | Brushes  | 00000100 | 00000020 | Binary   |
0x30 | BrshSide | 00000120 | 00000600 | Valve220 |
0x48 | Material | 00000720 | 00000371 | PlainTxt |
0x60 | KeyValue | 00000A94 | 00004C8D | PlainTxt |
0x78 | Entities | 00005000 | 00000800 | Binary   |
0x90 | EditHist | 00005800 | 0000BEEF | CRDTv1.0 |
0xA8 | PackFile | 0000FFFF | 00007777 | Deflate  |
```


## Lumps

### Brush

`Binary `

```
mins, maxs, first side, num sides
```

### Brush Side

`Valve220` / `Bin V220`

> NOTE: texture projection is different depending on variant
> -- we will need cross-conversion functions for each variant

```
material, plane, texture projection
```

### Material

```
string blob (zero-terminated list of strings)
```


### Plane

`BinRatio`

```
origin, x:y:z
```
