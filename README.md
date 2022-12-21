# Matrix Shooter - V1.0
## About:
Matrix Shooter is a 2D top-down shooter game designed on an Arduino Uno board using an 8x8 LED Matrix. The point of the game is for the player to shoot down as many enemies as they can, infinitely, going for the highest score possible while also avoiding fire from them. The player can use powerups to help themselves even the fight out against the enemies. Game becomes more difficult as the levels increase but a separate difficulty can also be selected. 

## Components:
* Arduino Uno Board
* Breadboard
* Small breadboard
* Joystick
* LCD Display
* 8x8 LED Matrix
* MAX7219 Driver
* 1 ceramic capacitor of 104 pF
* 1 electrolytic capacitor of 10 μF
* 1 10k Ohm rezistor - Driver
* 1 220 Ohm rezistor - LCD
* a lot of wires

## How to use:
### 1. Navigating the menus:
Turn the joystick up / down to navigate through menus.<br>
Press the joystick to enter in the menu.<br>
When in the About menu, press the joystick again to exit.<br>
Adjust selected setting by turning the joystick left or right.<br><br>

### 2. Playing the game:
Left / Right - move the player
Press - shoot a projectile
Hold - use stored powerup
<br><br>
### 3. Ending the game:
The game automatically ends when the player runs out of lives.
<br><br>
## In-game information:
The following information will be displayed on the LCD Display:
* HP - Health Points, amount of lives
* Lvl - current level
* Pts - current amount of points
* powerup icon - shows as X if there is no powerup stored or a specific icon depending on the powerup

Types of powerups:
1. One Life Up - when atleast one life is lost, a one life up powerup can spawn, increasing the current lives by one. Cannot be stored.
2. Bomb - using the bomb wipes out the entire screen of enemies and projectiles
3. Invicibility - makes the player invincible for a couple of seconds
4. Auto Shoot - game shoots for the player increasing efficiency

## Tehnical implementation:
Game functions on a per-second scan to spawn new enemies, powerups and to move the enemies. Projectiles are timed independently from the scan in a set speed based on the selected game difficulty. 

## Showcase video: [Matrix Shooter - V1.0](https://youtu.be/oumOWZD3_AE)

## Pictures of the setup: [Pictures Album](https://imgur.com/a/nXsVNII)
![circuit setup](https://i.imgur.com/vk1NrQW.jpg)

## Known Bugs:
- about menu line shows a down arrow even though it's only one total line
- menu doesn't update when game starts but will update at points / level / life alteration - possible interference or something
- LCD flickering annoyingly while in-game
- pressing the button while on auto shoot instantly uses the stored powerup
- bomb powerup is badly designed
- enemies sometimes get stuck - no idea why
- enemies move and shoot extremely random


## To Do List:
- hi scores menu (almost out of mem)
- proper player name
- proper game over
- proper game intro
- sound
- a way to force exit the game instead of hard reseting
- more randomized enemy shooting, they seem to synchronize a lot
- Arduino controlled LCD backlight
