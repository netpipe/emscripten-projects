# sox.js

An emscripten port of sox for use in the browser and node


## API Ideas

soxox().readFile('/foo.wav').convert({
}).delay({}).lowpass({}).writeFile('/foo.aif');

sox('/foo.wav').mix('/bar.wav').play();

sox('/foo.wav').info(function(err, data) {
	console.log(data);
});


## Building sox-emscripten.js

### Requirements

- [vagrant](http://vagrantup.com)
- [virtualbox](https://www.virtualbox.org/)

### How to build

```bash
vagrant up
vagrant ssh -c "/vagrant/scripts/build_sox.sh"
vagrant halt
```


## Notes

This project was started at
[Music Hack Day San Francisco 2014](https://www.hackerleague.org/hackathons/music-hack-day-san-francisco-2014)


### Helpful Links

- http://www3.ntu.edu.sg/home/ehchua/programming/cpp/gcc_make.html
- http://mozakai.blogspot.com/2012/03/howto-port-cc-library-to-javascript.html


## License
Copyright (c) 2014 skratchdot  
Licensed under the MIT license.
