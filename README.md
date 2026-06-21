# EndpointSecurityLab

Windows endpoint security portfolio lab covering ETW-based telemetry, kernel/user-mode telemetry, DLP/device control, app-control, injected sensors, browser/mail integration, syscalls, and hypervisor-assisted techniques.

## Scope

This repository contains a reusable Windows endpoint security foundation:

* Visual Studio / WDK build infrastructure
* test-signing support
* kernel utility libraries
* shared user/kernel utility code
* NT-style wrappers for code reuse between user mode and kernel mode
* CLI runtime and command-table generator
* command-driven test utilities
* DLP-oriented device control driver
* DLP-oriented file-system minifilter driver

## Current Modules

### SecLabDlp

`SecLabDlp` is an existing driver module in the lab.

It implements DLP-oriented USB device control/admission logic and demonstrates endpoint policy enforcement at the device-access level.

### SecLabFltDrv

`SecLabFltDrv` is an existing file-system minifilter module in the lab.

It implements DLP-oriented file-open inspection and can block open/create operations for files that contain data, based on policy decisions returned by the user-mode service.

Both modules use the shared kernel/user infrastructure provided by the repository.

## Design Notes

Some utility code is written so the same C++ implementation can be validated in user mode and then compiled into kernel-mode components.

This helps test low-level logic before running it inside a driver.

## Planned Areas

Future modules may include:

**Telemetry / visibility**
* ETW-based telemetry

**Control / enforcement**
* WFP/network filtering
* application control
* injected user-mode sensors
* browser and mail integration
* syscall-level visibility
* hypervisor-assisted techniques
* anti-tampering primitives

## License

Source available for non-commercial testing and evaluation only.

See `LICENSE`.
