<h1 align="center">
<br>
<img src="Tier.PNG", width="300" height="300">
<br>
</h1>

Tiburon is a process hollower that currently supports ghostly hollowing. For the sake of simplicity, the payload is written to a temporary file which is subsequently mapped. Ideally you would fetch it out of the .rsrc section, or over the internet.

### Features

- Tiburon creates two pipes which serve as the std I/O pipes for the child process. As two threads are responsible for redirecting user I/O to said pipes, it is possible to interact with the hollowed process in a seamless manner.
- The original image is not unmapped.

### Example usage

```
.\Tiburon.exe C:\Tools\mimikatz.exe C:\Windows\System32\RuntimeBroker.exe
```

### Detection

- Section created based off a delete-pending file.
- Comparing the mapped image with the one on disk.

An alternative approach would be to drop a legitimate executable, overwrite it with your payload, leveraging COW, before finally reverting it to its original content. 

#### References

This wonderful header file: https://github.com/mrexodia/phnt-single-header

The originals: https://github.com/Hagrid29/herpaderply_hollowing, https://github.com/hasherezade/transacted_hollowing
