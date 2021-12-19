
# Stretcher

A proof-of-concept audio time stretcher plugin using [Rubberband](https://breakfastquay.com/rubberband/).  

[![Chat on Matrix](https://matrix.to/img/matrix-badge.svg)](https://riot.im/app/#/room/#ninjas:matrix.org?action=chat)
[![Sponsor me](https://img.shields.io/github/sponsors/rghvdberg)](https://github.com/sponsors/rghvdberg)

---

## Build instructions

You'll need:  

- libgl1-mesa-dev
- libx11-dev
- libsndfile1-dev
- librubberband-dev
- libjack-jackd2-dev

```bash
git clone --recursive https://github.com/clearly-broken-software/Stretcher.git
cd Stretcher
mkdir build
cd build
cmake ../
make
```

There's no make install, just look in the `build/bin` dir for the binaries and copy them to the correct location on your system.
