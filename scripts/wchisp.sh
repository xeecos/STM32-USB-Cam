curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh
brew install libusb
# Ubuntu
sudo apt install libusb-1.0-0-dev

# install wchisp
cargo install wchisp --git https://github.com/ch32-rs/wchisp

# /etc/udev/rules.d/50-wchisp.rules
# SUBSYSTEM=="usb", ATTRS{idVendor}=="4348", ATTRS{idProduct}=="55e0", MODE="0666"
# or replace MODE="0666" with GROUP="plugdev" or something else