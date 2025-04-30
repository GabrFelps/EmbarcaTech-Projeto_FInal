# LedReflex

LedReflex is a reflex-based game developed for the BitDogLab board, featuring a 5x5 WS2812B LED matrix. Players must quickly count red and blue LEDs lit randomly and respond using buttons under time pressure. The game progressively increases difficulty and provides feedback through an OLED display, buzzer sounds, and real-time updates via Wi-Fi and a Telegram bot.

## Features

- 5x5 WS2812B LED matrix with dynamic red and blue LED patterns
- Timed input using buttons to count LEDs by color
- OLED display for visual feedback and game status
- Buzzer for audio signals on success or failure
- Wi-Fi connectivity for sending progress updates to a Telegram bot
- Increasing difficulty with faster response times and more LEDs lit

## Hardware Requirements

- BitDogLab development board
- WS2812B 5x5 LED matrix
- Buttons A and B for user input
- OLED display
- Buzzer
- Wi-Fi module

## Getting Started

1. Clone this repository:
```bash
git clone https://github.com/GabrFelps/LedReflex.git
```
2. Open the project in your Arduino IDE or compatible environment.
3. Configure Wi-Fi credentials and Telegram bot token in the source code.
4. Compile and upload the firmware to your BitDogLab board.
5. Start playing and monitor your progress via Telegram notifications!

## How to Play

- Observe the LEDs lit on the matrix.
- Count the number of red and blue LEDs.
- Use Button A to input the count of red LEDs.
- Use Button B to input the count of blue LEDs.
- Respond before time runs out to advance to the next round.

## Project Structure

- `src/` – Main source code files for game logic and hardware control
- `assets/` – Configuration files and resources
- `docs/` – Documentation and additional info

## Contributing

Contributions, suggestions, and bug reports are welcome! Feel free to open issues or submit pull requests.

## License

This project is licensed under the MIT License.

---

Developed by GabrFelps  
Feel free to reach out for questions or collaborations!
