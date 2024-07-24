<h1 align="center">
<br>
<img src="Tier.PNG", width="300" height="300">
<br>
</h1>

<h2> Tiburon </h2>

Tiburon is a process hollower that currently supports ghostly hollowing. 

### Features

- Tiburon creates two pipes which serve as the std I/O pipes for the child process. As two threads are responsible for redirecting user I/O to said pipes, it is possible to interact with the hollowed process in a seamless manner.
