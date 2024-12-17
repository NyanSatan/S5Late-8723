# S5Late-8723

***Important note**: this is a preliminary port of the exploit, use it on your own risk and always validate results it gives you*

***Important note** (2): although this code is unlikely to permanently break anything, you still should be very careful. I'm not responsible for any possible damage this might cause*

Implementation of [S5Late exploit](https://github.com/m-gsch/S5Late) by **@__gsch** for S5L8723 SoC which is used in iPod nano 6. It allows you to:

* Dump arbitrary memory
* Decrypt Image1-wrapped firmwares
    * ...as well as decrypt/encrypt arbitrary data with embedded GID key
* Boot custom 2nd-stage images

I only tested this on a M4 Max MacBook Pro & MacBook 12-inch, but given that it doesn't use any special USB haxx, it should run anywhere

## How to run
### Prerequisites

* iPod nano 6

* Python 3
    * `pyusb` is the only external dependency
        * Available from **pip**

### Entering DFU on iPod nano 6

Press and hold volume down and power buttons at the same time until Apple logo appears. Keep holding until screen goes black + 1-2 seconds

As soon as Finder/iTunes detects an iPod in DFU mode, it will try to download a recovery image and upload **WTF** from it

We want to avoid it - you can either send SIGSTOP signal to `AMP*` processes on your Mac, or just temporarily disable Internet connection (so it cannot download the recovery image)

You know you did everything right if you see this on USB:

```
USB DFU Device:

  Product ID:	0x1232
  Vendor ID:	0x05ac (Apple Inc.)
  Version:	0.01
  Serial Number:	87230000000001
  Speed:	Up to 480 Mb/s
  Manufacturer:	Apple, Inc.
  Location ID:	0x02100000 / 1
  Current Available (mA):	500
  Current Required (mA):	100
  Extra Operating Current (mA):	0
```

### Running the exploit

When your iPod is in DFU mode, just run the following:

```
➜  S5Late-8723 git:(master) ✗ ./S5Late pwn                                                                           
DONE
```

You know it reached pwned DFU mode if you see this on USB:

```
S5L8723 pwnDFU:

  Product ID:	0x1232
  Vendor ID:	0x05ac (Apple Inc.)
  Version:	0.01
  Serial Number:	87230000000001
  Speed:	Up to 480 Mb/s
  Manufacturer:	Apple, Inc.
  Location ID:	0x02100000 / 1
  Current Available (mA):	500
  Current Required (mA):	100
  Extra Operating Current (mA):	0
```

Device name is now changed to `S5L8723 pwnDFU`

### Sample operations

Dumping ROM:

```
➜  S5Late-8723 git:(master) ✗ ./S5Late dump /tmp/rom.bin 0x20000000 0x10000
dumping: 100%
succesfully dumped 65536 bytes in 4.958 seconds (13218.303 bytes/sec)
DONE
```

Dumping SRAM:

```
➜  S5Late-8723 git:(master) ✗ ./S5Late dump /tmp/sram.bin 0x22000000 0x30000
dumping: 100%
succesfully dumped 196608 bytes in 14.851 seconds (13239.106 bytes/sec)
DONE
```

Decrypting Image1:

```
➜  S5Late-8723 git:(master) ✗ ./S5Late image1 N20.bootloader.release.rb3 /tmp/n20_boot.bin
Image1 v2.0 (8723): type: 0x3 entry: 0x0 bodylen: 0x23b00 datalen: 0x24763 certoff: 0x23b80 certlen: 0xbe3
decrypting: 100%
succesfully decrypted 146176 bytes in 15.299 seconds (9554.336 bytes/sec)
DONE
```

Rebooting back into normal DFU:

```
➜  S5Late-8723 git:(master) ✗ ./S5Late reboot                                                                           
DONE
```

Booting a 2nd-stage bootloader:

```
➜  S5Late-8723 git:(master) ✗ ./S5Late boot /tmp/n20_wtf.bin
DONE
```

## Running your own code

Initial payload source code as well as DFU upload callback source code are available in this repository

You can modify them and recompile (requires ARM GNU toolchain)

## Precautions

This is a preliminary version of this exploit, so be careful with results yielded by dumping and AES operations

## Known issues

* AES operations are very slow - decrypting large payloads such as OSOS/retailOS (~9MB) takes around 15-25 minutes

* Apparently padding in AES operations isn't handled properly, so for inputs not aligned to 16 bytes (AES block size) it might yield some garbage in the end of an output

* Current implementation only boots EFI images, but this can be fixed very easily by modifying the hook code

## Credits

* @__gsch - for the original bug & exploit
* q3k - for sharing a lot of research on iPod bootroms and helping me
