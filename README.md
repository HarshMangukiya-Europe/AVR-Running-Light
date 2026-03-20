# AVR Running Light

## Overview
A responsive LED control system programmed in AVR-C on an Arduino Nano.
The system controls 8 LEDs across PORTB and PORTD using three buttons
and interrupt-driven logic.

## Features
- Button S1: Triggers sequential LED pattern (500ms delay)
- Button S2: Changes delay to 200ms for faster sequence
- Button S3: Interrupt that disables outer LEDs (PD5, PB4)

## Hardware
- Arduino Nano (ATmega328P)
- 8 LEDs across PORTB and PORTD
- 3 push buttons (S1, S2, S3)
- Breadboard + resistors

## Tools Used
- AVR-C (no Arduino libraries)
- AVR Studio / Microchip Studio
- Arduino Nano

## Demo Video
[![Watch on YouTube](https://img.shields.io/badge/YouTube-Watch%20Demo-red?logo=youtube)](https://youtube.com/shorts/mufAcJLIRyg?si=tJaTyW3kexhBAo4a)
