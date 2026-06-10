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
* initial DLP-oriented device control driver

## Current Module

### SecLabDlp

`SecLabDlp` is the first driver module in the lab.

It focuses on DLP-oriented device control/admission logic and uses the shared kernel/user infrastructure provided by the repository.

## Design Notes

Some utility code is written so the same C++ implementation can be validated in user mode and then compiled into kernel-mode components.

This helps test low-level logic before running it inside a driver.

## Planned Areas

Future modules may include:

**Telemetry / visibility**
* ETW-based telemetry

**Control / enforcement**
* file-system minifilter
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
