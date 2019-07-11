# CS:GO Memory Reading

I would like to start out by saying this part of the project is very varied in its uses so feel free to adapt it into your personal projects.
Note a few things: this is *not* intended to be used as a tool for hacking. If you blindly use these functions in a VAC-(or anti-cheat)-secured 
server, you will **almost certainly get detected** and (hopefully) receive a ban. As a result of this, I highly recommend running `-insecure` as 
a CS:GO launch option and running on private, local/ personally owned servers. Running this command disables VAC so it should not be an issue but 
it stops you from joining anything VAC protected. 

Another important thing to bare in mind is that these offsets may be (almost certainly are) out-of-date. This should not be too much of an issue,
simply search up "*csgo offset dumps*" and replace relevant values. There will be some undoubtedly that are not included in public dumps but that I 
included in my project. Unfortunately, unless you can find other sources online, you will have to boot up cheat engine and go digging yourself (if you
have never used cheat engine before, look up guides on youtube etc it is not too bad once you get the hang of it). I remember that the Z-value was almost 
"bugged" in certain cases so be especially diligent when it comes to that (something about cl_viewpos 2 instead of 1? Check my code). 

Aside from that, I took special care in making this Python compatible and you *should* be able to "plug-and-play" although I cannot remember if any extraneous
steps are required. I believe it is dependant highly on the build environment (I was using visual studio) but you should get it working after some googling and
fiddling.

Most of the functions offered are "getters" or "readers" as in they will just read the memory locations. There are two setters for controlling player view angles.
I believe certain other inputs, such as crouching or jumping (important for bunnyhopping) can be easily accessed but something like movement is not so trivial in my 
experience. Of course, it depends highly on the context, but I would recommend just using some sort of keyboard hook library for movement. 

Gl;hf :)!
