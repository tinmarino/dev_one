# File Driver to create a device /dev/one like the /dev/zero

### QuickStart

When installed, running:
```bash
cat /dev/one | hexdump -v
```

Prints:
```text
0000000 ffff ffff ffff ffff ffff ffff ffff ffff
```


### Install

1. `git clone https://github.com/tinmarino/dev_one.git DevOne && cd DevOne  # Download`
2. `make build        # Compile`
3. `make key          # Generate key for signing`
4. `sudo make sign    # Sign driver module to permit MOK enforcement (security)`
5. `sudo reboot now   # Reboot and enable Mok`
    1. A blue screen (MOK manager) will appear
    2. Choose "Enroll MOK"
    3. Choose "Continue"
    4. Choose "Yes" (when asked "Enroll the key")
    5. Enter the password you gave at `make sign`
    6. Choose "Reboot" (again)
6. `sudo make load    # Load`
7. `sudo make create  # Create /dev/one`
8. `make test         # Test if all is ok`


### Source

*  Device Driver: https://www.apriorit.com/dev-blog/195-simple-driver-for-linux-os
*  Signing driver: https://gist.github.com/Garoe/74a0040f50ae7987885a0bebe5eda1aa
*  Mok: https://ubuntu.com/blog/how-to-sign-things-for-secure-boot


### Util

```bash
sudo mokutil --list-new  # List key that will be added at boot
sudo mokutil --reset  # Delete future keys
sudo cat /proc/keys  # View your installed keys
dmesg -wH  # Kernel log like tail -f
```


### Licence

* Author: Tinmarino
* Date: 2020-10-12
* License: GPL => Open Source
