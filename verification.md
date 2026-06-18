# Verification Evidence

All outputs below were produced on 2026-06-11 with g++ 13 (host x86-64) and arm-none-eabi-g++ 13.2.1 (ARM). Benchmark numbers vary run to run; the ratios' order of magnitude is stable.

## Host equivalence tests (host-debug)

```
100% tests passed, 0 tests failed out of 9

Total Test time (real) =   0.01 sec
```

## Firmware sections and symbols

```
=== classic firmware (virtual + heap + exceptions) ===
   text	   data	    bss	    dec	    hex	filename
  19248	    120	   1440	  20808	   5148	build/arm/firmware_classic

=== modern firmware (concepts + variant + expected, no heap) ===
   text	   data	    bss	    dec	    hex	filename
   6140	    104	   5240	  11484	   2cdc	build/arm/firmware_modern

=== Section deltas (modern - classic) ===
Section   classic   modern    Delta
.text       19248     6140   -13108
.data         120      104      -16
.bss         1440     5240    +3800

=== exception machinery check ===
classic .ARM.extab/.exidx sections:
  [ 2] .ARM.extab        PROGBITS        000049a4 0059a4 00008c 00   A  0   0  4
  [ 3] .ARM.exidx        ARM_EXIDX       00004a30 005a30 0000e8 00  AL  1   0  4
modern .ARM.extab/.exidx sections:
  [ 2] .ARM.exidx        ARM_EXIDX       000017dc 0027dc 000008 00  AL  1   0  4

classic unwinder symbols: 46
modern unwinder symbols:  0
classic malloc symbols:   1
modern malloc symbols:    0
```

## Per-demo TU sizes (ARM firmware profile)

```
demo          .text C      .text M     .bss C     .bss M
demo01           1320          232          0          0
demo02            400          128          0          0
demo03            365          276          0          0
demo04            993          252          0          0
demo05            136         1088       1025          8
demo06             90           92          0          0
demo07           1078          558          0          0
demo08             40           38          0          0

demo03 storage evolution stages (.text per TU):
  stage 1 vector<unique_ptr<IParticle>>:    945 B
  stage 2 vector<Particle> (AoS):           365 B
  stage 3 Struct-of-Arrays (SoA):           276 B
```

## QEMU runs (both firmwares boot, run all demos, exit cleanly)

```
$ qemu-system-arm -machine mps2-an386 -nographic -semihosting -kernel build/arm/firmware_modern
[modern firmware] start
[modern firmware] done
$ qemu-system-arm -machine mps2-an386 -nographic -semihosting -kernel build/arm/firmware_classic
[classic firmware] start
[classic firmware] done
```

## Benchmarks (host-release, -Os -flto)

```
10000 frames/stream, 100 iterations
corrupt     exceptions ns/f    expected ns/f     ratio
        0%              2.4              1.7      1.4x
        1%              9.3              1.4      6.5x
       10%            104.8              2.6     40.2x
       25%            265.4              2.5    106.5x
       50%            528.6              2.3    227.9x

particles: 4096, iterations: 50000
stage                              total ms    ns/particle   vs stage 1
1: vector<unique_ptr<IParticle>>      423.9          2.070        1.00x
2: vector<Particle> (AoS)             223.0          1.089        1.90x
3: Struct-of-Arrays (SoA)             215.8          1.054        1.96x
```

## Storage evolution at -O3 -march=native (manual build, see README)

```
particles: 4096, iterations: 50000
stage                              total ms    ns/particle   vs stage 1
1: vector<unique_ptr<IParticle>>      299.8          1.464        1.00x
2: vector<Particle> (AoS)             153.8          0.751        1.95x
3: Struct-of-Arrays (SoA)              62.9          0.307        4.77x
```

## demo01 symbol check (heap / vtables / RTTI), ARM firmware profile

```
$ arm-none-eabi-nm demo01_classic.o | grep -E "_Znwj|_ZdlPv|_ZTV"
         U _ZdlPvj
         U _Znwj
         U _ZTVN10__cxxabiv117__class_type_infoE
         U _ZTVN10__cxxabiv120__si_class_type_infoE
00000000 R _ZTVN6demo017classic6CircleE
00000030 R _ZTVN6demo017classic8TriangleE
00000018 R _ZTVN6demo017classic9RectangleE

$ arm-none-eabi-nm demo01_modern.o | grep -cE "_Znwj|_ZdlPv|_ZTV"
0
```
