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

Download file and generate keys

```bash
git clone https://github.com/tinmarino/dev_one.git DevOne && cd DevOne  # 1/ Download
sudo make key     # 2 Generate key for signing
sudo reboot now   # 3 Reboot and enable Mok
  # 1. A blue screen (MOK manager) will appear
  # 2. Choose "Enroll MOK"
  # 3. Choose "Continue"
  # 4. Choose "Yes" (when asked "Enroll the key")
  # 5. Enter the password you gave at make sign
  # 6. Choose "Reboot" (again)
```

Install module in system

```bash
sudo make full
```

In case you want a fast development life cycle, here is how to load module once

```bash
make build        # 4 Compile
sudo make sign    # 5 Sign driver module to permit MOK enforcement (security)
sudo make user_load    # 6 Load
sudo make user_create  # 7 Create /dev/one
make test         # 8 Test if all is ok
```

As usual, to clean your work, run `sudo make clean`


### Util

```bash
# Installed modules, see #7
lsmod  # List modules
sudo modprobe one  # Load one driver => creates /dev/one
sudo depmod  # Re-create the module dependency list
sudo modprobe -r one  # Load one driver => removes /dev/one

# Keys
sudo mokutil --list-new  # List key that will be added at boot
sudo mokutil --reset  # Delete future keys
sudo cat /proc/keys  # View your installed keys
dmesg -wH  # Kernel log like tail -f
```

### Source

*  Device Driver: https://www.apriorit.com/dev-blog/195-simple-driver-for-linux-os
*  Signing driver: https://gist.github.com/Garoe/74a0040f50ae7987885a0bebe5eda1aa
*  Mok: https://ubuntu.com/blog/how-to-sign-things-for-secure-boot


### Licence

This project, DevOne, is licensed under the [GPL v2.0 or later](https://spdx.org/licenses/GPL-2.0-or-later.html)
Copyright &copy; 2020-2022 Martin Tourneboeuf (https://tinmarino.github.io)
