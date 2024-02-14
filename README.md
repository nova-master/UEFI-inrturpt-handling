# UEFI-inrturpt-handling
# UEFI USB Device Interrupt Handling

This UEFI application, written in C, showcases interrupt handling techniques in the Unified Extensible Firmware Interface (UEFI) environment, specifically targeting USB devices. The code sets up a timer-based interrupt to periodically trigger a function responsible for modifying USB device serial numbers. It dynamically updates the serial number buffer while handling interrupts, demonstrating real-time changes. Additionally, the application performs USB device enumeration and retrieves essential device descriptors and serial numbers. With a focus on interrupt-driven operations, this project offers insights into handling asynchronous events in UEFI programming, especially concerning USB device interactions.

## Usage

1. **Build:** Compile the UEFI application.
2. **Load:** Load it onto a UEFI system.
3. **Run:** Execute to observe USB device interrupt handling.

## License

This project is licensed under the [MIT License](LICENSE).
